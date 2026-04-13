#include "GamePlayClassicState.h"
#include <SMSlib.h>
#include <PSGlib.h>

#include "../sms.h"
#include "../GamePlay.h"
#include "../PadHelper.h"

#include "../gfx.h"

#include "WinnerState.h"
#include "../Game.h"

#include "../Audio/sfx.h"

#define STATE_PLAY 0
#define STATE_ANIMATE 1
#define STATE_ANIMATE_WAIT 2
#define STATE_END_CHECK 3
#define STATE_PAUSE 4

u8 WinningPlayer = 0;
s8 PlayerSetup[7];

static u16 _TurnCount = 0;

static s8 _CurrentPlayer = 0;
static s8 _PlayerIndex = 0;
static u8 _GameState = STATE_PLAY;

static s8 _GameFinished;
static s8 _Alive[7];
static s8 _Counts[7];

static Point _CursorPositions[7];

static u8 _AnimTimer = 0;
static u8 _AnimCounter = 0;
static u8 _Anim = 0;
static unsigned char _Palette[16];
static unsigned char _Palette2[16];

static s8 _MiniFlash = 0;

static void SetupGame()
{
	int i = 0;

	for (i = 0; i < 70; i++)
	{
		_PlayerGrid[i].Changed = 0;
		_PlayerGrid[i].GrowSize = 0;
		_PlayerGrid[i].Player = 0;
		_PlayerGrid[i].Size = 0;
		_PlayerGrid[i].Animate = 0;
		_PlayerGrid[i].ChangeAnim = 0;
	}

	// Reset the Player
	// Player 0 is no player
	_CurrentPlayer = 0;

	_GameFinished = 0;

	_TurnCount = 0;

	for (i = 0; i < 7; i++)
	{
		if (PlayerSetup[i])
		{
			_Alive[i] = 1;
		}
		else
		{
			_Alive[i] = 0;
		}

		if (_CurrentPlayer == 0 && PlayerSetup[i] != 0)
		{
			_CurrentPlayer = i;
		}

		_CursorPositions[i].X = -1;
		_CursorPositions[i].Y = -1;
	}

	ResetPads();

	_Anim = 0;
	_GameState = STATE_PLAY;
}

static u8 _ChangePalette = 0;
static void LineIRQ()
{
	if (_ChangePalette == 1 && _GameState != STATE_PAUSE)
	{
		_ChangePalette = 0;
		SMS_loadBGPalette(_Palette);
	}
}

static u8 _ProgressPlaces[6] = { 5, 9, 13, 17, 21, 25 };

#define NUMBER_TILE_INDEX Backing_TILECOUNT
#define ATOM_PROGRESS_1_INDEX Backing_TILECOUNT + Numbers_TILECOUNT
#define ATOM_PROGRESS_2_INDEX Backing_TILECOUNT + Numbers_TILECOUNT + atom_progress_1_TILECOUNT
#define ATOM_PROGRESS_3_INDEX Backing_TILECOUNT + Numbers_TILECOUNT + atom_progress_1_TILECOUNT + + atom_progress_2_TILECOUNT

static s16 _Buffer[3];
static void Draw2DigitNumber(int number, s16* buffer)
{
	if (number > 99)
	{
		buffer[0] = NUMBER_TILE_INDEX + 9;
		buffer[1] = NUMBER_TILE_INDEX + 9;
	}
	else if (number > 9)
	{
		int count = 0;

		while (number > 9)
		{
			number -= 10;
			count++;
		}

		buffer[0] = NUMBER_TILE_INDEX + count;
		buffer[1] = NUMBER_TILE_INDEX + number;
	}
	else
	{
		buffer[0] = NUMBER_TILE_INDEX;
		buffer[1] = NUMBER_TILE_INDEX + number;
	}	
}


