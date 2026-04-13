#include "fixed.h"

inline fixed Fixed_div(fixed val1, fixed val2)
{
    fixed v1 = val1 << (FIXED_FRAC_BITS / 2);
    fixed v2 = val2 >> (FIXED_FRAC_BITS / 2);

    return v1 / v2;
}


inline fixed Fixed_mul(fixed val1, fixed val2)
{
    fixed v1 = val1 >> (FIXED_FRAC_BITS / 2);
    fixed v2 = val2 >> (FIXED_FRAC_BITS / 2);

    return v1 * v2;
}

inline fixed Fixed_toInt(fixed value)
{
    return value >> FIXED_FRAC_BITS;
}

inline fixed Fixed_fromInt(fixed value)
{
    return value << FIXED_FRAC_BITS;
}

inline fixed Fixed_frac(fixed value)
{
    return value & FIXED_FRAC_MASK;
}