#ifndef _FIXED_H_
#define _FIXED_H_
#include "sms.h"

// Based on the SGDK Fixed number library
typedef s16 fixed;

#define FIXED_INT_BITS              8
#define FIXED_FRAC_BITS             (16 - FIXED_INT_BITS)
#define FIXED_INT_MASK              (((1 << FIXED_INT_BITS) - 1) << FIXED_FRAC_BITS)
#define FIXED_FRAC_MASK             ((1 << FIXED_FRAC_BITS) - 1)


#define FIXED(value)                ((fixed) ((value) * (1 << FIXED_FRAC_BITS)))


#define FIXED_FROM_INT(value)       (value << FIXED_FRAC_BITS)


fixed Fixed_div(fixed val1, fixed val2);
fixed Fixed_mul(fixed val1, fixed val2);
fixed Fixed_toInt(fixed value);
fixed Fixed_fromInt(fixed value);
fixed Fixed_frac(fixed value);

#endif