static void UpdateProgress()
{
	for (u8 j = 0; j < 7; j++)
	{
		_Counts[j] = 0;
	}

	for (GridSquare* it = _PlayerGrid; it != _PlayerGrid + 70; it++)
	{
		if (it->Size == 5)
		{
			_Counts[it->Player] += it->MaxSize;
		}
		else
		{
			_Counts[it->Player] += it->Size;
		}

	}

	for (int i = 1; i < 7; i++)
	{
		if(PlayerSetup[i] != 0)
		{
			if (_Alive[i])
			{
				_Buffer[0] = ATOM_PROGRESS_1_INDEX + i - 1;
				Draw2DigitNumber(_Counts[i], _Buffer +1);
			}
			else
			{
				_Buffer[0] = ATOM_PROGRESS_3_INDEX + i - 1;
				Draw2DigitNumber(_Counts[i], _Buffer + 1);
			}

			SMS_loadTileMap(_ProgressPlaces[i - 1], 1, _Buffer, 6);
		}		
	}
}


static void Enter()
{
	TileStartLocation = Backing_TILECOUNT;
	SMS_mapROMBank(3);

	SMS_loadPSGaidencompressedTiles(Backing_tiles_psgcompr, 0);
	SMS_loadSTMcompressedTileMap(0, 0, Backing_tileMap_stmcompr);

	SMS_loadTiles(Numbers_tiles_bin, NUMBER_TILE_INDEX, Numbers_tiles_bin_size);
	SMS_loadTiles(atom_progress_1_tiles_bin, ATOM_PROGRESS_1_INDEX, atom_progress_1_tiles_bin_size);
	SMS_loadTiles(atom_progress_2_tiles_bin, ATOM_PROGRESS_2_INDEX, atom_progress_1_tiles_bin_size);
	SMS_loadTiles(atom_progress_3_tiles_bin, ATOM_PROGRESS_3_INDEX, atom_progress_1_tiles_bin_size);




	for (u8 i = 0; i < 16; i++)
	{
		_Palette[i] = Backing_palette_bin[i];
	}

	for (u8 i = 0; i < 16; i++)
	{
		_Palette2[i] = MiniProfessors_palette_bin[i];
	}

	//SMS_loadPSGaidencompressedTiles(atoms2_tiles_psgcompr, Backing_TILECOUNT);

	SMS_mapROMBank(2);
	SMS_loadPSGaidencompressedTiles(MenuCursor2_tiles_psgcompr, Backing_TILECOUNT + atoms2_TILECOUNT);


	SMS_useFirstHalfTilesforSprites(TRUE);
	u8 SpritePlace = (u8)(Backing_TILECOUNT + atoms2_TILECOUNT);

	_SpriteCursor[0][0] = SMS_addSprite(0, 0, SpritePlace);
	_SpriteCursor[0][1] = SMS_addSprite(8, 0, SpritePlace + 1);
	_SpriteCursor[0][2] = SMS_addSprite(0, 8, SpritePlace + 2);

	_SpriteCursor[1][0] = SMS_addSprite(16, 0, SpritePlace + 3);
	_SpriteCursor[1][1] = SMS_addSprite(24, 0, SpritePlace + 4);
	_SpriteCursor[1][2] = SMS_addSprite(24, 8, SpritePlace + 5);

	_SpriteCursor[2][0] = SMS_addSprite(0, 16, SpritePlace + 6);
	_SpriteCursor[2][1] = SMS_addSprite(0, 24, SpritePlace + 7);
	_SpriteCursor[2][2] = SMS_addSprite(8, 24, SpritePlace + 8);

	_SpriteCursor[3][0] = SMS_addSprite(24, 16, SpritePlace + 9);
	_SpriteCursor[3][1] = SMS_addSprite(16, 24, SpritePlace + 10);
	_SpriteCursor[3][2] = SMS_addSprite(24, 24, SpritePlace + 11);

	
	CursorX = 0;
	CursorY = 0;

	WinningPlayer = 0;

	GridSetup();
	SetupGame();

	UpdateCursor();

	for (int i = 0; i < 6; i++)
	{
		SMS_mapROMBank(_PlayerBanks[i]);
		SMS_loadTiles(_IdleTiles[i][0], ANIMATED_TILE_INDEX + _TileOffset[i + 1], p1_idle_anim_1_tiles_bin_size);
		SMS_loadTiles(_GrowTiles[i][0], ANIMATED_TILE_INDEX + _TileOffset[i + 1] + 20, p1_idle_anim_1_tiles_bin_size);
	}


	SMS_mapROMBank(2);
	SMS_loadSpritePalette(MenuCursor2_palette_bin);
	SMS_setSpritePaletteColor(0, _Palette[0]);
	SMS_mapROMBank(3);


	RestAnims();
	SMS_loadBGPalette(_Palette);

	setRandomSeed(RndPump * random());
	SMS_setSpritePaletteColor(0, 0);
	_Anim = 0;

	for (int i = 0; i < 7; i++)
	{
		_Counts[i] = 0;
	}

	UpdateProgress();
	_MiniFlash = 0;

	//SMS_setLineInterruptHandler(LineIRQ);
	//
	//SMS_setLineCounter(16);
	//SMS_enableLineInterrupt();
}


