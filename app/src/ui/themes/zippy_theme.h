#ifndef ZIPPY_THEME_H__
#define ZIPPY_THEME_H__

#include <lvgl.h>

#if CONFIG_ZIPPY_THEME_DEFAULT
#include "zippy_theme_default.h"
lv_theme_t *(*lv_theme_zippy_init)(lv_disp_t * disp) = lv_theme_zippy_default_init;
#endif

#endif