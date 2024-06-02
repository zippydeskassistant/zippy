#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include "lvgl.h"

#include "ui/topbar/topbar_song.h"
#include "ui/ui.h"

LOG_MODULE_REGISTER(zippy_topbar_song);

extern struct k_mutex ui_mutex;

#define ZIPPY_SONG_MAX_LENGTH 32


int topbar_song_set(struct raw_hid_packet *packet) {

    // Get the length of the song name as a string
    uint8_t song_name_length = packet->data_len;
    
    // Create a buffer to hold the song name
    char song_name[ZIPPY_SONG_MAX_LENGTH];
    char *song_name_old;

    k_mutex_lock(&ui_mutex, K_FOREVER);
    song_name_old = lv_label_get_text(song_label);
    k_mutex_unlock(&ui_mutex);

    // Copy the song name from the packet to the buffer
    memcpy(song_name, packet->data, song_name_length);

    if (strncmp(song_name, song_name_old, ZIPPY_SONG_MAX_LENGTH) == 0) {
        return 0;
    }

    k_mutex_lock(&ui_mutex, K_FOREVER);
    lv_label_set_text_fmt(song_label, "%s", song_name);
    k_mutex_unlock(&ui_mutex);

    return 0;
}