static u8 PauseCheck()
{
	if (SMS_queryPauseRequested())
	{
		SMS_resetPauseRequest();

		SMS_loadBGPaletteHalfBrightness(_Palette);
		HideCursor();
		_GameState = STATE_PAUSE;
		return TRUE;
	}

	return FALSE;
}

static void PlayerInput()
{
	if (PauseCheck())
	{
		return;
	}

	u8 moved = FALSE;

	if (Pads[0].Up == PAD_PRESSED)
	{
		CursorY--;
		moved = TRUE;
	}
	else if (Pads[0].Down == PAD_PRESSED)
	{
		CursorY++;
		moved = TRUE;
	}

	if (Pads[0].Left == PAD_PRESSED)
	{
		CursorX--;
		moved = TRUE;
	}
	else if (Pads[0].Right == PAD_PRESSED)
	{
		CursorX++;
		moved = TRUE;
	}

	if (CursorX > 9)
	{
		CursorX = 9;
	}
	else if (CursorX < 0)
	{
		CursorX = 0;
	}

	if (CursorY > 6)
	{
		CursorY = 6;
	}
	else if (CursorY < 0)
	{
		CursorY = 0;
	}

	if (moved)
	{
		UpdateCursor();
		PSGSFXPlay(blip_psg, 0);
	}
	else
	{
		if (Pads[0].A == PAD_RELEASED)
		{

			if (TryIncrementSquare(CursorX, CursorY, _CurrentPlayer))
			{
				HideCursor();
				// change logic
				_GameState = STATE_ANIMATE;
				_Anim = 0;
				_MiniFlash = 0;
				return;
			}
		}
	}

}


void AIInput()
{
	int attempt = 5;
	int done = 0;
	int rndx = 0;
	int rndy = 0;
	int player;

	while (attempt > 0 && done == 0)
	{
		do
		{
			rndx = random();
			rndx = (rndx & 0xF);
		} while (rndx > 9);

		do
		{
			rndy = random();
			rndy = rndy & 0x7;
		} while (rndy > 6);

		player = PlayerAtSquare(rndx, rndy);

		if (player == 0 || player == _CurrentPlayer)
		{
			IncrementSquare(rndx, rndy, _CurrentPlayer);
			done = 1;
		}

		attempt--;
	}

	while (!done)
	{
		rndx++;

		if (rndx > 9)
		{
			rndx = 0;
			rndy++;
		}

		if (rndy > 6)
		{
			rndy = 0;
			rndx = 0;
		}

		player = PlayerAtSquare(rndx, rndy);

		if (player == 0 || player == _CurrentPlayer)
		{
			IncrementSquare(rndx, rndy, _CurrentPlayer);
			done = 1;
		}
	}

	CursorX = rndx;
	CursorY = rndy;

	//XGM_startPlayPCM(SND_MOVE, 1, SOUND_PCM_CH2);
	_GameState = STATE_ANIMATE;
	_Anim = 0;

	UpdateCursor();
}


static void FlashMiniAtom()
{	
	u16 buffer[1];
	if (_MiniFlash == 0)
	{
		buffer[0] = ATOM_PROGRESS_1_INDEX + _CurrentPlayer - 1;
		SMS_loadTileMap(_ProgressPlaces[_CurrentPlayer - 1], 1, buffer, 2);
	}
	if (_MiniFlash == 5)
	{
		buffer[0] = ATOM_PROGRESS_2_INDEX + _CurrentPlayer - 1;
		SMS_loadTileMap(_ProgressPlaces[_CurrentPlayer - 1], 1, buffer, 2);
	}
	else if (_MiniFlash == 9)
	{
		_MiniFlash = -1;
	}
	
	_MiniFlash++;
}

