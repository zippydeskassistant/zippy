#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include <lvgl.h>

#include "ui/ui.h"

extern lv_obj_t *tabview;

LOG_MODULE_REGISTER(tab_skeleton);

ZBUS_CHAN_DECLARE(zbus_chan_tab_skeleton);

void update_tab_skeleton(struct k_work *work) {}

K_WORK_DEFINE(work_tab_skeleton, update_tab_skeleton);

void zbus_cb_tab_skeleton(const struct zbus_channel *chan) {}

ZBUS_LISTENER_DEFINE(skeleton_listener, zbus_cb_tab_skeleton);

static int skeleton_tab_init(void) {
    LOG_INF("Initializing Skeleton tab");
    lv_obj_t *skeleton_tab = lv_tabview_add_tab(tabview, LV_SYMBOL_LIST);
    lv_obj_set_size(skeleton_tab, LV_PCT(100), LV_PCT(90));

    lv_obj_t *content;

    lv_obj_set_size(content, LV_PCT(100), LV_PCT(100));

    return 0;
}

SYS_INIT(skeleton_tab_init, APPLICATION, CONFIG_ZIPPY_UI_TAB_INIT_PRIORITY);