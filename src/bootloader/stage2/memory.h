#pragma once

#include <stdint.h>

#define SEG(seg_off) (seg_off>>16)
#define OFF(seg_off) (seg_off&0xFFFF)
#define SEG_OFF_2_LIN(seg_off) ((SEG(seg_off) << 4) + OFF(seg_off))