static void AnimateScreen()
{
	u8 animating = 1;
	int x = 0;
	int y = 0;
	u8 j;
	u8 allSame = 1;
	s8 lastPlayer = -1;
	char exploded = 0;
	char done = 1;
	char grow = 0;


	for (j = 0; j < 70; j++)
	{
		if (_PlayerGrid[j].GrowSize && _PlayerGrid[j].Size != 5)
		{
			_PlayerGrid[j].Changed = 1;
			if (_PlayerGrid[j].GrowSize)
			{
				done = 0;
			}

			_PlayerGrid[j].ChangeAnim = 1;
			_PlayerGrid[j].Size += _PlayerGrid[j].GrowSize;

			if (_PlayerGrid[j].Size > _PlayerGrid[j].MaxSize)
			{
				_PlayerGrid[j].GrowSize = _PlayerGrid[j].Size - _PlayerGrid[j].MaxSize;
				_PlayerGrid[j].Size = _PlayerGrid[j].MaxSize;
			}
			else
			{
				_PlayerGrid[j].GrowSize = 0;
			}

			_PlayerGrid[j].Animate = 1;
		}
		else
		{
			_PlayerGrid[j].Animate = 0;
		}
	}

	if (_TurnCount)
	{
		for (j = 0; j < 7; j++)
		{
			_Alive[j] = 0;
		}
	}

	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			int i = (y * 10) + x;

			u16 size = _PlayerGrid[i].Size;
			u16 player = _PlayerGrid[i].Player;			

			if (lastPlayer == -1 && player != 0)
			{
				lastPlayer = player;
			}
			else if (lastPlayer != player && player != 0)
			{
				allSame = 0;
			}


			if (_TurnCount)
			{
				_Alive[player]++;
			}

			if (_PlayerGrid[i].Changed)
			{
				animating = 2;

				if (size == _PlayerGrid[i].MaxSize)
				{
					//DrawSquare(x, y, 4, player, 1);
					_PlayerGrid[i].Changed = 1;
					_PlayerGrid[i].Size = 5;
					exploded = 1;
					_PlayerGrid[i].Animate = 8;
				}
				else if (size == 5)
				{
					_PlayerGrid[i].Size = 0;
					_PlayerGrid[i].Player = 0;
					_PlayerGrid[i].Changed = 0;
					_PlayerGrid[i].Animate = 1;
					// Clear it and just set the Changed flag

					// Do explosion logic!
					
					if (y == 0 && x == 0)
					{
						u8 t = x + 1;
						IncrementSquare(t, y, player);
						IncrementSquare(x, y + 1, player);
					}
					else if (y == 0 && x == 9)
					{
						IncrementSquare(x - 1, y, player);
						IncrementSquare(x, y + 1, player);
					}
					else if (y == 0)
					{
						u8 t = x + 1;
						IncrementSquare(t, y, player);
						IncrementSquare(x - 1, y, player);
						IncrementSquare(x, y + 1, player);
					}
					else if (y == 6 && x == 0)
					{
						u8 t = x + 1;
						IncrementSquare(t, y, player);
						IncrementSquare(x, y - 1, player);
					}
					else if (x == 0)
					{
						u8 t = x + 1;
						IncrementSquare(x + 1, y, player);
						IncrementSquare(x, y - 1, player);
						IncrementSquare(x, y + 1, player);
					}
					else if (y == 6 && x == 9)
					{
						IncrementSquare(x - 1, y, player);
						IncrementSquare(x, y - 1, player);
					}
					else if (y == 6)
					{
						u8 t = x + 1;
						IncrementSquare(t, y, player);
						IncrementSquare(x - 1, y, player);
						IncrementSquare(x, y - 1, player);
					}
					else if (x == 9)
					{
						IncrementSquare(x - 1, y, player);
						IncrementSquare(x, y - 1, player);
						IncrementSquare(x, y + 1, player);
					}
					else
					{
						u8 t = x + 1;
						IncrementSquare(t, y, player);
						IncrementSquare(x, y - 1, player);
						IncrementSquare(x, y + 1, player);
						IncrementSquare(x - 1, y, player);
					}					
				}
				else
				{
					if (size)
					{
						//DrawSquare(x, y, size, player, 0);
						_PlayerGrid[i].Player = player;
						_PlayerGrid[i].Changed = 0;
						grow = 1;
					}
				}
			}
		}
	}
	if (done)
	{
		animating--;
	}

	if (exploded)
	{
		PSGSFXPlay(explosion0_psg, 0);
	}
	else if (grow)
	{
		PSGSFXPlay(hitHurt0_psg, 0);
	}

	// Allow for early out!
	if (allSame && _TurnCount)
	{
		_GameFinished = 1;
		WinningPlayer = _CurrentPlayer;
		animating = 0;
	}

	if (animating >= 1)
	{
		_GameState = STATE_ANIMATE_WAIT;			
	}
	else
	{
		_GameState = STATE_END_CHECK;
	}
}



