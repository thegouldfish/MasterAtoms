#include "PadHelper.h"
#include <SMSlib.h>

Pad Pads[2];

static u8 UpdateInput(u16 newState, u16 prev, u16 toCheck)
{
	if ((newState & toCheck) && (prev & toCheck))
		return PAD_HELD;
	else if (!(prev & toCheck) && (newState & toCheck))
		return PAD_PRESSED;
	else if ((prev & toCheck) && !(newState & toCheck))
		return PAD_RELEASED;
	else
		return PAD_UP;
}


void SetupPads()
{
	for (int i = 0; i < 2; i++)
	{
		Pads[i].PrevState = 0;
		Pads[i].PrevState = 0;
	}

	UpdatePads();
}

void ResetPads()
{
	for (int i = 0; i < 2; i++)
	{
		Pads[i].PrevState = 0;
		Pads[i].PrevState = 0;
	}
}

void UpdatePads()
{
	u16 value = SMS_getKeysStatus();

	Pads[0].PrevState = Pads[0].CurrentState;
	Pads[0].CurrentState = value;


	Pads[0].A = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState, PORT_A_KEY_1);
	Pads[0].B = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState, PORT_A_KEY_2);
	Pads[0].Up = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState, PORT_A_KEY_UP);
	Pads[0].Down = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState, PORT_A_KEY_DOWN);
	Pads[0].Left = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState, PORT_A_KEY_LEFT);
	Pads[0].Right = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState, PORT_A_KEY_RIGHT);


	Pads[1].PrevState = Pads[0].CurrentState;
	Pads[1].CurrentState = value;
	Pads[1].A = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState,		PORT_B_KEY_1);
	Pads[1].B = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState,		PORT_B_KEY_2);
	Pads[1].Up = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState,		PORT_B_KEY_UP);
	Pads[1].Down = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState,		PORT_B_KEY_DOWN);
	Pads[1].Left = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState,		PORT_B_KEY_LEFT);
	Pads[1].Right = UpdateInput(Pads[0].CurrentState, Pads[0].PrevState,	PORT_B_KEY_RIGHT);
}
