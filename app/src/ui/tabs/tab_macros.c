#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

LOG_MODULE_REGISTER(tab_macros);

extern struct k_mutex lvgl_init_mutex;

void update_tab_macros(struct k_work *work) {}

K_WORK_DEFINE(work_tab_macros, update_tab_macros);

void zbus_listener_tab_macros(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(macros_listener, zbus_listener_tab_macros);