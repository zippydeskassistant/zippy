#ifndef TOPBAR_TIME_H__
#define TOPBAR_TIME_H__

#include "usb/raw_hid.h"

int time_set(struct raw_hid_packet *packet);

#endif