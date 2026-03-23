#include <SMSlib.h>
#include <PSGlib.h>

#include "../PadHelper.h"
#include "../Game.h"

#include "GameOverState.h"
#include "ChallengeModeState.h"
#include "ModeSelectState.h"

#include "../gfx.h"
#include "../Audio/sfx.h"

#define LARGENUMBERS_TILE_INDEX GameOverBacking_TILECOUNT

static void DrawXDigitNumber(u32 number, u8 digit, u8 x, u8 y)
{
	s16 numbs[6] = { 0,0,0,0,0,0 };

	if (number > 999999)
	{
		for (int i = 0; i < 6; i++)
		{
			numbs[i] = 9;
		}
	}

	if (digit > 6)
	{
		digit = 6;
	}

	int i = digit - 1;
	while (number > 0)
	{
		numbs[i] = (number % 10);
		number /= 10;
		i--;
	}

	u16 buffer[4];
	int index = 0;
	for (int i = 0; i < digit; i++)
	{
		buffer[0] = LARGENUMBERS_TILE_INDEX + (numbs[i] * 2);
		
		buffer[2] = LARGENUMBERS_TILE_INDEX + 20 + (numbs[i] * 2);
	
		buffer[1] = LARGENUMBERS_TILE_INDEX + 1 + (numbs[i] * 2);
		buffer[3] = LARGENUMBERS_TILE_INDEX + 21 + (numbs[i]* 2);
	
		SMS_loadTileMapArea(x, y, buffer, 2, 2);
		x += 2;
	}
}

static void Enter()
{
	SMS_mapROMBank(GameOverBacking_palette_bin_bank);
	SMS_loadPSGaidencompressedTiles(GameOverBacking_tiles_psgcompr, 0);
	SMS_loadSTMcompressedTileMap(0, 0, GameOverBacking_tileMap_stmcompr);
	
	SMS_loadTiles(LargeNumbers_tiles_bin, LARGENUMBERS_TILE_INDEX, LargeNumbers_tiles_bin_size);
	

	DrawXDigitNumber(Score, 6, 9, 8);

	DrawXDigitNumber(CurrentLevel + 1, 2, 13, 14);

	SMS_loadBGPalette(GameOverBacking_palette_bin);
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


SimpleState GameOverState =
{
	Enter,
	Update,
	End
};