#include <SMSlib.h>
#include <PSGlib.h>

#include "../sms.h"
#include "../fixed.h"
#include "../PadHelper.h"

#include "ChallengeModeState.h"
#include "GameOverState.h"
#include "../Game.h"
#include "../GamePlay.h"


#include "../gfx.h"
#include "../Audio/sfx.h"

// Globals
u32 Score = 0;
u8 CurrentLevel = 0;

// States
#define CHALLENGEMODE_STATE_PLAY 0
#define CHALLENGEMODE_STATE_ANIMATE 1
#define CHALLENGEMODE_STATE_ANIMATE_WAIT 2
#define CHALLENGEMODE_STATE_END_CHECK 3
#define CHALLENGEMODE_STATE_LEVEL_UP 4
#define CHALLENGEMODE_STATE_GAME_OVER 5
#define CHALLENGEMODE_STATE_RESET 6
#define CHALLENGEMODE_STATE_PAUSED 7
#define CHALLENGEMODE_STATE_COUNTDOWN 8



// Locals
static u8 _GameState = CHALLENGEMODE_STATE_PLAY;

static u8 _LevelNeeded = 2;

static fixed _InitalLevelingAmount = FIXED(2.0);
static fixed _LevelBoosting = FIXED(1.25);
static fixed _LevelingAmount;


static s8 _AtomNeededCount[7];

static u8 _NextAtom;
static u8 _CurrentAtom;


static const fixed _InitalMaxTime = FIXED(70.0);
static const fixed _InitalTimePerExplosion = FIXED(1.5);


static fixed _Time;
static fixed _MaxTime;
static fixed _MaxTimeDecrease = FIXED(0.85);
static fixed _TimePerExplosion;
static fixed _TimePenaty = FIXED(0.60);
static fixed _TimeTick = FIXED(0.01666); // about 1/60 of a second, 





static float _Multiplier = 1;
static float _ScorePerExplotion = 1;


static u8 _Exploded = 0;
static u8 _EarlyOut;


#define RandomBagCount 18
static u8 _RandomBag[RandomBagCount];
static u8 _CurrentRnd = 0;


static u8 _AnimTimer = 0;
static u8 _AnimCounter = 0;

#define WAITFOR 100
#define WAITFORCOUNTDOWN 30
static s16 _levelWait = WAITFOR;
static u8 _CountDown = 0;


static unsigned char _Palette[16];

static u16 _Buffer[10];


static u8 _DrawUpdateState = 0;
static u8 _MiniFlash = 0;

#define INFOSPRITE_COUNT 32
static char _InfoSprite[INFOSPRITE_COUNT];


#define ATOMS_TILE_INDEX  ChallengeModeBacking_TILECOUNT
#define NUMBERS_TILE_INDEX  ChallengeModeBacking_TILECOUNT + atoms2_TILECOUNT
#define MINIATOMS_TILE_INDEX  ChallengeModeBacking_TILECOUNT + atoms2_TILECOUNT + Numbers_TILECOUNT
#define TIMER_TILE_INDEX  ChallengeModeBacking_TILECOUNT + atoms2_TILECOUNT + Numbers_TILECOUNT + MiniAtoms_TILECOUNT
#define CURSOR_TILE_INDEX  ChallengeModeBacking_TILECOUNT + atoms2_TILECOUNT + Numbers_TILECOUNT + MiniAtoms_TILECOUNT + timer_bar_TILECOUNT

#define INFO_TILE_INDEX  ChallengeModeBacking_TILECOUNT + atoms2_TILECOUNT + Numbers_TILECOUNT + MiniAtoms_TILECOUNT + timer_bar_TILECOUNT + MenuCursor2_TILECOUNT


#define SetNumber(number, atomNumber) Draw2DigitNumber(number, 8 + (atomNumber * 3), 1);

static s8 _Anim = 0;

#define SCORE_DIGITS 6
#define SCORE_ADDER 5
static u8 _ScoreBank[SCORE_DIGITS];

