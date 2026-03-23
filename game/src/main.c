#include <SMSlib.h>
#include <PSGlib.h>
#include "PadHelper.h"
#include "SimpleState.h"
#include "Game.h"
#include "sms.h"

#include "States/TitleScreenState.h"


#include "States/ChallengeModeState.h"
#include "States/GameOverState.h"


StateMachine GameMachineState;

void main (void)
{	
	SetupPads();
	RndPump = 0;
	StateMachineStart(&GameMachineState, &TitleScreenState);

	//Score = 12345;
	//StateMachineStart(&GameMachineState, &GameOverState);

	SMS_initSprites();

	SMS_displayOn();
	for (;;)
	{
		RndPump++;
		StateMachineUpdate(&GameMachineState);

		SMS_waitForVBlank();		
		PSGFrame();
		PSGSFXFrame();
		UpdatePads();
		

		SMS_copySpritestoSAT();
	}
}


SMS_EMBED_SEGA_ROM_HEADER(9999, 0);
SMS_EMBED_SDSC_HEADER(1, 0, 2026, 03, 20, "GouldFish", "MasterAtoms", "Atoms For MasterSystem");