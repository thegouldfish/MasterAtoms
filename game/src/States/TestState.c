#include <SMSlib.h>
#include "../sms.h"

#include "TestState.h"
#include "../PadHelper.h"
#include "../Game.h"

#include "../gfx.h"

typedef struct
{
	const unsigned char* Tiles;
	const unsigned char* Map;
	const unsigned char* Palette;
	const u8 Bank;
} WinnerDetails;


static WinnerDetails WinnerSetup[12] =
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


static u8 winningPlayer = 1;
static u8 type = 1;

static void Enter()
{
	u8 index = (winningPlayer - 1);

	if (type == 2)
	{
		index += 6;
	}

	type++;
	if (type > 2)
	{
		winningPlayer++;
		type = 1;

		if (winningPlayer > 6)
		{
			winningPlayer = 1;
			type = 1;
		}
	}

	WinnerDetails* winner = &WinnerSetup[index];

	SMS_mapROMBank(winner->Bank);
	SMS_loadPSGaidencompressedTiles(winner->Tiles, 0);
	SMS_loadSTMcompressedTileMap(0, 0, winner->Map);
	SMS_loadBGPalette(winner->Palette);
}


static void Update()
{
	if (Pads[0].A == PAD_RELEASED)
	{
		StateMachineChange(&GameMachineState, &TestState);
	}
}

static void End()
{
	SMS_zeroBGPalette();
	
	// Can't return sprites just "clear" all of them.
	SMS_initSprites();
	SMS_zeroSpritePalette();
}

SimpleState TestState =
{
	Enter,
	Update,
	End
};