static void Draw2DigitNumber(int number, u8 x, u8 y)
{
	if (number > 99)
	{
		_Buffer[0] = NUMBERS_TILE_INDEX + 9;
		_Buffer[1] = NUMBERS_TILE_INDEX + 9;
	}
	else if (number > 9)
	{
		int count = 0;

		while (number > 9)
		{
			number -= 10;
			count++;
		}

		_Buffer[0] = NUMBERS_TILE_INDEX + count;
		_Buffer[1] = NUMBERS_TILE_INDEX + number;
	}
	else
	{
		_Buffer[0] = NUMBERS_TILE_INDEX;
		_Buffer[1] = NUMBERS_TILE_INDEX + number;
	}

	SMS_loadTileMap(x, y, _Buffer, 4);
}


static void DrawXDigitNumber(u32 number, u8 digit, u8 x, u8 y)
{
	s16 numbs[6] = { 0,0,0,0,0,0 };

	if (number > 999999)
	{
		for (int i = 0; i < 6; i++)
		{
			numbs[i] = 9;
		}
		SMS_loadTileMap(x, y, _Buffer, digit * 2);
		return;
	}

	if (digit > 6)
	{
		digit = 6;
	}

	int i = digit -1;
	while (number > 0)
	{
		numbs[i] = (number % 10);
		number /= 10;
		i--;
	}

	for (int i = 0; i < digit; i++)
	{
		_Buffer[i] = NUMBERS_TILE_INDEX + numbs[i];
	}

	SMS_loadTileMap(x, y, _Buffer, digit * 2);
}

static void SetScore(u32 number)
{
	s16 numbs[6] = {0,0,0,0,0,0};

	if (Score > 999999)
	{
		for (int i = 0; i < 6; i++)
		{
			_Buffer[i] = NUMBERS_TILE_INDEX + 9;
		}
	}
	else
	{
		s8 current = SCORE_ADDER;
		s8 next = current - 1;

		do
		{
			if (_ScoreBank[current] >= 10)
			{
				if (next >= 0)
				{
					_ScoreBank[current] -= 10;
					++_ScoreBank[next];
				}
				else
				{
					_ScoreBank[current] = 9;
				}
			}
			else
			{
				_Buffer[current] = NUMBERS_TILE_INDEX + _ScoreBank[current];
				--current;
				--next;
			}
		} while (current >= 0);
	}

	//int i = 5;
	//while (number > 0)
	//{
	//	numbs[i] = (number % 10);
	//	number /= 10;
	//	i--;
	//}
	//
	//for (int i = 0; i < 6; i++)
	//{
	//	_Buffer[i] = NUMBERS_TILE_INDEX + numbs[i];		
	//}

	SMS_loadTileMap(2, 0, _Buffer, 12);
}


static void HideInfoSprites()
{
	char* itr = _InfoSprite;
	for (int i = 0; i < 32; i++)
	{
		SMS_hideSprite(*itr);
		itr++;
	}
}


#define INFO_SPRITE_LEVELUP 0
#define INFO_SPRITE_TIMELOST 1
#define INFO_SPRITE_PAUSED 2
#define INFO_SPRITE_THREE 3
#define INFO_SPRITE_TWO 4
#define INFO_SPRITE_ONE 5


