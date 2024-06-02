#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

#include "features/grub_commander/grub_commander.h"

#include "ui/tabs/tabs.h"

#include "ui/ui.h"

LOG_MODULE_REGISTER(tab_grub_commander);

extern lv_obj_t *tabview;

static lv_obj_t *roller;

extern struct k_mutex ui_mutex;

ZBUS_CHAN_DECLARE(grub_commander_chan);

static void grub_commander_roller_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        struct gc_message selected_option;
        selected_option.type = MSG_TYPE_UI;
        k_mutex_lock(&ui_mutex, K_FOREVER);
        selected_option.index = lv_roller_get_selected(roller);
        k_mutex_unlock(&ui_mutex);

        zbus_chan_pub(&grub_commander_chan, &selected_option, K_NO_WAIT);

        LOG_INF("Selected option: %d", selected_option.index);
    }
}

int grub_commander_tab_init(void) {
    LOG_INF("Initializing GRUB Commander tab");
    lv_obj_t *grub_commander_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_LIST);
    lv_obj_clear_flag(grub_commander_tab, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(grub_commander_tab, LV_PCT(100), LV_PCT(90));

    roller = lv_roller_create(grub_commander_tab);
    lv_roller_set_options(roller,
                          "Ubuntu\n"
                          "Windows\n"
                          "UEFI",
                          LV_ROLLER_MODE_NORMAL
    );
    lv_roller_set_visible_row_count(roller, 3);

    uint16_t index;
    grub_get_boot_opt(&index);
    lv_roller_set_selected(roller, index, LV_ANIM_OFF);

    lv_obj_set_size(roller, LV_PCT(100), LV_PCT(100));

    lv_obj_add_event_cb(roller, grub_commander_roller_event_cb, LV_EVENT_ALL, NULL);

    return 0;
}


static void zbus_cb_grub_commander_tab(const struct zbus_channel *chan) {
    const struct gc_message *selected_option = zbus_chan_const_msg(chan);

    if (selected_option->type == MSG_TYPE_FEATURE) {
        uint16_t index = selected_option->index;
        if (roller != NULL) {
            k_mutex_lock(&ui_mutex, K_FOREVER);
            lv_roller_set_selected(roller, index, LV_ANIM_OFF);
            k_mutex_unlock(&ui_mutex);
        }
        LOG_INF("FEATURE: Selected option: %d", selected_option->index);
    }
}

ZBUS_LISTENER_DEFINE(grub_commander_tab, zbus_cb_grub_commander_tab);