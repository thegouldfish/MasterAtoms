#include <SMSlib.h>
#include "TitleScreenState.h"
#include "../PadHelper.h"
#include "../Game.h"

#include "PlayerSelectState.h"
#include "ModeSelectState.h"

#include "../gfx.h"

#include "../Audio/music.h"
#include <PSGlib.h>


static u8 _currentPal = 11;
static u8 _frame = 0;

static void SetPal()
{
	u8 prevPal = _currentPal;
	u8 prevShadow = _currentPal - 1;

	if (prevShadow < 11)
	{
		prevShadow = 15;
	}

	_currentPal++;
	if (_currentPal > 15)
	{
		_currentPal = 11;
	}

	SMS_setBGPaletteColor(prevShadow, Tile_Animated_palette_bin[1]);
	SMS_setBGPaletteColor(prevPal, Tile_Animated_palette_bin[Tile_Animated_palette_bin_size-1]);
	SMS_setBGPaletteColor(_currentPal, Tile_Animated_palette_bin[2]);
}


static void Enter()
{
	SMS_mapROMBank(2);
	SMS_loadPSGaidencompressedTiles(Tile_Animated_tiles_psgcompr, 0);
	SMS_loadSTMcompressedTileMap(0, 0, Tile_Animated_tileMap_stmcompr);
	SMS_loadBGPalette(Tile_Animated_palette_bin);
	
	for (int i = 11; i < 16; i++)
	{
		SMS_setBGPaletteColor(i, Tile_Animated_palette_bin[1]);
	}
	
	_currentPal = 11;
	SetPal();
	SMS_setSpritePaletteColor(0, 0);
	//PSGPlay(atoms_psg);	
}


static void Update()
{
	if (Pads[0].A == PAD_RELEASED)
	{
		//StateMachineChange(&GameMachineState, &PlayerSelectState);
		StateMachineChange(&GameMachineState, &ModeSelectState);
	}

	_frame++;

	if (_frame == 8)
	{
		SetPal();
		_frame = 0;
	}
}


static void End()
{
	SMS_zeroBGPalette();
}


SimpleState TitleScreenState =
{
	Enter,
	Update,
	End
};


