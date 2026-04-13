#ifndef _GAMEPLAY_H_
#define _GAMEPLAY_H_

#include "sms.h"

#define atoms2_TILECOUNT 72

typedef struct
{
	u8 Player;
	//u8 PreviousPlayer;

	s8 Size;
	s8 GrowSize;
	s8 Changed;
	s8 MaxSize;
	s8 Explode;

	s8 Animate;

	u8 X;
	u8 Y;

	s8 ChangeAnim;

	u16 TileX;
	u16 TileY;
}GridSquare;

typedef struct
{
	const unsigned char* Tiles;
	const unsigned char* Map;
	const unsigned char* Palette;
	const u8 Bank;
} ScreenDetails;

extern GridSquare _PlayerGrid[70];

extern int CursorX;
extern int CursorY;

extern u16 TileStartLocation;


#define ANIMATED_TILE_INDEX 208
extern u8 _PlayerBanks[6];

extern const unsigned char* _IdleTiles[6][4];
extern const unsigned char* _GrowTiles[6][4];

extern u8 _IdleIndex[2];
extern u8 _CritialIndex[3];
extern u8 _Grow[4];
extern u8 _TileOffset[7];




#define CURSOR_START_X 10
#define CURSOR_START_Y 18
extern char _SpriteCursor[4][3];

void GridSetup();

void IncrementSquare(u8 x, u8 y, u8 player);

u8 TryIncrementSquare(u8 x, u8 y, u8 player);

u8 PlayerAtSquare(u8 x, u8 y);

u8 SizeAtSquare(u8 x, u8 y);

void DrawFullGrid();

void DrawGameGrid();

void UpdateCursor();

void HideCursor();

void LoadIdleAnim(u8 current, u8 animCounter);

void LoadGrowAnim(u8 current, u8 animCounter);

void RestAnims();

void DrawGameGrid2();


inline void IncrementSquare2(GridSquare* square, u8 player);
#endif