#ifndef _FIXED_H_
#define _FIXED_H_
#include "sms.h"

// Based on the SGDK's Fixed point math library
typedef s32 fix32;

#define FIX32_INT_BITS              22
#define FIX32_FRAC_BITS             (32 - FIX32_INT_BITS)
#define FIX32_INT_MASK              (((1 << FIX32_INT_BITS) - 1) << FIX32_FRAC_BITS)
#define FIX32_FRAC_MASK             ((1 << FIX32_FRAC_BITS) - 1)


#define FIX32(value)                ((fix32) ((value) * (1 << FIX32_FRAC_BITS)))


#define FIX32_FROM_INT(value)       (value << FIX32_FRAC_BITS)


fix32 F32_div(fix32 val1, fix32 val2);
fix32 F32_mul(fix32 val1, fix32 val2);
s32 F32_toInt(fix32 value);
fix32 F32_fromInt(s32 value);
fix32 F32_frac(s32 value);

#endif