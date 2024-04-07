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

static void volume_slider_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);

    uint32_t value = lv_slider_get_value(slider);
    LOG_INF("Main Volume Changed: %d", value); 
}

static lv_obj_t * create_slider_with_label(lv_obj_t * parent, char * label_text) {
    lv_obj_t *slider_container = create_flex_container(parent, 100, 20, LV_FLEX_FLOW_COLUMN, false);

    lv_obj_t *label = lv_label_create(slider_container);
    lv_obj_set_size(label, LV_PCT(100), LV_PCT(50));
    lv_label_set_text(label, label_text);

    lv_obj_t *slider = lv_slider_create(slider_container);
    lv_obj_set_size(slider, LV_PCT(100), LV_PCT(50));
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);

    lv_obj_add_event_cb(slider, volume_slider_cb, LV_EVENT_RELEASED, label_text);

    return slider_container;
}

int audio_controller_tab_init(void) {
    LOG_INF("Initializing Audio Controller tab");
    lv_obj_t *audio_controller_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_size(audio_controller_tab, LV_PCT(100), LV_PCT(90));

    lv_obj_t *audio_controller_flex = create_flex_container(audio_controller_tab, 100, 100, LV_FLEX_FLOW_COLUMN, false);

    char *main_slider_title = "System";
    lv_obj_t *main_volume_slider = create_slider_with_label(audio_controller_flex, main_slider_title);

    return 0;
}