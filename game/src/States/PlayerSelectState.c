#include <SMSlib.h>
#include <PSGlib.h>
#include "../sms.h"
#include "../Game.h"

#include "PlayerSelectState.h"

#include "../PadHelper.h"
#include "../bank2.h"
#include "../gfxTileCount.h"
#include "GamePlayClassicState.h"

#include "../Audio/sfx.h"


static char _SpriteCursor[4][3];

static s8 _currentSelection;
static u8 _PlayerType[] = { 1,2,0,0,0,0 };

static const Point _Positions[] = { {16, 8}, {104,8}, {192,8}, {16,96}, {104,96}, {192,96} };



static void MoveCursor(u8 sound)
{
	u8 x = _Positions[_currentSelection].X;
	u8 y = _Positions[_currentSelection].Y;

	SMS_updateSpritePosition(_SpriteCursor[0][0], x + 0 + 0, y + 0 + 0);
	SMS_updateSpritePosition(_SpriteCursor[0][1], x + 8 + 0, y + 0 + 0);
	SMS_updateSpritePosition(_SpriteCursor[0][2], x + 0 + 0, y + 8 + 0);

	SMS_updateSpritePosition(_SpriteCursor[1][0], x + 16 + 16, y + 0 + 0);
	SMS_updateSpritePosition(_SpriteCursor[1][1], x + 24 + 16, y + 0 + 0);
	SMS_updateSpritePosition(_SpriteCursor[1][2], x + 24 + 16, y + 8 + 0);

	SMS_updateSpritePosition(_SpriteCursor[2][0], x + 0 + 0, y + 16 + 16);
	SMS_updateSpritePosition(_SpriteCursor[2][1], x + 0 + 0, y + 24 + 16);
	SMS_updateSpritePosition(_SpriteCursor[2][2], x + 8 + 0, y + 24 + 16);

	SMS_updateSpritePosition(_SpriteCursor[3][0], x + 24 + 16, y + 16 + 16);
	SMS_updateSpritePosition(_SpriteCursor[3][1], x + 16 + 16, y + 24 + 16);
	SMS_updateSpritePosition(_SpriteCursor[3][2], x + 24 + 16, y + 24 + 16);

	if (sound)
	{
		PSGSFXPlay(blip_psg, 3);
	}
}


static void UpdatePlayer(u8 player)
{	
	u8 x = _Positions[player].X / 8;
	u8 y = _Positions[player].Y / 8;
	u16  buffer[36];


	if (_PlayerType[player] == 0)
	{
		for (int i = 0; i < 36; i++)
		{
			buffer[i] = 0;
		}
	}
	else
	{
		int offset = player;
		if (_PlayerType[player] == 2)
		{
			offset += 6;
		}

		offset *= 72;

		const u16* data = MenuProfessors_tileMap_bin + offset;

		for (int i = 0; i < 36; i++)
		{
			buffer[i] = *data + PlayerSelectBacking_TILECOUNT;
			data++;
		}

	}

	SMS_loadTileMapArea(x, y, buffer, 6, 6);
}

static void Enter()
{
	SMS_mapROMBank(2);
	SMS_loadPSGaidencompressedTiles(PlayerSelectBacking_tiles_psgcompr, 0);
	SMS_loadTileMap(0, 0, PlayerSelectBacking_tileMap_bin, PlayerSelectBacking_tileMap_bin_size);
	

	SMS_loadPSGaidencompressedTiles(MenuProfessors_tiles_psgcompr, PlayerSelectBacking_TILECOUNT);

	SMS_loadPSGaidencompressedTiles(MenuCursor2_tiles_psgcompr, PlayerSelectBacking_TILECOUNT + MenuProfessors_TILECOUNT);
	
	SMS_useFirstHalfTilesforSprites(FALSE);
	u8 SpritePlace = (u8)(PlayerSelectBacking_TILECOUNT + MenuProfessors_TILECOUNT);

	_SpriteCursor[0][0] = SMS_addSprite(0, 0, SpritePlace);
	_SpriteCursor[0][1] = SMS_addSprite(8, 0, SpritePlace + 1);
	_SpriteCursor[0][2] = SMS_addSprite(0, 8, SpritePlace + 2);

	_SpriteCursor[1][0] = SMS_addSprite(16 + 16, 0, SpritePlace + 3);
	_SpriteCursor[1][1] = SMS_addSprite(24 + 16, 0, SpritePlace + 4);
	_SpriteCursor[1][2] = SMS_addSprite(24 + 16, 8, SpritePlace + 5);

	_SpriteCursor[2][0] = SMS_addSprite(0, 16 + 16, SpritePlace + 6);
	_SpriteCursor[2][1] = SMS_addSprite(0, 24 + 16, SpritePlace + 7);
	_SpriteCursor[2][2] = SMS_addSprite(8, 24 + 16, SpritePlace + 8);

	_SpriteCursor[3][0] = SMS_addSprite(24 + 16, 16 + 16, SpritePlace + 9);
	_SpriteCursor[3][1] = SMS_addSprite(16 + 16, 24 + 16, SpritePlace + 10);
	_SpriteCursor[3][2] = SMS_addSprite(24 + 16, 24 + 16, SpritePlace + 11);

	_currentSelection = 0;
	MoveCursor(FALSE);
	
	for (u8 i = 0; i < 6; i++)
	{
		UpdatePlayer(i);
	}
	
	SMS_loadBGPalette(PlayerSelectBacking_palette_bin);
	SMS_loadSpritePalette(MenuCursor2_palette_bin);
	SMS_setSpritePaletteColor(0, 0);
}




static void Update()
{

	if (Pads[0].Left == PAD_RELEASED)
	{
		_currentSelection--;
		if (_currentSelection < 0)
		{
			_currentSelection = 5;
		}

		MoveCursor(TRUE);
	}
	else if (Pads[0].Right == PAD_RELEASED)
	{
		_currentSelection++;
		if (_currentSelection > 5)
		{
			_currentSelection = 0;
		}

		MoveCursor(TRUE);
	}
	else if (Pads[0].Up == PAD_RELEASED)
	{
		_currentSelection -= 3;
		if (_currentSelection < 0)
		{
			_currentSelection += 6;
		}

		MoveCursor(TRUE);
	}
	else if (Pads[0].Down == PAD_RELEASED)
	{
		_currentSelection += 3;
		if (_currentSelection > 5)
		{
			_currentSelection -= 6;
		}

		MoveCursor(TRUE);
	}

	if (Pads[0].B == PAD_RELEASED)
	{
		_PlayerType[_currentSelection]++;

		if (_PlayerType[_currentSelection] > 2)
		{
			_PlayerType[_currentSelection] = 0;
		}

		PSGSFXPlay(hitHurt0_psg, 3);
		UpdatePlayer(_currentSelection);
	}

	if (Pads[0].A == PAD_RELEASED)
	{
		u8 willPlay = 0;
		for (u8 i = 0; i < 6; i++)
		{
			if (_PlayerType[i])
			{
				willPlay++;
			}
		}

		if (willPlay >= 2)
		{
			PlayerSetup[0] = 0;

			for (u8 i = 1; i < 7; i++)
			{
				PlayerSetup[i] = _PlayerType[i - 1];
			}
			
			PSGSFXPlay(laserShoot0_psg, 3);
			StateMachineChange(&GameMachineState, &GamePlayClassicState);
		}		
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


SimpleState PlayerSelectState =
{
	Enter,
	Update,
	End
};


