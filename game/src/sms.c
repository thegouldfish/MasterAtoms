#include "sms.h"
#include <SMSlib.h>
#include <PSGlib.h>

static u16 randbase;
u16 RndPump;

void setRandomSeed(u16 seed)
{
    // xor it with a random value to avoid 0 value
    randbase = seed ^ 0xD94B;
}

u16 random()
{
    randbase ^= (randbase >> 5);
    randbase ^= (randbase << 9);
    randbase ^= (randbase >> 7);

    return randbase;
}

u16 random_int(u16 min, u16 max)
{
    return min + random() % (max + 1 - min);
}


void WaitForSFX()
{
    while (PSGSFXGetStatus())
    {
        PSGSFXFrame();
        PSGFrame();
        SMS_waitForVBlank();
    }
}