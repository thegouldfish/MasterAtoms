#include <SMSlib.h>
#include <PSGlib.h>
#include "../sms.h"

#include "PlayerSelectState.h"
#include "GamePlayClassicState.h"
#include "../PadHelper.h"
#include "../Game.h"
#include "../GamePlay.h"

#include "WinnerState.h"
#include "ModeSelectState.h"

#include "../gfx.h"
#include "../Audio/sfx.h"


static ScreenDetails WinnerSetup[12] =
{
	{
		Winner_1_1_tiles_psgcompr,
		Winner_1_1_tileMap_stmcompr,
		Winner_1_1_palette_bin,
		Winner_1_1_tileMap_stmcompr_bank
	},
	{
		Winner_2_1_tiles_psgcompr,
		Winner_2_1_tileMap_stmcompr,
		Winner_2_1_palette_bin,
		Winner_2_1_tileMap_stmcompr_bank
	},
	{
		Winner_3_1_tiles_psgcompr,
		Winner_3_1_tileMap_stmcompr,
		Winner_3_1_palette_bin,
		Winner_3_1_tileMap_stmcompr_bank
	},
	{
		Winner_4_1_tiles_psgcompr,
		Winner_4_1_tileMap_stmcompr,
		Winner_4_1_palette_bin,
		Winner_4_1_tileMap_stmcompr_bank
	},
	{
		Winner_5_1_tiles_psgcompr,
		Winner_5_1_tileMap_stmcompr,
		Winner_5_1_palette_bin,
		Winner_5_1_tileMap_stmcompr_bank
	},
	{
		Winner_6_1_tiles_psgcompr,
		Winner_6_1_tileMap_stmcompr,
		Winner_6_1_palette_bin,
		Winner_6_1_tileMap_stmcompr_bank
	},
	{
		Winner_1_2_tiles_psgcompr,
		Winner_1_2_tileMap_stmcompr,
		Winner_1_2_palette_bin,
		Winner_1_2_tileMap_stmcompr_bank
	},			 
	{		 
		Winner_2_2_tiles_psgcompr,
		Winner_2_2_tileMap_stmcompr,
		Winner_2_2_palette_bin,
		Winner_2_2_tileMap_stmcompr_bank
	},			 
	{			 
		Winner_3_2_tiles_psgcompr,
		Winner_3_2_tileMap_stmcompr,
		Winner_3_2_palette_bin,
		Winner_3_2_tileMap_stmcompr_bank
	},
	{
		Winner_4_2_tiles_psgcompr,
		Winner_4_2_tileMap_stmcompr,
		Winner_4_2_palette_bin,
		Winner_4_2_tileMap_stmcompr_bank
	},
	{
		Winner_5_2_tiles_psgcompr,
		Winner_5_2_tileMap_stmcompr,
		Winner_5_2_palette_bin,
		Winner_5_2_tileMap_stmcompr_bank
	},
	{
		Winner_6_2_tiles_psgcompr,
		Winner_6_2_tileMap_stmcompr,
		Winner_6_2_palette_bin,
		Winner_6_2_tileMap_stmcompr_bank
	},
};



static void Enter()
{
	u8 index = (WinningPlayer - 1);

	if (PlayerSetup[WinningPlayer] == 2)
	{
		index += 6;
	}

	ScreenDetails* winner = &WinnerSetup[index];

	SMS_mapROMBank(winner->Bank);
	SMS_loadPSGaidencompressedTiles(winner->Tiles, 0);
	SMS_loadSTMcompressedTileMap(0, 0, winner->Map);
	SMS_loadBGPalette(winner->Palette);
}


static void Update()
{
	if (Pads[0].A == PAD_RELEASED)
	{
		PSGSFXPlay(laserShoot0_psg, 3);
		StateMachineChange(&GameMachineState, &ModeSelectState);		
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

SimpleState WinnerState =
{
	Enter,
	Update,
	End
};

