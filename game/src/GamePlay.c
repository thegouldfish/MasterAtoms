#include "GamePlay.h"
#include "sms.h"

#include "gfx.h"
#include "gfxTileCount.h"

#include <SMSlib.h>

#define OFFSETX 2
#define OFFSETY 3


GridSquare _PlayerGrid[70];

int CursorX = 0;
int CursorY = 0;

u16 TileStartLocation = Backing_TILECOUNT;

char _SpriteCursor[4][3];


u8 _PlayerBanks[6] =
{
	9,
	9,
	9,
	10,
	10,
	10,
};

const unsigned char* _IdleTiles[6][4] =
{
	{p1_idle_anim_1_tiles_bin,p1_idle_anim_2_tiles_bin,p1_idle_anim_3_tiles_bin,p1_idle_anim_4_tiles_bin},
	{p2_idle_anim_1_tiles_bin,p2_idle_anim_2_tiles_bin,p2_idle_anim_3_tiles_bin,p2_idle_anim_4_tiles_bin},
	{p3_idle_anim_1_tiles_bin,p3_idle_anim_2_tiles_bin,p3_idle_anim_3_tiles_bin,p3_idle_anim_4_tiles_bin},
	{p4_idle_anim_1_tiles_bin,p4_idle_anim_2_tiles_bin,p4_idle_anim_3_tiles_bin,p4_idle_anim_4_tiles_bin},
	{p5_idle_anim_1_tiles_bin,p5_idle_anim_2_tiles_bin,p5_idle_anim_3_tiles_bin,p5_idle_anim_4_tiles_bin},
	{p6_idle_anim_1_tiles_bin,p6_idle_anim_2_tiles_bin,p6_idle_anim_3_tiles_bin,p6_idle_anim_4_tiles_bin}
};

const unsigned char* _GrowTiles[6][4] =
{
	{p1_grow_anim_1_tiles_bin,p1_grow_anim_2_tiles_bin,p1_grow_anim_3_tiles_bin,p1_grow_anim_4_tiles_bin},
	{p2_grow_anim_1_tiles_bin,p2_grow_anim_2_tiles_bin,p2_grow_anim_3_tiles_bin,p2_grow_anim_4_tiles_bin},
	{p3_grow_anim_1_tiles_bin,p3_grow_anim_2_tiles_bin,p3_grow_anim_3_tiles_bin,p3_grow_anim_4_tiles_bin},
	{p4_grow_anim_1_tiles_bin,p4_grow_anim_2_tiles_bin,p4_grow_anim_3_tiles_bin,p4_grow_anim_4_tiles_bin},
	{p5_grow_anim_1_tiles_bin,p5_grow_anim_2_tiles_bin,p5_grow_anim_3_tiles_bin,p5_grow_anim_4_tiles_bin},
	{p6_grow_anim_1_tiles_bin,p6_grow_anim_2_tiles_bin,p6_grow_anim_3_tiles_bin,p6_grow_anim_4_tiles_bin}
};

u8 _IdleIndex[2] = { 0, 4 };
u8 _CritialIndex[3] = { 2, 6, 8 };
u8 _Grow[4] = { 20, 22, 24, 26 };

u8 _TileOffset[7] =
{
	0,
	36 * 0,
	36 * 1,
	36 * 2,
	36 * 3,
	36 * 4,
	36 * 5,
};



void LoadIdleAnim(u8 current, u8 animCounter)
{
	SMS_mapROMBank(_PlayerBanks[current-1]);
	if (animCounter == 0)
	{
		SMS_loadTiles(_IdleTiles[current - 1][0], ANIMATED_TILE_INDEX + _TileOffset[current], p1_idle_anim_1_tiles_bin_size);
	}
	else if (animCounter == 8)
	{
		SMS_loadTiles(_IdleTiles[current - 1][1], ANIMATED_TILE_INDEX + _TileOffset[current], p1_idle_anim_1_tiles_bin_size);
	}
	else if (animCounter == 16)
	{
		SMS_loadTiles(_IdleTiles[current - 1][2], ANIMATED_TILE_INDEX + _TileOffset[current], p1_idle_anim_1_tiles_bin_size);
	}
	else if (animCounter == 24)
	{
		SMS_loadTiles(_IdleTiles[current - 1][3], ANIMATED_TILE_INDEX + _TileOffset[current], p1_idle_anim_1_tiles_bin_size);
	}
}