static void ShowInfoSprites(u8 message) 
{
	u16 x = 96;
	u16 y = 80;
	
	SMS_mapROMBank(3);

	switch (message)
	{
		case INFO_SPRITE_LEVELUP:
			SMS_loadTiles(levelup_tiles_bin, INFO_TILE_INDEX, levelup_tiles_bin_size);
			break;
	
		case INFO_SPRITE_TIMELOST:
			SMS_loadTiles(TimeLost_tiles_bin, INFO_TILE_INDEX, TimeLost_tiles_bin_size);
			break;

		case INFO_SPRITE_PAUSED:
			SMS_loadTiles(Paused_tiles_bin, INFO_TILE_INDEX, Paused_tiles_bin_size);
			break;

		case INFO_SPRITE_THREE:
			SMS_loadTiles(three_tiles_bin, INFO_TILE_INDEX, Paused_tiles_bin_size);
			break;

		case INFO_SPRITE_TWO:
			SMS_loadTiles(two_tiles_bin, INFO_TILE_INDEX, Paused_tiles_bin_size);
			break;

		case INFO_SPRITE_ONE:
			SMS_loadTiles(one_tiles_bin, INFO_TILE_INDEX, Paused_tiles_bin_size);
			break;
	}

	char* itr = _InfoSprite;
	for (int i = 0; i < 32; i++)
	{
		SMS_updateSpritePosition(*itr, x, y);
		x += 8;
		if (x >= 160)
		{
			x = 96;
			y += 8;
		}
		itr++;
	}
}

static void SetupRandom(int prev)
{
	for (int i = 0; i < RandomBagCount; i++)
	{
		_RandomBag[i] = 0;
	}


	int r = RandomBagCount / 6;


	for (int k = 0; k < r; k++)
	{
		int place = k * 6;

		for (int i = 0; i < 6; i++)
		{
			while (1)
			{
				int rnd = random_int(1, 6);

				if (rnd != prev)
				{
					int count = 0;
					for (int j = 0; j < 6; j++)
					{
						if (_RandomBag[place + j] == rnd)
						{
							count++;
						}
					}

					if (count == 0)
					{
						prev = rnd;
						_RandomBag[place + i] = rnd;
						break;
					}
				}
			}
		}
	}
}


static void RandomiseGrid()
{
	for (int i = 0; i < 70; i++)
	{
		_PlayerGrid[i].Changed = 0;
		_PlayerGrid[i].GrowSize = 0;
		_PlayerGrid[i].Player = 0;
		_PlayerGrid[i].Size = 0;
		_PlayerGrid[i].Animate = 1;
		_PlayerGrid[i].Explode = 0;
		_PlayerGrid[i].ChangeAnim = 0;
	}

	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			u16 player = random_int(0, 6);
			u16 size = 0;
			if (player != 0)
			{
				size = random_int(1, _PlayerGrid[(y * 10) + x].MaxSize - 1);
			}

			_PlayerGrid[(y * 10) + x].Size = size;
			_PlayerGrid[(y * 10) + x].Player = player;
		}
	}
}

