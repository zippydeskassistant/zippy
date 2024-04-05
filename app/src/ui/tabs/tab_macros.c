#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

#include "ui/tabs/tabs.h"

#include "ui/ui.h"

extern lv_obj_t *tabview;

LOG_MODULE_REGISTER(tab_macros);

ZBUS_CHAN_DECLARE(zbus_chan_tab_macros);

void update_tab_macros(struct k_work *work) {}

K_WORK_DEFINE(work_tab_macros, update_tab_macros);

void zbus_cb_tab_macros(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(macros_listener, zbus_cb_tab_macros);

int macros_tab_init(void) {
    LOG_INF("Initializing macros tab");
    lv_obj_t *macros_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_KEYBOARD);
    lv_obj_set_size(macros_tab, LV_PCT(100), LV_PCT(90));

    lv_obj_t *content = lv_label_create(macros_tab);
    lv_label_set_text(content, "Macros tab content");

    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100));

    return 0;
}