void LoadGrowAnim(u8 current, u8 animCounter)
{
	SMS_mapROMBank(_PlayerBanks[current - 1]);
	if (animCounter == 0)
	{
		SMS_loadTiles(_GrowTiles[current - 1][0], ANIMATED_TILE_INDEX + _TileOffset[current] + 20, p1_grow_anim_1_tiles_bin_size);
	}
	else if (animCounter == 6)
	{
		SMS_loadTiles(_GrowTiles[current - 1][1], ANIMATED_TILE_INDEX + _TileOffset[current] + 20, p1_grow_anim_1_tiles_bin_size);
	}
	else if (animCounter == 12)
	{
		SMS_loadTiles(_GrowTiles[current - 1][2], ANIMATED_TILE_INDEX + _TileOffset[current] + 20, p1_grow_anim_1_tiles_bin_size);
	}
	else if (animCounter == 18)
	{
		SMS_loadTiles(_GrowTiles[current - 1][3], ANIMATED_TILE_INDEX + _TileOffset[current] + 20, p1_grow_anim_1_tiles_bin_size);
	}
}


void RestAnims()
{
	for (int i = 0; i < 70; i++)
	{
		if (_PlayerGrid[i].ChangeAnim == 1)
		{
			_PlayerGrid[i].ChangeAnim = 0;
			_PlayerGrid[i].Animate = 1;
		}
	}
}

void GridSetup()
{
	u8 i = 0;
	u8 y = 0;
	u8 x = 0;

	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			_PlayerGrid[i].Player = 0;

			if (y == 0 && x == 0)
			{
				_PlayerGrid[i].MaxSize = 2;
			}
			else if (y == 0 && x == 9)
			{
				_PlayerGrid[i].MaxSize = 2;
			}
			else if (y == 0)
			{
				_PlayerGrid[i].MaxSize = 3;
			}
			else if (y == 6 && x == 0)
			{
				_PlayerGrid[i].MaxSize = 2;
			}
			else if (x == 0)
			{
				_PlayerGrid[i].MaxSize = 3;
			}
			else if (y == 6 && x == 9)
			{
				_PlayerGrid[i].MaxSize = 2;
			}
			else if (y == 6)
			{
				_PlayerGrid[i].MaxSize = 3;
			}
			else if (x == 9)
			{
				_PlayerGrid[i].MaxSize = 3;
			}
			else
			{
				_PlayerGrid[i].MaxSize = 4;
			}

			_PlayerGrid[i].X = x;
			_PlayerGrid[i].Y = y;

			_PlayerGrid[i].TileX = (x * 3) + OFFSETX;
			_PlayerGrid[i].TileY = (y * 3) + OFFSETY;

			i++;
		}
	}
}

void IncrementSquare(u8 x, u8 y, u8 player)
{
	u8 square = (y * 10) + x;

	_PlayerGrid[square].GrowSize++;
	_PlayerGrid[square].Changed = 1;
	_PlayerGrid[square].Player = player;
}

u8 TryIncrementSquare(u8 x, u8 y, u8 player)
{
	u8 square = (y * 10) + x;

	if (_PlayerGrid[square].Player == player || _PlayerGrid[square].Player == 0)
	{
		IncrementSquare(x, y, player);
		return TRUE;
	}

	return FALSE;
}

u8 PlayerAtSquare(u8 x, u8 y)
{
	return _PlayerGrid[(y * 10) + x].Player;
}

u8 SizeAtSquare(u8 x, u8 y)
{
	return _PlayerGrid[(y * 10) + x].Size;
}


void DrawAtom(s8 player, u8 x, u8 y, u8 size, u8 gridSquare)
{
	u16 buffer[4];
	u16 tileX = (x * 3) + OFFSETX;
	u16 tileY = (y * 3) + OFFSETY;
	size--;

	if (player < 0)
	{
		// Tile 0 is the blank tile
		buffer[0] = 0;
		buffer[1] = 0;
		buffer[2] = 0;
		buffer[3] = 0;
	}
	else
	{
		//const u16* data = MenuProfessors_tileMap_bin + offset;
		//u16 start = TileStartLocation + (player * 2) + (size * 24);
		u16 start = TileStartLocation + (player << 1) + (size * 24);

		buffer[0] = start;
		buffer[1] = start + 1;
		buffer[2] = start + 12;
		buffer[3] = start + 13;
	}


	SMS_loadTileMapArea(tileX, tileY, buffer, 2, 2);
}