static void PlayerInput()
{
	if (Pads[0].Up == PAD_PRESSED)
	{
		CursorY--;
		PSGSFXPlay(blip_psg, 0);
	}
	else if (Pads[0].Down == PAD_PRESSED)
	{
		CursorY++;
		PSGSFXPlay(blip_psg, 0);
	}

	if (Pads[0].Left == PAD_PRESSED)
	{
		CursorX--;
		PSGSFXPlay(blip_psg, 0);
	}
	else if (Pads[0].Right == PAD_PRESSED)
	{
		CursorX++;
		PSGSFXPlay(blip_psg, 0);
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

	if (Pads[0].A == PAD_RELEASED)
	{
		if (TryIncrementSquare(CursorX, CursorY, _CurrentAtom))
		{
			_GameState = CHALLENGEMODE_STATE_ANIMATE;
			_Anim = 0;
		}
	}

	UpdateCursor();

	if (SMS_queryPauseRequested())
	{
		SMS_resetPauseRequest();

		SMS_loadBGPaletteHalfBrightness(_Palette);
		ShowInfoSprites(INFO_SPRITE_PAUSED);
		HideCursor();
		_GameState = CHALLENGEMODE_STATE_PAUSED;
	}	
}

static void SetupLevel()
{
	if (CurrentLevel > 0)
	{
		_MaxTime = Fixed_mul(_MaxTime, _MaxTimeDecrease);
		_TimePerExplosion = Fixed_mul(_TimePerExplosion, _MaxTimeDecrease);
	}

	CurrentLevel++;

	_AtomNeededCount[0] = 0;
	for (int i = 1; i < 7; i++)
	{
		_AtomNeededCount[i] = _LevelNeeded;
	}

	RandomiseGrid();

	_LevelingAmount = _LevelingAmount + _LevelBoosting;
	_LevelNeeded += Fixed_toInt(_LevelingAmount);

	// Cap at 99 for now as that is the max number we can show with just 2 number slots
	if (_LevelNeeded > 99)
	{
		_LevelNeeded = 99;
	}
}


static void NextAtom()
{
	_CurrentAtom = _NextAtom;
	_NextAtom = _RandomBag[_CurrentRnd];
	_CurrentRnd++;

	if (_CurrentRnd >= RandomBagCount)
	{
		SetupRandom(_CurrentAtom);
		_CurrentRnd = 0;
	}

	SMS_setSpritePaletteColor(4, _Palette[((_CurrentAtom - 1) * 2) + 4]);
	SMS_setSpritePaletteColor(5, _Palette[((_CurrentAtom - 1) * 2) + 5]);

	_Buffer[0] = MINIATOMS_TILE_INDEX + (_NextAtom - 1);
	SMS_loadTileMap(27, 1, _Buffer, 2);

	_Buffer[0] = MINIATOMS_TILE_INDEX + (_CurrentAtom - 1);
	SMS_loadTileMap(29, 1, _Buffer, 2);
}


static void UpdateNumbers()
{
	for (int i = 0; i < 6; i++)
	{
		SetNumber(_AtomNeededCount[i + 1], i);
	}
}

static void CleanUp()
{
	_Anim = 0;
	for (int y = 0; y < 7; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			_PlayerGrid[(y * 10) + x].GrowSize = 0;
			_PlayerGrid[(y * 10) + x].Animate = 1;


			if (_PlayerGrid[(y * 10) + x].Player == 0)
			{
				_PlayerGrid[(y * 10) + x].Size = 0;
			}
		}
	}
}


static s8 CheckForSpace()
{
	for (int i = 0; i < 70; i++)
	{
		if (_PlayerGrid[i].Player == 0)
		{
			return TRUE;
		}
		else if (_PlayerGrid[i].Player == _CurrentAtom)
		{
			return TRUE;
		}
	}

	return FALSE;
}


static void UpdateTime()
{
	u16 whole = Fixed_toInt(_Time);
	u16 frac = (Fixed_frac(_Time) / 255.0f) * 99;

	Draw2DigitNumber(whole, 25, 0);
	Draw2DigitNumber(frac, 28, 0);
	//DrawXDigitNumber(frac,3, 28, 0);
}



