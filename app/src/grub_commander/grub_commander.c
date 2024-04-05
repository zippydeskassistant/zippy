#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/fs/fs.h>
#include <ff.h>

#include "grub_commander.h"

LOG_MODULE_REGISTER(grub_commander);

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

static int grub_commander_init(void) {
    int err;

    err = grub_write_boot_opt(0);
    if (err) {
        LOG_ERR("Failed to write boot option: %d", err);
        return err;
    }

    return 0;
}

SYS_INIT(grub_commander_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static void zbus_cb_grub_commander(const struct zbus_channel *chan) {
    int err;

    const struct gc_index_message *selected_option = zbus_chan_const_msg(chan);

    uint16_t index = selected_option->index;

    err = grub_write_boot_opt(index);
    if (err) {
        LOG_ERR("Failed to write boot option: %d", err);
    }
    else {
        LOG_INF("Selected option: %d", selected_option->index);
    }
}

ZBUS_LISTENER_DEFINE(grub_commander_listener, zbus_cb_grub_commander);