void DrawFullGrid()
{
	int x = 0;
	int y = 0;

	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			int maxSize = _PlayerGrid[(y * 10) + x].MaxSize;

			if (maxSize == 4)
				maxSize = 5;

			int rnd = (((y + x) * y) + 5 / (x + 1)) % (6);
			DrawAtom(rnd, x, y, maxSize, (y * 10) + x);
		}
	}
}


void DrawGameGrid()
{
	int x = 0;
	int y = 0;

	GridSquare* square = _PlayerGrid;

	for (y = 0; y < 7; y++)
	{
		for (x = 0; x < 10; x++)
		{
			if (square->Animate != 0)
			{
				int size = square->Size;
				if (size == 5)
				{
					size = 4;
				}
				DrawAtom(square->Player - 1, x, y, size, 0);

				square->Animate = 0;
			}
			square++;
		}
	}
}

void UpdateCursor()
{
	u8 x = (CursorX * 24) + CURSOR_START_X;
	u8 y = (CursorY * 24) + CURSOR_START_Y;

	SMS_updateSpritePosition(_SpriteCursor[0][0], x + 0, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[0][1], x + 8, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[0][2], x + 0, y + 8);

	SMS_updateSpritePosition(_SpriteCursor[1][0], x + 12, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[1][1], x + 20, y + 0);
	SMS_updateSpritePosition(_SpriteCursor[1][2], x + 20, y + 8);

	SMS_updateSpritePosition(_SpriteCursor[2][0], x + 0, y + 12);
	SMS_updateSpritePosition(_SpriteCursor[2][1], x + 0, y + 20);
	SMS_updateSpritePosition(_SpriteCursor[2][2], x + 8, y + 20);

	SMS_updateSpritePosition(_SpriteCursor[3][0], x + 20, y + 12);
	SMS_updateSpritePosition(_SpriteCursor[3][1], x + 12, y + 20);
	SMS_updateSpritePosition(_SpriteCursor[3][2], x + 20, y + 20);
}

void HideCursor()
{
	for (u8 i = 0; i < 4; i++)
	{
		for (u8 j = 0; j < 3; j++)
		{
			SMS_hideSprite(_SpriteCursor[i][j]);
		}
	}
}



void DrawGameGrid2()
{
	u16 buffer[4];
	GridSquare* square = _PlayerGrid;
	GridSquare* end = _PlayerGrid + 70;
	for (; square != end; square++)
	{
		if (square->Animate != 0)
		{
			s8 size = square->Size;

			if (square->Player == 0)
			{
				// Tile 0 is the blank tile
				buffer[0] = 0;
				buffer[1] = 0;
				buffer[2] = 0;
				buffer[3] = 0;
			}
			else
			{
				u16 start = 0;
				u16 offset = 0;

				if (square->ChangeAnim == 0)
				{
					// special case
					if (size == 5)
					{
						start = ANIMATED_TILE_INDEX + _Grow[size - 2] + _TileOffset[square->Player];
						offset = 8;
					}
					// Critical
					else if (size + 1 == square->MaxSize)
					{
						start = ANIMATED_TILE_INDEX + _CritialIndex[size - 1] + _TileOffset[square->Player];
						offset = 10;
					}
					// idle size
					else
					{
						start = ANIMATED_TILE_INDEX + _IdleIndex[size - 1] + _TileOffset[square->Player];
						offset = 10;
					}
				}
				else
				{
					if (size == 5)
					{
						size--;
					}

					// Grow
					start = ANIMATED_TILE_INDEX + _Grow[size - 1] + _TileOffset[square->Player];
					offset = 8;
				}

				buffer[0] = start;
				buffer[1] = start + 1;
				buffer[2] = start + offset;
				buffer[3] = start + offset + 1;
			}
			SMS_loadTileMapArea(square->TileX, square->TileY, buffer, 2, 2);
			square->Animate = 0;
		}
	}
}