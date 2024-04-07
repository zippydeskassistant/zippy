#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>
#include <ff.h>

#include "zippy/zippy_msg.h"

#include "features/grub_commander/grub_commander.h"

LOG_MODULE_REGISTER(grub_commander);

ZBUS_CHAN_DEFINE(grub_commander_chan,
                 struct gc_message,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(grub_commander_tab, grub_commander_feature),
                 ZBUS_MSG_INIT(0)
);

#define GRUB_CFG_PATH "/NAND:/boot_opt.cfg"

#define OS_HW_SWITCH_OPTIONS 4

char os_hw_switch_options[OS_HW_SWITCH_OPTIONS][32] = {
    "set os_hw_switch=0\n",
    "set os_hw_switch=1\n",
    "set os_hw_switch=2\n",
    "set os_hw_switch=3\n",
};

static int grub_write_boot_opt(int index) {
    if (index > OS_HW_SWITCH_OPTIONS) {
        return -EINVAL;
    }

    struct fs_file_t file;
    fs_file_t_init(&file);

    int err = fs_open(&file, GRUB_CFG_PATH, FS_O_CREATE | FS_O_WRITE);
    fs_write(&file, os_hw_switch_options[index], strlen(os_hw_switch_options[index]));
    fs_close(&file);
    return err;
}

int grub_get_boot_opt(uint16_t *index) {
    struct fs_file_t file;
    fs_file_t_init(&file);

    int err = fs_open(&file, GRUB_CFG_PATH, FS_O_READ);
    if (err) {
        index = 0;
        return err;
    }

    char buf[32];
    fs_read(&file, buf, sizeof(buf));
    fs_close(&file);

    for (int i = 0; i < OS_HW_SWITCH_OPTIONS; i++) {
        if (strncmp(buf, os_hw_switch_options[i], strlen(os_hw_switch_options[i])) == 0) {
            *index = i;
            return 0;
        }
    }

    return -EINVAL;
}

static int grub_commander_init(void) {
    int err;

    uint16_t index;
    err = grub_get_boot_opt(&index);
    if (err) {
        LOG_ERR("Failed to get boot option: %d", err);

        err = grub_write_boot_opt(index);
        if (err) {
            LOG_ERR("Failed to write boot option: %d", err);
            return err;
        }
    }

    return 0;
}

SYS_INIT(grub_commander_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void zbus_cb_grub_commander_feature(const struct zbus_channel *chan) {
    int err;

    const struct gc_message *selected_option = zbus_chan_const_msg(chan);

    if (selected_option->type == MSG_TYPE_UI) {
        uint16_t index = selected_option->index;

        err = grub_write_boot_opt(index);
        if (err) {
            LOG_ERR("Failed to write boot option: %d", err);
        }
        else {
            LOG_INF("FEATURE: Selected option: %d", selected_option->index);
        }
    }
}

ZBUS_LISTENER_DEFINE(grub_commander_feature, zbus_cb_grub_commander_feature);

