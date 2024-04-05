#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

LOG_MODULE_REGISTER(tab_audio_controller);

extern struct k_mutex lvgl_init_mutex;

void update_tab_audio_controller(struct k_work *work) {}

K_WORK_DEFINE(work_tab_audio_controller, update_tab_audio_controller);

void zbus_listener_tab_audio_controller(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(audio_controller_listener, zbus_listener_tab_audio_controller);
