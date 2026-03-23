#include "fixed.h"

inline fix32 F32_div(fix32 val1, fix32 val2)
{
    fix32 v1 = val1 << (FIX32_FRAC_BITS / 2);
    fix32 v2 = val2 >> (FIX32_FRAC_BITS / 2);

    return v1 / v2;
}


inline fix32 F32_mul(fix32 val1, fix32 val2)
{
    fix32 v1 = val1 >> (FIX32_FRAC_BITS / 2);
    fix32 v2 = val2 >> (FIX32_FRAC_BITS / 2);

    return v1 * v2;
}

inline s32 F32_toInt(fix32 value)
{
    return value >> FIX32_FRAC_BITS;
}

inline fix32 F32_fromInt(s32 value)
{
    return value << FIX32_FRAC_BITS;
}

inline fix32 F32_frac(fix32 value)
{
    return value & FIX32_FRAC_MASK;
}