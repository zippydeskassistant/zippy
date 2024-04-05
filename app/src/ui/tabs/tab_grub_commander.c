#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

#include "grub_commander.h"

#include "tabs.h"

#include "ui.h"

extern lv_obj_t *tabview;
LOG_MODULE_REGISTER(tab_grub_commander);

ZBUS_OBS_DECLARE(grub_commander_listener);

ZBUS_CHAN_DEFINE(grub_commander_chan,
                 struct gc_index_message,
                 NULL,
                 NULL,
                 ZBUS_OBSERVERS(grub_commander_listener),
                 ZBUS_MSG_INIT(0)
);

static void grub_commander_roller_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        struct gc_index_message selected_option;
        selected_option.index = lv_roller_get_selected(roller);

        zbus_chan_pub(&grub_commander_chan, &selected_option, K_NO_WAIT);

        LOG_INF("Selected option: %d", selected_option.index);
    }
}

int grub_commander_tab_init(void) {
    LOG_INF("Initializing GRUB Commander tab");
    lv_obj_t *grub_commander_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_LIST);
    lv_obj_set_size(grub_commander_tab, LV_PCT(100), LV_PCT(90));

    lv_obj_t *roller = lv_roller_create(grub_commander_tab);
    lv_roller_set_options(roller,
                          "Ubuntu\n"
                          "Windows\n"
                          "UEFI",
                          LV_ROLLER_MODE_NORMAL
    );

    lv_roller_set_visible_row_count(roller, 3);
    lv_roller_set_selected(roller, 1, LV_ANIM_OFF);

    lv_obj_set_size(roller, LV_PCT(100), LV_PCT(100));

    lv_obj_add_event_cb(roller, grub_commander_roller_event_cb, LV_EVENT_ALL, NULL);

    return 0;
}