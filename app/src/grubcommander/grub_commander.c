#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

LOG_MODULE_REGISTER(grub_commander);

struct grub_commander_data {
    uint8_t dummy;
};

ZBUS_CHAN_DEFINE(grub_commander_chan,
                 struct grub_commander_data,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(grub_commander_listener),
                 ZBUS_MSG_INIT(0),
);