void CheckForFinished()
{
	u16 startingPlayer = _CurrentPlayer;
	while (1)
	{
		_CurrentPlayer++;

		if (_CurrentPlayer > 6)
		{
			_CurrentPlayer = 1;
			_TurnCount++;
		}

		if (_TurnCount)
		{
			if (_CurrentPlayer == startingPlayer)
			{
				_GameFinished = 1;
				WinningPlayer = startingPlayer;
				break;
			}
			else if (_Alive[_CurrentPlayer] && PlayerSetup[_CurrentPlayer] != 0)
			{
				break;
			}
		}
		else
		{
			if (PlayerSetup[_CurrentPlayer] != 0)
			{
				break;
			}
		}
	}
}


static void UpdateVisiblePlayer()
{
	SMS_setSpritePaletteColor(4, _Palette[((_CurrentPlayer-1) * 2) + 4]);
	SMS_setSpritePaletteColor(5, _Palette[((_CurrentPlayer-1) * 2) + 5]);
}




static void Update()
{
	//SMS_loadBGPalette(_Palette2);
	_ChangePalette = 1;

	switch (_GameState)
	{
		case STATE_PLAY:
		{
			if (PauseCheck())
			{
				return;
			}

			if (PlayerSetup[_CurrentPlayer] == 1)
			{
				PlayerInput();
			}
			else if (PlayerSetup[_CurrentPlayer] == 2)
			{
				AIInput();
			}

			LoadIdleAnim(_CurrentPlayer, _Anim);
			
			FlashMiniAtom();
			++_Anim;
			if (_Anim == 32)
			{
				_Anim = 0;
			}

			break;
		}

		case STATE_ANIMATE:
		{			
			_AnimTimer = 20;
			_AnimCounter = 0;
			
			AnimateScreen();			
			break;
		}


		case STATE_ANIMATE_WAIT:
		{
			LoadGrowAnim(_CurrentPlayer, _AnimCounter);
			_AnimCounter++;
			if (_AnimCounter >= _AnimTimer)
			{
				_GameState = STATE_ANIMATE;
				RestAnims();
			}

			break;
		}


		case STATE_END_CHECK:
		{
			UpdateProgress();
			_CursorPositions[_CurrentPlayer].X = CursorX;
			_CursorPositions[_CurrentPlayer].Y = CursorY;

			CheckForFinished();

			if (_GameFinished)
			{
				WinningPlayer = _CurrentPlayer;
				StateMachineChange(&GameMachineState, &WinnerState);
				return;
			}
			else
			{
				UpdateVisiblePlayer();
				//SwitchPlayer();
				//UpdateUpNext();
				//EnableLoopingAnims();


				if (_CursorPositions[_CurrentPlayer].X != -1 && _CursorPositions[_CurrentPlayer].Y != -1)
				{
					CursorX = _CursorPositions[_CurrentPlayer].X;
					CursorY = _CursorPositions[_CurrentPlayer].Y;
				}

				_GameState = STATE_PLAY;
				_Anim = 0;
				UpdateCursor();
			}

			
			
			break;
		}

		case STATE_PAUSE:
			if (SMS_queryPauseRequested())
			{
				SMS_resetPauseRequest();
				SMS_loadBGPalette(_Palette);
				_GameState = STATE_PLAY;
				UpdateCursor();
			}
			break;
	}

	DrawGameGrid2();
}


static void End()
{
	SMS_zeroBGPalette();


	// Can't return sprites just "clear" all of them.
	SMS_initSprites();
	SMS_zeroSpritePalette();

	WaitForSFX();
}

SimpleState GamePlayClassicState =
{
	Enter,
	Update,
	End
};