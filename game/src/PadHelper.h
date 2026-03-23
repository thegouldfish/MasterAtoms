#ifndef PAD_HELPER_
#define PAD_HELPER_

#include "sms.h"

#define PAD_UP 0
#define PAD_RELEASED 1
#define PAD_HELD 2
#define PAD_PRESSED 3



typedef struct
{
	u8 A;
	u8 B;

	u8 Up;
	u8 Down;
	u8 Left;
	u8 Right;

	u16 CurrentState;
	u16 PrevState;
} Pad;

extern Pad Pads[2];

void SetupPads();
void UpdatePads();
void ResetPads();

#endif