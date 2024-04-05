#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

LOG_MODULE_REGISTER(tab_skeleton);

extern struct k_mutex lvgl_init_mutex;

void update_tab_skeleton(struct k_work *work) {}

K_WORK_DEFINE(work_tab_skeleton, update_tab_skeleton);

void zbus_listener_tab_skeleton(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(skeleton_listener, zbus_listener_tab_skeleton);