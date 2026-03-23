#include <SMSlib.h>
#include <PSGlib.h>
#include "../sms.h"
#include "../Audio/sfx.h"

#include "../PadHelper.h"
#include "../Game.h"
#include "../GamePlay.h"

#include "HelpState.h"

#include "ModeSelectState.h"
#include "../gfx.h"

static u8 _CurrentHelpScreen = 0;
#define MAX_HELP_SCREEN 6

static const ScreenDetails HelpScreens[MAX_HELP_SCREEN] =
{
	{
		help_1_tiles_psgcompr,
		help_1_tileMap_stmcompr,
		help_1_palette_bin,
		help_1_tileMap_stmcompr_bank
	},
	{
		help_1_1_tiles_psgcompr,
		help_1_1_tileMap_stmcompr,
		help_1_1_palette_bin,
		help_1_1_tileMap_stmcompr_bank
	},
	{
		help_2_tiles_psgcompr,
		help_2_tileMap_stmcompr,
		help_2_palette_bin,
		help_2_tileMap_stmcompr_bank
	},
	{
		help_3_tiles_psgcompr,
		help_3_tileMap_stmcompr,
		help_3_palette_bin,
		help_3_tileMap_stmcompr_bank
	},
	{
		help_3_1_tiles_psgcompr,
		help_3_1_tileMap_stmcompr,
		help_3_1_palette_bin,
		help_3_1_tileMap_stmcompr_bank
	},
	{
		help_4_tiles_psgcompr,
		help_4_tileMap_stmcompr,
		help_4_palette_bin,
		help_4_tileMap_stmcompr_bank
	}
};

static void ShowHelpScreen()
{
	SMS_zeroBGPalette();
	ScreenDetails* screen = &HelpScreens[_CurrentHelpScreen];

	SMS_mapROMBank(screen->Bank);
	SMS_loadPSGaidencompressedTiles(screen->Tiles, 0);
	SMS_loadSTMcompressedTileMap(0, 0, screen->Map);
	SMS_loadBGPalette(screen->Palette);
}


static void Enter()
{
	_CurrentHelpScreen = 0;
	ShowHelpScreen();
}




static void Update()
{
	if (Pads[0].B == PAD_RELEASED)
	{
		PSGSFXPlay(laserShoot0_psg, 3);
		StateMachineChange(&GameMachineState, &ModeSelectState);
	}

	if (Pads[0].Left == PAD_RELEASED && _CurrentHelpScreen != 0)
	{
		_CurrentHelpScreen--;
		ShowHelpScreen();
		PSGSFXPlay(blip_psg, 0);
	}
	else if (Pads[0].Right == PAD_RELEASED && _CurrentHelpScreen != (MAX_HELP_SCREEN-1))
	{
		_CurrentHelpScreen++;
		ShowHelpScreen();
		PSGSFXPlay(blip_psg, 0);
	}
}

static void End()
{
	SMS_zeroBGPalette();
	
	// Can't return sprites just "clear" all of them.
	SMS_initSprites();
	SMS_zeroSpritePalette();

	WaitForSFX();
}

SimpleState HelpState =
{
	Enter,
	Update,
	End
};

