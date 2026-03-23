#ifndef _TYPES_H_
#define _TYPES_H_


#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef NULL
#define NULL    0
#endif


typedef signed char s8;

typedef short s16;

typedef long s32;

typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned long u32;


typedef struct
{
	s16 X;
	s16 Y;
} Point;


extern u16 RndPump;
void setRandomSeed(u16 seed);
u16 random();
u16 random_int(u16 min, u16 max);

void WaitForSFX();

#endif