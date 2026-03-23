#include <SMSlib.h>
#include <PSGlib.h>
#include "../sms.h"
#include "../Game.h"

#include "ModeSelectState.h"

#include "../PadHelper.h"
#include "../bank8.h"
#include "../gfxTileCount.h"
#include "../Audio/sfx.h"

#include "PlayerSelectState.h"
#include "ChallengeModeState.h"
#include "HelpState.h"



static s8 _Anim = 0;
static u8 _AnimCounter = 0;
static u8 _AnimDirection = 0;

static s8 _Selection = 0;

static char _SpriteCursor[8];

static void MoveCursor()
{
	s16 y = 0;
	s16 x = 0;


	switch (_Selection)
	{
		case 0:	
			y = 55;
			x = 24;
			break;

		case 1:
			y = 92;
			x = 0;
			break;

		case 2:
			y = 125;
			x = 50;
			break;
	}

	
	SMS_updateSpritePosition(_SpriteCursor[0], x +  0 + _Anim, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[1], x +  8 + _Anim, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[2], x + 16 + _Anim, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[3], x + 24 + _Anim, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[4], x +  0 + _Anim, y + 8);
	SMS_updateSpritePosition(_SpriteCursor[5], x +  8 + _Anim, y + 8);
	SMS_updateSpritePosition(_SpriteCursor[6], x + 16 + _Anim, y + 8);
	SMS_updateSpritePosition(_SpriteCursor[7], x + 24 + _Anim, y + 8);
}


static void Enter()
{
	SMS_mapROMBank(8);
	SMS_loadPSGaidencompressedTiles(SelectModeScreen_tiles_psgcompr, 0);
	
	SMS_loadPSGaidencompressedTiles(modeSelectPointer_tiles_psgcompr, SelectModeScreen_TILECOUNT);
	
	SMS_loadTileMap(0, 0, SelectModeScreen_tileMap_bin, SelectModeScreen_tileMap_bin_size);


	SMS_useFirstHalfTilesforSprites(FALSE);
	
	for (int i = 0; i < 8; i++)
	{
		_SpriteCursor[i] = SMS_addSprite(0, 0, SelectModeScreen_TILECOUNT+i);
	}

	SMS_loadBGPalette(SelectModeScreen_palette_bin);
	SMS_loadSpritePalette(modeSelectPointer_palette_bin);
	SMS_setSpritePaletteColor(0, 0);

	_Selection = 0;

	MoveCursor();
}




static void Update()
{
	_AnimCounter++;

	if (_AnimCounter > 3)
	{
		if (_AnimDirection == 0)
		{
			_Anim++;

			if (_Anim > 4)
			{
				_AnimDirection = 1;
				_Anim = 4;
			}

			MoveCursor();
		}
		else if (_AnimDirection == 1)
		{
			_Anim--;

			if (_Anim < 0)
			{
				_AnimDirection = 0;
				_Anim = 0;
			}

			MoveCursor();
		}

		_AnimCounter = 0;		
	}


	if (Pads[0].Up == PAD_RELEASED)
	{
		_Selection--;

		if (_Selection < 0)
		{
			_Selection = 2;
		}		
		
		PSGSFXPlay(blip_psg, 3);
	}
	else if (Pads[0].Down == PAD_RELEASED)
	{
		_Selection++;

		if (_Selection > 2)
		{
			_Selection = 0;
		}

		PSGSFXPlay(blip_psg, 3);
	} 
	else if (Pads[0].A == PAD_RELEASED)
	{
		switch (_Selection)
		{
			case 0:
				StateMachineChange(&GameMachineState, &PlayerSelectState);
			break;

			case 1:
				StateMachineChange(&GameMachineState, &ChallengeModeState);
				break;

			case 2:
				StateMachineChange(&GameMachineState, &HelpState);
				break;
		}

		PSGSFXPlay(laserShoot0_psg, 3);
	}
}


static void End()
{
	SMS_zeroBGPalette();
		
	// Can't return sprites just "clear" all of them.
	SMS_initSprites();
	SMS_zeroSpritePalette();

	WaitForSFX();
	PSGStop();
}


SimpleState ModeSelectState =
{
	Enter,
	Update,
	End
};


