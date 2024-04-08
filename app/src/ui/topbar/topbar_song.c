#include <zephyr/logging/log.h>
#include "lvgl.h"

#include "ui/ui.h"
#include "ui/topbar/topbar_time.h"

int song_set(struct raw_hid_packet *packet) {

    // Get the length of the song name as a string
    uint8_t song_name_length = packet->data_len;
    
    // Create a buffer to hold the song name
    char song_name[32];

    // Copy the song name from the packet to the buffer
    memcpy(song_name, packet->data, song_name_length);

    lv_label_set_text_fmt(song_label, "%s", song_name);
    return 0;
}