static void AnimateScreen()
{
	s8 animating = 1;
	u8 allSame = 1;
	s8 lastPlayer = -1;
	u8 exploded = 0;
	u8 done = 1;
	u8 grow = 0;
	_EarlyOut = 0;

	GridSquare* square = _PlayerGrid;
	GridSquare* endSquare = square + 70;
	//for (int j = 0; j < 70; j++)
	for (;square != endSquare;square++)
	{
		if (square->GrowSize && square->Size != 5)
		{
			square->Changed = 1;
			if (square->GrowSize)
			{
				done = 0;
			}

			square->Size += square->GrowSize;
			square->ChangeAnim = 1;

			if (square->Size > square->MaxSize)
			{
				square->GrowSize = square->Size - square->MaxSize;
				square->Size = square->MaxSize;
			}
			else
			{
				square->GrowSize = 0;
			}

			square->Animate = 1;
		}
		else
		{
			square->Animate = 0;
		}
	}

	square = _PlayerGrid;
	for (; square != endSquare; square++)
	{
		u16 size = square->Size;
		u16 player = square->Player;

		if (lastPlayer == -1 && player != 0)
		{
			lastPlayer = player;
		}
		else if (lastPlayer != player && player != 0)
		{
			allSame = 0;
		}

		if (square->Changed)
		{
			animating = 2;

			if (size == square->MaxSize)
			{
				square->Changed = 1;
				square->Size = 5;
				exploded = 1;
				square->Animate = 8;

				_AtomNeededCount[square->Player]--;

				if (_AtomNeededCount[square->Player] < 0)
				{
					_AtomNeededCount[square->Player] = 0;
				}


				_Time += _TimePerExplosion;

				u8 toAdd = (int)(_Multiplier * _ScorePerExplotion);

				Score += toAdd;
				_ScoreBank[SCORE_ADDER] += toAdd;

				_Exploded = 1;
			}
			else if (size == 5)
			{
				square->Size = 0;
				square->Player = 0;
				square->Changed = 0;
				square->Animate = 1;
				// Clear it and just set the Changed flag

				// Do explosion logic!

				if (square->Y == 0 && square->X == 0)
				{
					IncrementSquare(square->X + 1, square->Y, player);
					IncrementSquare(square->X, square->Y + 1, player);
				}
				else if (square->Y == 0 && square->X == 9)
				{
					IncrementSquare(square->X - 1, square->Y, player);
					IncrementSquare(square->X, square->Y + 1, player);
				}
				else if (square->Y == 0)
				{
					IncrementSquare(square->X + 1, square->Y, player);
					IncrementSquare(square->X - 1, square->Y, player);
					IncrementSquare(square->X, square->Y + 1, player);
				}
				else if (square->Y == 6 && square->X == 0)
				{
					IncrementSquare(square->X + 1, square->Y, player);
					IncrementSquare(square->X, square->Y - 1, player);
				}
				else if (square->X == 0)
				{
					IncrementSquare(square->X + 1, square->Y, player);
					IncrementSquare(square->X, square->Y - 1, player);
					IncrementSquare(square->X, square->Y + 1, player);
				}
				else if (square->Y == 6 && square->X == 9)
				{
					IncrementSquare(square->X - 1, square->Y, player);
					IncrementSquare(square->X, square->Y - 1, player);
				}
				else if (square->Y == 6)
				{
					IncrementSquare(square->X + 1, square->Y, player);
					IncrementSquare(square->X - 1, square->Y, player);
					IncrementSquare(square->X, square->Y - 1, player);
				}
				else if (square->X == 9)
				{
					IncrementSquare(square->X - 1, square->Y, player);
					IncrementSquare(square->X, square->Y - 1, player);
					IncrementSquare(square->X, square->Y + 1, player);
				}
				else
				{
					IncrementSquare(square->X + 1, square->Y, player);
					IncrementSquare(square->X, square->Y - 1, player);
					IncrementSquare(square->X, square->Y + 1, player);
					IncrementSquare(square->X - 1, square->Y, player);
				}
			}
			else
			{
				if (size)
				{
					square->Player = player;
					square->Changed = 0;
					grow = 1;
				}
			}
		}
	}

	if (done)
	{
		animating--;
	}

	// Allow for early out!
	if (allSame)
	{
		animating = 0;
		_EarlyOut = 1;
		_GameState = CHALLENGEMODE_STATE_END_CHECK;
	}

	if (animating >= 1)
	{
		_GameState = CHALLENGEMODE_STATE_ANIMATE_WAIT;

		if (exploded)
		{
			PSGSFXPlay(explosion0_psg, 0);
		}
		else if (grow)
		{
			PSGSFXPlay(hitHurt0_psg, 0);
		}
		
	}
	else
	{
		_GameState = CHALLENGEMODE_STATE_END_CHECK;
	}
}

static void FlashMiniAtom()
{
	_MiniFlash++;
	u16 buffer[1];
	if (_MiniFlash == 5)
	{
		_MiniFlash = 0;
		if (_AnimCounter == 0)
		{
			buffer[0] = ATOMS_TILE_INDEX + _CurrentAtom - 1;
			_AnimCounter = 1;
		}
		else
		{
			buffer[0] = 5 + _CurrentAtom - 1;
			_AnimCounter = 0;
		}	

		SMS_loadTileMap(7 + ((_CurrentAtom - 1)*3), 1, buffer, 2);
	}
}

