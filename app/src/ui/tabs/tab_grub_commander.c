#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

#include "ui.h"

extern lv_obj_t *tabview;

LOG_MODULE_REGISTER(tab_grub_commander);

ZBUS_CHAN_DECLARE(zbus_chan_tab_grub_commander);

void update_tab_grub_commander(struct k_work *work) {}

K_WORK_DEFINE(work_tab_grub_commander, update_tab_grub_commander);

void zbus_cb_tab_grub_commander(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(grub_commander_listener, zbus_cb_tab_grub_commander);

static void grub_commander_roller_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        char selected_option[16];
        lv_roller_get_selected_str(roller, selected_option, sizeof(selected_option));
        LOG_INF("Selected option: %s", selected_option);
    }
}

static int grub_commander_tab_init(void) {
    LOG_INF("Initializing GRUB Commander tab");
    lv_obj_t *grub_commander_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_LIST);

    lv_obj_t *roller = lv_roller_create(grub_commander_tab);
    lv_roller_set_options(roller,
                          "Windows\n"
                          "Ubuntu\n"
                          "UEFI\n",
                          LV_ROLLER_MODE_NORMAL
    );

    lv_roller_set_visible_row_count(roller, 3);
    lv_obj_set_size(roller, LV_PCT(100), LV_PCT(100));
    lv_obj_center(roller);

    lv_obj_add_event_cb(roller, grub_commander_roller_event_cb, LV_EVENT_ALL, NULL);

    return 0;
}

SYS_INIT(grub_commander_tab_init, APPLICATION, CONFIG_ZIPPY_UI_TAB_INIT_PRIORITY);