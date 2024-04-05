/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <lvgl_input_device.h>
#include "zippy_theme.h"
#include "ui.h"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

lv_obj_t *tabview;

static lv_obj_t *create_flex_container(lv_obj_t * parent, int per_x, int per_y, lv_flex_flow_t flow_direction, bool scrollable) {
    lv_obj_t * flex_container = lv_obj_create(parent);

    if (flex_container == NULL) {
        return NULL;
    }

    lv_obj_set_size(flex_container, LV_PCT(per_x), LV_PCT(per_y));
    lv_obj_set_flex_flow(flex_container, flow_direction);

    if (scrollable) {
        lv_obj_add_flag(flex_container, LV_OBJ_FLAG_SCROLLABLE);
    }
    else {
        lv_obj_clear_flag(flex_container, LV_OBJ_FLAG_SCROLLABLE);
    }

    return flex_container;
}

static lv_obj_t * create_topbar(lv_obj_t * parent, const char * title) {
    lv_obj_t * topbar_container = create_flex_container(parent, 95, 10, LV_FLEX_FLOW_ROW, false);

    lv_obj_t * title_label = lv_label_create(topbar_container);
    lv_label_set_text(title_label, title);
    lv_obj_set_flex_grow(title_label, 2);

    lv_obj_t * spacer = lv_obj_create(topbar_container);
    lv_obj_set_flex_grow(spacer, 4);

    lv_obj_t * time_label = lv_label_create(topbar_container);
    lv_label_set_text(time_label, "HH:MM xm");
    lv_obj_set_flex_grow(time_label, 4);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_add_state(topbar_container, LV_STATE_DISABLED);

    lv_obj_set_flex_align(topbar_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    return topbar_container;    
}

static lv_obj_t * create_tabview(lv_obj_t * parent) {
    lv_obj_t *tv = lv_tabview_create(parent, LV_DIR_TOP, 20);
    return tv;
}

int ui_init(void) {
    lv_disp_t *disp = lv_disp_get_default();

    lv_theme_t *zippy_theme;
    zippy_theme = lv_theme_zippy_init(lv_disp_get_default());
    
    lv_disp_set_theme(disp, zippy_theme);

	const struct device *display_dev;

    lv_obj_t *flex_container = create_flex_container(lv_scr_act(), 100, 100, LV_FLEX_FLOW_COLUMN, false);
    lv_obj_set_flex_align(flex_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *topbar = create_topbar(flex_container, "GRUB");

    tabview = create_tabview(flex_container);
    lv_obj_align_to(tabview, topbar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return -EIO;
	}

	lv_task_handler();
	display_blanking_off(display_dev);

    return 0;
}

SYS_INIT(ui_init, APPLICATION, CONFIG_ZIPPY_UI_INIT_PRIORITY);

int ui_thread(void)
{
	while (1) {
		lv_task_handler();
		k_msleep(10);
	}
}

K_THREAD_DEFINE(ui_thread_id, 4096, ui_thread, NULL, NULL, NULL, 5, 0, 0);