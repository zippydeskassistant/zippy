#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "lvgl.h"

#include "ui/ui.h"
#include "ui/topbar/topbar_time.h"

extern struct k_mutex ui_mutex;

int topbar_time_set(struct raw_hid_packet *packet) {
    uint8_t hour = packet->data[0];
    uint8_t minute = packet->data[1];
    uint8_t am_pm = packet->data[2];
    k_mutex_lock(&ui_mutex, K_FOREVER);
    lv_label_set_text_fmt(time_label, "%02d:%02d %s", hour, minute, am_pm ? "PM" : "AM");
    k_mutex_unlock(&ui_mutex);
    return 0;
}
