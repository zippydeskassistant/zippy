#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/usb/class/usbd_msc.h>
#include <zephyr/fs/fs.h>

#include <zephyr/storage/flash_map.h>
#include <ff.h>

LOG_MODULE_REGISTER(zippy_msc);

USBD_DEFINE_MSC_LUN(NAND, "Zephyr", "FlashDisk", "0.00");

#define STORAGE_PARTITION		storage_partition
#define STORAGE_PARTITION_ID		FIXED_PARTITION_ID(STORAGE_PARTITION)

static struct fs_mount_t fs_mnt;

static int setup_flash(struct fs_mount_t *mnt)
{
	int err = 0;
#if CONFIG_DISK_DRIVER_FLASH
	unsigned int id;
	const struct flash_area *pfa;

	mnt->storage_dev = (void *)STORAGE_PARTITION_ID;
	id = STORAGE_PARTITION_ID;

	err = flash_area_open(id, &pfa);
	LOG_INF("Area %u at 0x%x on %s for %u bytes",
	       id, (unsigned int)pfa->fa_off, pfa->fa_dev->name,
	       (unsigned int)pfa->fa_size);

	if (err < 0) {
		LOG_INF("Erasing flash area ... ");
		err = flash_area_erase(pfa, 0, pfa->fa_size);
		LOG_INF("%d", err);
	}

	if (err < 0) {
		flash_area_close(pfa);
	}
#endif
	return err;
}

static int mount_app_fs(struct fs_mount_t *mnt)
{
	int err;

#if CONFIG_FAT_FILESYSTEM_ELM
	static FATFS fat_fs;

	mnt->type = FS_FATFS;
	mnt->fs_data = &fat_fs;
	if (IS_ENABLED(CONFIG_DISK_DRIVER_RAM)) {
		mnt->mnt_point = "/RAM:";
	} else if (IS_ENABLED(CONFIG_DISK_DRIVER_SDMMC)) {
		mnt->mnt_point = "/SD:";
	} else {
		mnt->mnt_point = "/NAND:";
	}

#endif
	err = fs_mount(mnt);

	return err;
}

static int msc_init(void)
{
	struct fs_mount_t *mp = &fs_mnt;
	struct fs_dir_t dir;
	struct fs_statvfs sbuf;
	int err;

	fs_dir_t_init(&dir);

	if (IS_ENABLED(CONFIG_DISK_DRIVER_FLASH)) {
		err = setup_flash(mp);
		if (err < 0) {
			LOG_ERR("Failed to setup flash area");
			return err;
		}
	}

	if (!IS_ENABLED(CONFIG_FAT_FILESYSTEM_ELM)) {
		LOG_ERR("No file system selected");
		return err;
	}

	err = mount_app_fs(mp);
	if (err < 0) {
		LOG_ERR("Failed to mount filesystem");
		return err;
	}

	/* Allow log messages to flush to avoid interleaved output */
	k_sleep(K_MSEC(50));

	LOG_INF("Mount %s: %d", fs_mnt.mnt_point, err);

	err = fs_statvfs(mp->mnt_point, &sbuf);
	if (err < 0) {
		LOG_ERR("FAIL: statvfs: %d", err);
		return err;
	}

	LOG_INF("%s: bsize = %lu ; frsize = %lu ;"
	       " blocks = %lu ; bfree = %lu",
	       mp->mnt_point,
	       sbuf.f_bsize, sbuf.f_frsize,
	       sbuf.f_blocks, sbuf.f_bfree);

	err = fs_opendir(&dir, mp->mnt_point);
	LOG_INF("%s opendir: %d", mp->mnt_point, err);

	if (err < 0) {
		LOG_ERR("Failed to open directory");
	}

	while (err >= 0) {
		struct fs_dirent ent = { 0 };

		err = fs_readdir(&dir, &ent);
		if (err < 0) {
			LOG_ERR("Failed to read directory entries");
			break;
		}
		if (ent.name[0] == 0) {
			LOG_INF("End of files");
			break;
		}
		LOG_INF("  %c %u %s",
		       (ent.type == FS_DIR_ENTRY_FILE) ? 'F' : 'D',
		       ent.size,
		       ent.name);
	}

	(void)fs_closedir(&dir);

	return err;
}

SYS_INIT(msc_init, APPLICATION, CONFIG_ZIPPY_USBD_MSC_INIT_PRIORITY);