static void Enter()
{
	TileStartLocation = ATOMS_TILE_INDEX;
	SMS_mapROMBank(3);
	_DrawUpdateState = 0;

	SMS_loadPSGaidencompressedTiles(ChallengeModeBacking_tiles_psgcompr, 0);
	SMS_loadSTMcompressedTileMap(0, 0, ChallengeModeBacking_tileMap_stmcompr);


//	SMS_loadPSGaidencompressedTiles(atoms2_tiles_psgcompr, ATOMS_TILE_INDEX);
	SMS_loadTiles(miniAtoms2_tiles_bin, ATOMS_TILE_INDEX, miniAtoms2_tiles_bin_size);


	//SMS_loadPSGaidencompressedTiles(atoms2_tiles_psgcompr, ATOMS_TILE_INDEX);

	SMS_loadTiles(Numbers_tiles_bin, NUMBERS_TILE_INDEX, Numbers_tiles_bin_size);
	
	SMS_loadTiles(MiniAtoms_tiles_bin, MINIATOMS_TILE_INDEX, MiniAtoms_tiles_bin_size);
	
	SMS_loadTiles(timer_bar_tiles_bin, TIMER_TILE_INDEX, timer_bar_tiles_bin_size);
	
	SMS_loadTiles(levelup_tiles_bin, INFO_TILE_INDEX, levelup_tiles_bin_size);

	for (u8 i = 0; i < 16; i++)
	{
		_Palette[i] = ChallengeModeBacking_palette_bin[i];
	}

	SMS_mapROMBank(2);
	SMS_loadPSGaidencompressedTiles(MenuCursor2_tiles_psgcompr, CURSOR_TILE_INDEX);


	
	SMS_useFirstHalfTilesforSprites(TRUE);
	_SpriteCursor[0][0] = SMS_addSprite(0, 0,   CURSOR_TILE_INDEX);
	_SpriteCursor[0][1] = SMS_addSprite(8, 0,   CURSOR_TILE_INDEX + 1);
	_SpriteCursor[0][2] = SMS_addSprite(0, 8,   CURSOR_TILE_INDEX + 2);
											    
	_SpriteCursor[1][0] = SMS_addSprite(16, 0,  CURSOR_TILE_INDEX + 3);
	_SpriteCursor[1][1] = SMS_addSprite(24, 0,  CURSOR_TILE_INDEX + 4);
	_SpriteCursor[1][2] = SMS_addSprite(24, 8,  CURSOR_TILE_INDEX + 5);
											    
	_SpriteCursor[2][0] = SMS_addSprite(0, 16,  CURSOR_TILE_INDEX + 6);
	_SpriteCursor[2][1] = SMS_addSprite(0, 24,  CURSOR_TILE_INDEX + 7);
	_SpriteCursor[2][2] = SMS_addSprite(8, 24,  CURSOR_TILE_INDEX + 8);
												
	_SpriteCursor[3][0] = SMS_addSprite(24, 16, CURSOR_TILE_INDEX + 9);
	_SpriteCursor[3][1] = SMS_addSprite(16, 24, CURSOR_TILE_INDEX + 10);
	_SpriteCursor[3][2] = SMS_addSprite(24, 24, CURSOR_TILE_INDEX + 11);


	u8 c = INFO_TILE_INDEX;
	char* itr = _InfoSprite;
	for (int i = 0; i < 32; i++)
	{
		*itr = SMS_addSprite(i*8, 0, c);
		SMS_hideSprite(*itr);
		itr++;
		c++;
	}
		
	CursorX = 0;
	CursorY = 0;

	GridSetup();

	//setRandomSeed(5);
	//RndPump = 5;

	setRandomSeed(RndPump);
	SetupRandom(0);
	_LevelingAmount = _InitalLevelingAmount;
	_LevelNeeded = 2;
	SetupLevel();
	UpdateCursor();

	_NextAtom = random_int(1, 6);
	_NextAtom = _RandomBag[0];
	_CurrentRnd = 1;
	_Multiplier = 1;

	_TimePerExplosion = _InitalTimePerExplosion;
	_MaxTime = _InitalMaxTime;
	_Time = _MaxTime;

	_GameState = CHALLENGEMODE_STATE_COUNTDOWN;

	//NextAtom();


	for (int i = 0; i < 6; i++)
	{
		SMS_mapROMBank(_PlayerBanks[i]);
		SMS_loadTiles(_IdleTiles[i][0], ANIMATED_TILE_INDEX + _TileOffset[i+1], p1_idle_anim_1_tiles_bin_size);
		SMS_loadTiles(_GrowTiles[i][0], ANIMATED_TILE_INDEX + _TileOffset[i+1] + 20, p1_idle_anim_1_tiles_bin_size);
	}


	_Anim = 0;
	Score = 0;

	for (int i = 0; i < SCORE_DIGITS; i++)
	{
		_ScoreBank[i] = 0;
	}

	CurrentLevel = 0;



	_MiniFlash = 0;
	//DrawGameGrid();
	SetScore(Score);
	UpdateNumbers();
	Draw2DigitNumber((u32)_Multiplier, 2, 1);

		
	ShowInfoSprites(INFO_SPRITE_THREE);



	
	HideCursor();

	_CountDown = 3;
	_levelWait = WAITFORCOUNTDOWN;


	DrawGameGrid2();

	SMS_mapROMBank(2);
	SMS_loadSpritePalette(MenuCursor2_palette_bin);
	SMS_setSpritePaletteColor(0, 0);
	SMS_loadBGPaletteHalfBrightness(_Palette);
	PSGSFXPlay(hitHurt0_psg, 0);

	ResetPads();

	NextAtom();
}


