#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

#include "ui/tabs/tabs.h"

#include "ui/ui.h"

extern lv_obj_t *tabview;

LOG_MODULE_REGISTER(tab_audio_controller);

ZBUS_CHAN_DECLARE(zbus_chan_tab_audio_controller);

void update_tab_audio_controller(struct k_work *work) {}

K_WORK_DEFINE(work_tab_audio_controller, update_tab_audio_controller);

void zbus_cb_tab_audio_controller(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(audio_controller_listener, zbus_cb_tab_audio_controller);

static void audio_controller_roller_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        char selected_option[16];
        lv_roller_get_selected_str(roller, selected_option, sizeof(selected_option));
        LOG_INF("Selected option: %s", selected_option);
    }
}

int audio_controller_tab_init(void) {
    LOG_INF("Initializing Audio Controller tab");
    lv_obj_t *audio_controller_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_size(audio_controller_tab, LV_PCT(100), LV_PCT(90));

    lv_obj_t *roller = lv_roller_create(audio_controller_tab);
    lv_roller_set_options(roller,
                          "Foo\n"
                          "Bar\n"
                          "Baz",
                          LV_ROLLER_MODE_NORMAL
    );

    lv_roller_set_visible_row_count(roller, 3);
    lv_roller_set_selected(roller, 1, LV_ANIM_OFF);

    lv_obj_set_size(roller, LV_PCT(100), LV_PCT(100));

    lv_obj_add_event_cb(roller, audio_controller_roller_event_cb, LV_EVENT_ALL, NULL);

    return 0;
}