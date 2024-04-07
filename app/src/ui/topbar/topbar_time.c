#include <zephyr/logging/log.h>
#include "lvgl.h"

#include "ui/ui.h"
#include "ui/topbar/topbar_time.h"

int time_set(struct raw_hid_packet *packet) {
    uint8_t hour = packet->data[0];
    uint8_t minute = packet->data[1];
    uint8_t am_pm = packet->data[2];

    lv_label_set_text_fmt(time_label, "%02d:%02d %s", hour, minute, am_pm ? "PM" : "AM");
    return 0;
}