static void Update()
{
	s8 count = 0;

	switch (_GameState)
	{
		case CHALLENGEMODE_STATE_PLAY:
			PlayerInput();

			_Exploded = 0;
			_Time -= _TimeTick;

			if (_Time < 0)
			{
				_Time = 0;
				_GameState = CHALLENGEMODE_STATE_GAME_OVER;
			}

			//IdleAnim();
			LoadIdleAnim(_CurrentAtom, _Anim);
			FlashMiniAtom();
			SMS_mapROMBank(3);
			++_Anim;

			if (_Anim == 32)
			{
				_Anim = 0;
			}
			break;
	
		case CHALLENGEMODE_STATE_ANIMATE:
			HideCursor();
			AnimateScreen();
			_AnimTimer = 20;
			_AnimCounter = 0;

			SetNumber(_AtomNeededCount[_CurrentAtom], (_CurrentAtom-1));

			SetScore(Score);
			Draw2DigitNumber((u32)_Multiplier, 2, 1);
			break;

		case CHALLENGEMODE_STATE_ANIMATE_WAIT:
			LoadGrowAnim(_CurrentAtom, _AnimCounter);
			++_AnimCounter;
			if (_AnimCounter >= _AnimTimer)
			{
				_GameState = CHALLENGEMODE_STATE_ANIMATE;
				RestAnims();
			}
			break;

		case CHALLENGEMODE_STATE_END_CHECK:	
			count = 0;
			for (int i = 1; i < 7; i++)
			{
				if (_AtomNeededCount[i] > 0)
				{
					count++;
				}
			}

			if (count == 0)
			{
				_GameState = CHALLENGEMODE_STATE_LEVEL_UP;
				_levelWait = WAITFOR;
			}
			else
			{
				if (_EarlyOut)
				{
					_levelWait = WAITFOR;
					_GameState = CHALLENGEMODE_STATE_RESET;
				}
				else
				{
					_GameState = CHALLENGEMODE_STATE_PLAY;
					CleanUp();

					NextAtom();

					if (!CheckForSpace())
					{
						_levelWait = WAITFOR;
						_GameState = CHALLENGEMODE_STATE_RESET;
					}
				}
			}

			if (_Exploded)
			{
				_Multiplier++;

				if (_Multiplier > CurrentLevel)
				{
					_Multiplier = CurrentLevel + 1;
				}
			}
			else
			{
				_Multiplier = 1;
			}
			break;
		
		case CHALLENGEMODE_STATE_LEVEL_UP:
			if (_levelWait == WAITFOR)
			{
				SMS_loadBGPaletteHalfBrightness(_Palette);
				ShowInfoSprites(INFO_SPRITE_LEVELUP);

				SetupLevel();
				SetupRandom(0);
				HideCursor();

				
			}

			if (_levelWait < 0)
			{
				SMS_loadBGPalette(_Palette);
				HideInfoSprites();

				_GameState = CHALLENGEMODE_STATE_PLAY;
				_DrawUpdateState == 2;

				_Anim = 0;
				_levelWait = WAITFOR + 1;
				UpdateCursor();
				
				for (int i = 0; i < 6; i++)
				{
					SetNumber(_AtomNeededCount[i + 1], i);
				}
			}

			_levelWait--;
			break;

		case CHALLENGEMODE_STATE_RESET:
			if (_levelWait == WAITFOR)
			{
				SMS_loadBGPaletteHalfBrightness(_Palette);
				ShowInfoSprites(INFO_SPRITE_TIMELOST);

				RandomiseGrid();
				SetupRandom(_CurrentAtom);
				HideCursor();
				_Multiplier = 1;
				_Time = Fixed_mul(_Time, _TimePenaty);
			}
			if (_levelWait < 0)
			{
				SMS_loadBGPalette(_Palette);
				HideInfoSprites();

				_GameState = CHALLENGEMODE_STATE_PLAY;
				_Anim = 0;
				_levelWait = WAITFOR + 1;
				UpdateCursor();
			}

			_levelWait--;
			break;

		case CHALLENGEMODE_STATE_GAME_OVER:
			StateMachineChange(&GameMachineState, &GameOverState);
			return;
		

		case CHALLENGEMODE_STATE_PAUSED:
			if (SMS_queryPauseRequested())
			{
				SMS_resetPauseRequest();
				SMS_loadBGPalette(_Palette);
				HideInfoSprites();
				_GameState = CHALLENGEMODE_STATE_PLAY;
				UpdateCursor();
			}
			break;
		


		case CHALLENGEMODE_STATE_COUNTDOWN:
			_levelWait--;
			if (_levelWait == 0)
			{
				_CountDown--;

				switch (_CountDown)
				{
					case 2:
						ShowInfoSprites(INFO_SPRITE_TWO);
						_levelWait = WAITFORCOUNTDOWN;
						PSGSFXPlay(hitHurt0_psg, 0);
						break;

					case 1:
						ShowInfoSprites(INFO_SPRITE_ONE);
						_levelWait = WAITFORCOUNTDOWN;
						PSGSFXPlay(hitHurt0_psg, 0);

						break;

					case 0:
						HideInfoSprites();
						SMS_loadBGPalette(_Palette);
						PSGSFXPlay(laserShoot0_psg, 3);
						_GameState = CHALLENGEMODE_STATE_PLAY;
				}

			}
			break;
	}


	if (_GameState != CHALLENGEMODE_STATE_GAME_OVER)
	{
		for (int i = 1; i < 7; i++)
		{
			if (_AtomNeededCount[i] < 0)
			{
				_AtomNeededCount[i] = 0;
			}
		}


		if (_Time > _MaxTime)
		{
			_Time = _MaxTime;
		}

		DrawGameGrid2();
		
		if (_DrawUpdateState == 2)
		{
			UpdateTime();
		}

		_DrawUpdateState++;
		if (_DrawUpdateState > 3)
		{
			_DrawUpdateState = 0;
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
}

SimpleState ChallengeModeState =
{
	Enter,
	Update,
	End
};
