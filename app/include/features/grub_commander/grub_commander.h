#ifndef GRUB_COMMANDER_H__
#define GRUB_COMMANDER_H__

#include <stdint.h>

#include "zippy/zippy_msg.h"

struct gc_message {
    enum msg_type type;
    uint16_t index;
};

int grub_get_boot_opt(uint16_t *index);

#endif