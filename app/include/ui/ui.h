#ifndef ZIPPY_UI_H__
#define ZIPPY_UI_H__

#include <lvgl.h>

/* Tab View, accessed by all tabs */
extern lv_obj_t *tabview;

extern lv_obj_t *time_label;

extern lv_obj_t *song_label;

lv_obj_t *create_flex_container(lv_obj_t * parent, int per_x, int per_y, lv_flex_flow_t flow_direction, bool scrollable);

#endif