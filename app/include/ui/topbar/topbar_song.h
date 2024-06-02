#ifndef TOPBAR_SONG_H__
#define TOPBAR_SONG_H__

#include "usb/raw_hid.h"

int topbar_song_set(struct raw_hid_packet *packet);

#endif