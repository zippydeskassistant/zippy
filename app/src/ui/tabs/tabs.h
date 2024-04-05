#ifndef TABS_H__
#define TABS_H__

#include <lvgl.h>

#ifdef CONFIG_ZIPPY_AUDIO_CONTROLLER
int audio_controller_tab_init(void);
#endif

#ifdef CONFIG_ZIPPY_AUDIO_PLAYER
int audio_player_tab_init(void);
#endif

#ifdef CONFIG_ZIPPY_GRUB_COMMANDER
int grub_commander_tab_init(void);
#endif

#ifdef CONFIG_ZIPPY_MACROS
int macros_tab_init(void);
#endif

#endif