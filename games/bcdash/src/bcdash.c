
/*
 *  B.C. Dash
 *  Copyright (C) 2009 Paul McPhee
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/****************************************
 *			Library Dependencies		*
 ****************************************/
#include <avr/pgmspace.h>
#include <string.h>
#include <uzebox.h>
#include <stdint.h>
#include "platz.h"

/****************************************
 *				Constants				*
 ****************************************/

#define LOW_FLASH

// Player
#define BCDASH_EEPROM_ID				14
#define SPRITE_BOB						0
#define SPD_MAX_BOB_GND_X				1
#define SPD_MAX_BOB_GND_Y				2
#define SPD_MAX_LYNX_GND_X				12
#define SPD_MAX_LYNX_GND_Y				12
#define SPD_MAX_DRAGONFLY_X				12
#define SPD_MAX_DRAGONFLY_Y				8
#define SPD_MAX_TURTLE_X				12
#define SPD_MAX_TURTLE_Y				4
#define SPD_MAX_SNAIL_X					2
#define SPD_MAX_SNAIL_Y					12
#define LOC_BOB_X						120
#define LOC_BOB_Y						40
	// States
#define BS_MOVING						1
#define BS_SWING						2
#define BS_COCONUT						4
#define BS_THROW						8

	// Terrestrial jumping
#define TM_YSTATE_IDLE					1
#define TM_YSTATE_TAKEOFF				2
#define TM_YSTATE_RISING				4
#define TM_YSTATE_PEAK					8
#define TM_YSTATE_FALLING				16
	// Forms
#define FORM_LYNX						1
#define FORM_DRAGONFLY					2
#define FORM_TURTLE						4
#define FORM_SNAIL						8

// Movement
#define MOVE_NONE						0
#define MOVE_LEFT						BTN_LEFT
#define MOVE_RIGHT						BTN_RIGHT
#define MOVE_DOWN						BTN_DOWN
#define MOVE_UP							BTN_UP
#define MOVE_JUMP						BTN_A

// SFX
#define SFX_CHAN						2

#define SFX_TEST						0 //20
#define SFX_WATERFALL					(SFX_TEST+11)
#define SFX_BOMB						(SFX_TEST+12)
#define SFX_COLLECT_PUP					(SFX_TEST+13)
#define SFX_COLLECT_COIN				(SFX_TEST+14)
#define SFX_COLLECT_PSN					(SFX_TEST+15)
#define SFX_QUIET						(SFX_TEST+16)

#define SFX_VOL_WATERFALL				0x40
#define SFX_VOL_BOMB					0xFF
#define SFX_VOL_COLLECT_PUP				0xFF //0xA6
#define SFX_VOL_COLLECT_COIN			0xFF //0xA6
#define SFX_VOL_COLLECT_PSN				0xFF //0xA6


// TILES
#define TILE_SKY						0
#define TILE_WATER						1
#define TILE_STAR						2
#define TILE_TIME						3
#define TILE_PUPSEL						4
#define TILE_GRASS_LT					11
#define TILE_GRASS_RT					13
#define TILE_ICICLE						21
#define TILE_ICE						22
#define TILE_TIME_BEST					23
#define TILE_EGG						28
#define TILE_EGG_WATER					29
#define TILE_FONT_START					82
#define TILE_GND_WATER_TOP_LT			104
#define TILE_GND_WATER_TOP_RT			106
#define TILE_GND_BTM_LT					19
#define TILE_GND_BTM_RT					20


// Invisible triggers
#define TRIG_CHECK_0					1
#define TRIG_CHECK_1					2
#define TRIG_CHECK_2					4
#define TRIG_CHECK_3					8
#define TRIG_WATER						9
#define TRIG_WFALL						10

// Mutable bg ids
#define MUT_COIN						0
#define MUT_POISON						1
#define MUT_BOMB						2
#define MUT_BOOTS						3
#define MUT_BUDDHA						4
#define MUT_POGO						5
#define MUT_EGG							6
#define MUT_HOURGLASS					7
#define MUT_BOMB_TARGET					8
#define MUT_FREEZE						9
#define MUT_WLK_WATER					10
#define MUT_SECRET						11
#define MUT_SECRET_WATER				12
#define MUT_FIRE						13
#define MUT_BOMB_TARGET_SWITCH			14

#define MUT_BG_COUNT					32	// 32x8 bits for 256 bgs total

// HUD
#define HUD_ORIGIN_Y					VRAM_TILES_V
#define HUD_CURR_TIME_X					1
#define HUD_CURR_TIME_Y					(HUD_ORIGIN_Y+2)
#define HUD_BEST_TIME_X					(HUD_CURR_TIME_X)
#define HUD_BEST_TIME_Y					(HUD_ORIGIN_Y+1)
#define HUD_PUPS_X						7
#define HUD_PUPS_Y						(HUD_ORIGIN_Y+1)

// Pups
#define PUP_EGG							0
#define PUP_BOOTS						1
#define PUP_BUDDHA						2
#define PUP_COINS						3
#define PUP_DFLY						3

// Poisons

#define PSN_SNAIL						0
#define PSN_REV_CTLS					1
#define PSN_FREEZE						2
#define PSN_COIN_TAX					3

// Misc
#define HZ								64	// For pup/poison counters. Better efficient than accurate.
#define GSTATE_INTRO					1
#define GSTATE_PLAYING					2
#define GSTATE_PAUSED					4

/****************************************
 *				Utils					*
 ****************************************/
// General
#define MIN(_x,_y)  ((_x)<(_y) ? (_x) : (_y))
#define MAX(_x,_y)  ((_x)>(_y) ? (_x) : (_y))
#define ABS(_x)		(((_x) > 0) ? (_x) : -(_x))

#ifdef LOW_FLASH
	#define TRIGGER_NOTE(patch,note,vol,len)	TriggerNote(SFX_CHAN,(patch),(note),(vol));	\
												sfxtmr = (len);
#else
	#define TRIGGER_NOTE(patch,note,vol,len)	TriggerFx(patch,vol,true);
#endif

/****************************************
 *			Type declarations			*
 ****************************************/
typedef struct bcTime {
	char sec1;
	char sec10;
	char min;
} bcTime;

typedef struct decCount {
	char ones;
	char tens;
} decCount;

typedef struct dsdt {
	u8 ds;
	u8 count;
	u8 limit;
} dsdt;

typedef struct accel {		// Single axis acceleration
	u8 		state;			// Movement state (jumping etc)
	u8 		max;
	dsdt 	acc;			// Acceleration
	dsdt 	dec;			// Deceleration
} accel;

typedef struct player {
	platzActor	pa;
	u8			sprite;
	u8			form;
	u8			state;
	u8			animFlag;
	accel		ax;
	accel		ay;
	animation 	anim;
	void (*moveFunc)(u16);
} player;

/****************************************
 *			Data Dependencies			*
 ****************************************/
// Tiles
#include "data/bcdash/tiles.pic.inc"
#include "data/bcdash/tiles.map.inc"
// Sprites
#include "data/bcdash/sprites.pic.inc"
#include "data/bcdash/sprites.map.inc"
// Animations
#include "data/bcdash/animations.map.inc"
// Sound
#include "data/bcdash/patches.inc"
#include "data/bcdash/crazy_caroms.inc"
// Level data
#include "data/bcdash/platz.levels.inc"

/****************************************
 *			File-level variables		*
 ****************************************/

player bob;
u16 move = MOVE_NONE; 		// Bob's current movement instruction
u8 hangtime = 10;
u8 wfall;
u8 wfalltmr = 15;
u8 fps;
u8 checkpoints;
u8 mutmap[MUT_BG_COUNT];
bcTime tCurr,tBest;
// Power-ups
u8 pupsel;
u8 puphud[4];
u16 puptmrs[4];
u8 walkOnWater;
u8 hrgtmr;
// Explosion flash
u8 expltmr;
rect explRect;
// Poisons
u8 prng;
u16 psntmrs[3];	// Poison/pup timers
// Pogo stick
u16 pogotmr;
// High level game state
u8 gstate = GSTATE_INTRO;
// Sfx
u8 sfxtmr;

/****************************************
 *			Function prototypes			*
 ****************************************/
void ActivateTrigger(u16 index, u8 type, char trig);	// Platz event callback target
u8 BgMutator(u8 evType, bgInner *bgiSrc, bgInner *bgiInfo, void *v);	// Platz mutable bg event callback target
void AerialMovement(u16 move);
void AerialAnimation(char dir, char prevDir);
void TerrestrialMovement(u16 move);
void TerrestrialAnimation(char prevDir, u16 move, u8 collMask);
void AquaticMovement(u16 move);
void AquaticAnimation(char prevDir, u16 move);
char SetForm(u8 form, char dirX);
void LoadBobAnimations(char dirX);
//void BobFormatEeprom(u8 quick);
//u16 BobLoadGame(void);
//void BobSaveGame(u8 sp);
void ResetGame(void);
void BCFormatEeprom(void);
void SaveBestTime(void);
void LoadBestTime(void);
void InitHud(void);
void AdjustTime(char sec);
void PrintHudTime(void);
void PrintHudBestTime(void);
void PrintHudPupCount(u8 x, u8 y, u8 count, char indicator);
void UpdateHudPups(void);
void UpdateTimers(void);

/****************************************
 *			Function definitions		*
 ****************************************/

u8 BgMutator(u8 evType, bgInner *bgiSrc, bgInner *bgiInfo, void *v) {
	u8 mapIndex = (bgiInfo->type)>>3,mutIndex = (bgiInfo->type)&7,mutVal,clr;
	rect *r;

	mutVal = mutmap[mapIndex]&(1<<mutIndex);

	if (evType == PLATZ_MUT_EV_DRAW) {
		if (mutVal) {
			if (bgiInfo->tile == MUT_BOMB_TARGET_SWITCH)	// Switch off linked mutable bg whilst we are active
				mutmap[bgiInfo->r.left] &=~ (1<<bgiInfo->r.right);
			return 1;
		}
	} else if (evType == PLATZ_MUT_EV_ANIM) {
		if (mutVal) {
			if (bgiInfo->tile == MUT_FREEZE)
				if (psntmrs[PSN_FREEZE])
					return 0;
			return 1;
		}
	} else { // PLATZ_MUT_EV_COLLISION
		if (mutVal) {
			// Set to most commonly used
			clr = TILE_SKY;
			r = &bgiSrc->r;

			switch (bgiInfo->tile) {
				case MUT_COIN:
					TRIGGER_NOTE(SFX_COLLECT_COIN,83,SFX_VOL_COLLECT_COIN,20);
					++puphud[PUP_COINS];
					break;
				case MUT_POISON:
					TRIGGER_NOTE(SFX_COLLECT_PSN,50,SFX_VOL_COLLECT_PSN,10);

					if ((const char*)pgm_read_word(&(pgmMaps[bgiSrc->tile])) == mapPsn)
						clr = TILE_SKY;
					else
						clr = TILE_WATER;
					// Randomize poison effect (LFSR seeded with rendered frames count when start was pressed at title/pause screen)
					switch (prng&3) {
						case PSN_FREEZE:
							psntmrs[PSN_FREEZE] = 4*HZ;
							break;
						case PSN_SNAIL:	// Snail will get most traffic
							if ((psntmrs[PSN_SNAIL]>>6) < 96)	// We don't have room for 3 digits
								psntmrs[PSN_SNAIL] += 4*HZ;
							break;
						case PSN_REV_CTLS:
							psntmrs[PSN_REV_CTLS] = 4*HZ;
							break;
						case PSN_COIN_TAX:
							if (puphud[PUP_COINS])
								--puphud[PUP_COINS];
							break;
					}

					prng = (prng>>1) | (((((prng^(prng>>2))^(prng>>3))^(prng>>4)))<<7);
					break;
				case MUT_BOMB:
					TRIGGER_NOTE(SFX_BOMB,0,SFX_VOL_BOMB,33);
					mutmap[mapIndex+((mutIndex+1)>>3)] &=~ (1<<((mutIndex+1)&7));	// Destroy target bg
										
					if (expltmr)
						PlatzFill(&explRect,TILE_SKY);	// Clear old one in case it hasn't expired
					explRect = bgiSrc->r;
					++explRect.right;
					--explRect.top;
					PlatzFillMap(&explRect,0,0,mapExplosion,2);
					expltmr = HZ>>2;
					clr = TILE_SKY;
					r = &bgiInfo->r;
					// Switch on linked mutable bg
					bgOuter *bgo = (bgOuter*)v;
					// Bombs and target switches are contiguous mutable inner bgs (that's where the 3 comes from)
					memcpy_P(bgiSrc,pgmBgsInner+bgo->index+3,sizeof(bgInner));
					mutmap[bgiSrc->r.left] |= (1<<bgiSrc->r.right);
					break;
				case MUT_BOMB_TARGET_SWITCH:
				case MUT_BOMB_TARGET:
					return 1;
				case MUT_BOOTS:
					if ((const char*)pgm_read_word(&(pgmMaps[bgiSrc->tile])) == mapBoots)
						clr = TILE_SKY;
					else
						clr = TILE_WATER;
					++puphud[PUP_BOOTS];
					break;
				case MUT_BUDDHA:
					++puphud[PUP_BUDDHA];
					break;
				case MUT_POGO:
					pogotmr = 6*HZ;
					break;
				case MUT_EGG:
					if (bgiSrc->tile == TILE_EGG)
						clr = TILE_SKY;
					else
						clr = TILE_WATER;
					++puphud[PUP_EGG];
					break;
				case MUT_HOURGLASS:
					hrgtmr += 5;
					break;
				case MUT_WLK_WATER:
					r = 0;

					if ((bob.form != FORM_TURTLE) && puptmrs[PUP_BUDDHA]) {
						// Must test with tile-coordinates. Pixel coordinates will always fail.
						if (!PlatzRectsIntersect(&bgiInfo->r,&(rect){(bob.pa.loc.x-bob.pa.bbx)>>3,
								(bob.pa.loc.x+bob.pa.bbx)>>3,(bob.pa.loc.y-bob.pa.bby)>>3,(bob.pa.loc.y+bob.pa.bby)>>3})) {
							walkOnWater = 1;
							return 1;
						}
					}
					break;
				case MUT_SECRET_WATER:
					clr = TILE_WATER;
					break;
				case MUT_FIRE:
					if (hrgtmr == 0)
						fps+=10;	// Time penalty for touching the fire
					r = 0;
					break;
			}

			switch (bgiInfo->tile) {
				case MUT_BOOTS:
				case MUT_BUDDHA:
				case MUT_POGO:
				case MUT_EGG:
				case MUT_HOURGLASS:
					TRIGGER_NOTE(SFX_COLLECT_PUP,60,SFX_VOL_COLLECT_PUP,10);
					break;
			}
			
			if (r) {
				mutmap[mapIndex] &=~ mutVal;
				PlatzFill(r,clr);
			}
		}
	}
	return 0;
}


void ResetGame(void) {
	checkpoints = 0;
	memset(psntmrs,0x00,3);
	memset(puphud,0x00,4);

	// Make sure active power-ups run their clean up code
	for (u8 i = 0; i < 4; i++)
		if (puptmrs[i])
			puptmrs[i] = 1;
	UpdateTimers();
	memset(mutmap,0xff,MUT_BG_COUNT);
	sfxtmr = 0;
	TriggerNote(2,SFX_QUIET,60,5);
	wfall = 0;
	wfalltmr = 15;
	TriggerNote(3,SFX_QUIET,60,5);
}


void BCFormatEeprom(void) {
	// Set sig. so we don't format next time
	WriteEeprom(0, (u8)EEPROM_SIGNATURE);
	WriteEeprom(1, (u8)(EEPROM_SIGNATURE>>8));

	// Paint free blocks
	for (int i = 2; i < 64; i++) {
		WriteEeprom(i*EEPROM_BLOCK_SIZE,(u8)EEPROM_FREE_BLOCK);
		WriteEeprom(i*EEPROM_BLOCK_SIZE+1,(u8)(EEPROM_FREE_BLOCK>>8));
	}
}


void SaveBestTime(void) {
	struct EepromBlockStruct ebs;

	if (!isEepromFormatted()) {
		BCFormatEeprom();
	}
	
	ebs.id = BCDASH_EEPROM_ID;
	ebs.data[0] = tBest.min;
	ebs.data[1] = tBest.sec10;
	ebs.data[2] = tBest.sec1;
	EepromWriteBlock(&ebs);
}


void LoadBestTime(void) {
	struct EepromBlockStruct ebs;

	ebs.id = BCDASH_EEPROM_ID;

	if (EepromReadBlock(BCDASH_EEPROM_ID, &ebs) == 0) {
		tBest.min = ebs.data[0];
		tBest.sec10 = ebs.data[1];
		tBest.sec1 = ebs.data[2];
	} else {
		tBest.min = 9;
		tBest.sec10 = 9;
		tBest.sec1 = 9;
	}
}


void InitHud(void) {
	SetTile(HUD_CURR_TIME_X,HUD_CURR_TIME_Y,TILE_TIME);
	SetTile(HUD_BEST_TIME_X,HUD_BEST_TIME_Y,TILE_TIME_BEST);
	SetTile(HUD_CURR_TIME_X+3,HUD_CURR_TIME_Y,TILE_FONT_START+10);
	SetTile(HUD_BEST_TIME_X+3,HUD_BEST_TIME_Y,TILE_FONT_START+10);
	SetTile(HUD_BEST_TIME_X+2,HUD_BEST_TIME_Y,TILE_FONT_START+tBest.min);
	SetTile(HUD_BEST_TIME_X+4,HUD_BEST_TIME_Y,TILE_FONT_START+tBest.sec10);
	SetTile(HUD_BEST_TIME_X+5,HUD_BEST_TIME_Y,TILE_FONT_START+tBest.sec1);
	// Powerups
	DrawMap2(HUD_PUPS_X+1,HUD_PUPS_Y,mapPsn);
	SetTile(HUD_PUPS_X+6,HUD_PUPS_Y,TILE_EGG);
	DrawMap2(HUD_PUPS_X+5,HUD_PUPS_Y+1,mapBoots);
	DrawMap2(HUD_PUPS_X+10,HUD_PUPS_Y,mapBuddha);
	DrawMap2(HUD_PUPS_X+15,HUD_PUPS_Y,animCoins);
}


void AdjustTime(char sec) {
	tCurr.sec1 += sec;
	sec = (sec > 0)?1:-1;	// Normalize and re-use

	while ((tCurr.sec1 >= 10) || (tCurr.sec1 < 0)) {
		tCurr.sec1 -= 10*sec;
		tCurr.sec10 += sec;
	}

	while ((tCurr.sec10 >= 6) || (tCurr.sec10 < 0)) {
		tCurr.sec10 -= 6*sec;
		tCurr.min += sec;

		if (tCurr.min > 9)
			tCurr.min = 9;
		else if (tCurr.min < 0)
			tCurr.min = 0;
	}
}


void PrintHudTime(void) {
	SetTile(HUD_CURR_TIME_X+2,HUD_CURR_TIME_Y,TILE_FONT_START+tCurr.min);
	SetTile(HUD_CURR_TIME_X+4,HUD_CURR_TIME_Y,TILE_FONT_START+tCurr.sec10);
	SetTile(HUD_CURR_TIME_X+5,HUD_CURR_TIME_Y,TILE_FONT_START+tCurr.sec1);
}


void PrintHudBestTime(void) {
	SetTile(HUD_BEST_TIME_X+2,HUD_BEST_TIME_Y,TILE_FONT_START+tBest.min);
	SetTile(HUD_BEST_TIME_X+4,HUD_BEST_TIME_Y,TILE_FONT_START+tBest.sec10);
	SetTile(HUD_BEST_TIME_X+5,HUD_BEST_TIME_Y,TILE_FONT_START+tBest.sec1);
}


void PrintHudPupCount(u8 x, u8 y, u8 count, char indicator) {
	u8 bcdl,bcdh;

	count = MIN(count,99);
	bcdl = count&0x0f;
	bcdh = (count>>4)&0x0f;
	bcdl += (bcdh*6);

	// Rarely runs and never more than thrice with binary input <= 99
	while (bcdl > 9) {
		bcdl -= 10;
		++bcdh;
	}

	// Leading "X"
	if (indicator >= 0) {
		if (indicator != pupsel)
			SetTile(x++,y,TILE_FONT_START+11);
		else
			SetTile(x++,y,TILE_PUPSEL);
	}
	
	if (bcdh)
		SetTile(x++,y,TILE_FONT_START+bcdh);
	else
		SetTile(x+1,y,TILE_SKY);
	SetTile(x,y,TILE_FONT_START+bcdl);
}


void UpdateHudPups(void) {
	PrintHudPupCount(HUD_PUPS_X+7,HUD_PUPS_Y,puphud[PUP_EGG],PUP_EGG);
	PrintHudPupCount(HUD_PUPS_X+7,HUD_PUPS_Y+1,puphud[PUP_BOOTS],PUP_BOOTS);
	PrintHudPupCount(HUD_PUPS_X+12,HUD_PUPS_Y+1,puphud[PUP_BUDDHA],PUP_BUDDHA);
	PrintHudPupCount(HUD_PUPS_X+17,HUD_PUPS_Y+1,puphud[PUP_COINS],PUP_COINS);
	PrintHudPupCount(HUD_PUPS_X+3,HUD_PUPS_Y+1,(u8)((psntmrs[PSN_SNAIL]+psntmrs[PSN_REV_CTLS]+psntmrs[PSN_FREEZE])>>6),-1);
}


void UpdateTimers(void) {
	// Hide bomb explosion
	if (expltmr)
		if (--expltmr == 0)
			PlatzFill(&explRect,TILE_SKY);

	// Pogo stick
	if (pogotmr) {
		if (--pogotmr == 0) {
			if (bob.form == FORM_LYNX)
				bob.ay.dec.ds = 16;
			hangtime = 10;
		} else if (bob.form == FORM_LYNX) {
			bob.ay.dec.ds = 32;
			hangtime = 16;
		}
	}

	// Boots - let player retain high speed until making an error (i.e. don't wind back current speed)
	if (puptmrs[PUP_BOOTS])
		if (--puptmrs[PUP_BOOTS] == 0)
			bob.ax.max >>= 1;

	if (psntmrs[PSN_FREEZE]) {	// Freeze poison
		--psntmrs[PSN_FREEZE];
	} else if (psntmrs[PSN_REV_CTLS]) {	// Wonky controls
		--psntmrs[PSN_REV_CTLS];
	} else if (psntmrs[PSN_SNAIL]) {
			if ((bob.form == FORM_LYNX) && (bob.ay.state == TM_YSTATE_IDLE))
				SetForm(FORM_SNAIL,bob.pa.vx.dir);
			if (bob.form == FORM_SNAIL) {
				if (psntmrs[PSN_SNAIL])
					--psntmrs[PSN_SNAIL];
			}
	} else if (bob.form == FORM_SNAIL) {
		if (SetForm(FORM_LYNX,bob.pa.vx.dir))
			psntmrs[PSN_SNAIL] = HZ>>2;	// Failed - try again later
	}

	// Egg/flying
	if (puptmrs[PUP_EGG]) {
		if (--puptmrs[PUP_EGG] == 0)
			if (bob.form == FORM_DRAGONFLY)
				SetForm(FORM_LYNX,bob.pa.vx.dir);
	} else if (puptmrs[PUP_DFLY]) {
		if (--puptmrs[PUP_DFLY] == 0) {
			if (bob.form != FORM_TURTLE)
				SetForm(FORM_SNAIL,bob.pa.vx.dir);
		}
	} else if (bob.form == FORM_DRAGONFLY)
		SetForm(FORM_LYNX,bob.pa.vx.dir);

	// Walk on water
	if (puptmrs[PUP_BUDDHA]) {
		if (--puptmrs[PUP_BUDDHA] == 0)
			walkOnWater = 0;
	}
}


void LoadBobAnimations(char dirX) {
	u8 index = 0;

	if (dirX > 0) index += 1;

	switch (bob.form) {
		case FORM_LYNX:
			if ((bob.state&BS_MOVING) || bob.pa.vx.vel) {
				// Move past lynx idle animation
				index += 2;
			}
			break;
		case FORM_DRAGONFLY:
			index += 4;
			break;
		case FORM_TURTLE:
			index += 6;
			break;
		case FORM_SNAIL:
			index += 8;
			break;
	}
		
	memcpy_P(&bob.anim,bobAnimations+index,sizeof(animation));
}


void ActivateTrigger(u16 index, u8 type, char trig) {
	switch(index) {
		case TRIG_CHECK_0:
			if (checkpoints == 0) {
				tCurr.sec1 = 0;
				tCurr.sec10 = 0;
				tCurr.min = 0;
				fps = 0;
				PrintHudTime();
			}	// Fall through
		case TRIG_CHECK_1:
		case TRIG_CHECK_2:
		case TRIG_CHECK_3:
			// Ensure player hits checkpoints in succession
			if ((checkpoints&(index-1)) == (index-1))
				checkpoints |= index;
			break;
		case TRIG_WATER:				
			if (trig > 0) {
				if (bob.form == FORM_TURTLE) {
					SetForm(FORM_LYNX,bob.pa.vx.dir);
					bob.ay.state = TM_YSTATE_TAKEOFF;

					if (move&MOVE_JUMP)
						pogotmr = HZ>>2;
				}
			} else {
				if ((bob.form != FORM_TURTLE) && ((bob.ay.state&(TM_YSTATE_TAKEOFF|TM_YSTATE_RISING|TM_YSTATE_PEAK)) == 0) && (walkOnWater == 0)) {
					SetForm(FORM_TURTLE,bob.pa.vx.dir);
				}					
			}
			break;
		case TRIG_WFALL:
			if (trig > 0) {
				// Turn off and reset timer
				wfall = 0;
				wfalltmr = 15;
				TriggerNote(3,SFX_QUIET,60,5);
			} else {
				if (wfall == 0) {
					wfall = 1;
					wfalltmr = 15;
				}
			}
			break;
	}
}


char SetForm(u8 form, char dirX) {
	u8 snailTurtleVelY, retVal = 0;

	if (bob.form == form) {
		return retVal;
	}

	u8 prevForm = bob.form;
	PlatzHideSprite(bob.sprite,bob.anim.wid,bob.anim.hgt);
	bob.form = form;
	LoadBobAnimations(dirX);
	
	// Don't call PlatzSetBoundingBoxDimensions before bob has been initialised
	if (!prevForm || PlatzSetBoundingBoxDimensions(&bob.pa,bob.anim.wid<<3,bob.anim.hgt<<3)) {
		bob.ay.state = TM_YSTATE_IDLE;

		switch (form) {		
			case FORM_DRAGONFLY:
				PlatzSetVelocity(&bob.pa.vy,MIN(bob.pa.vy.vel,SPD_MAX_DRAGONFLY_Y),&bob.pa.trLoc.y);
				bob.moveFunc = AerialMovement;
				bob.ax.acc.ds = 2;
				bob.ax.dec.ds = 2;
				bob.ax.acc.count = 0;
				bob.ax.dec.count = 0;
				bob.ax.acc.limit = 8;
				bob.ax.dec.limit = 16;
				bob.ax.max = SPD_MAX_DRAGONFLY_X;
				bob.ay.acc.ds = 2;
				bob.ay.dec.ds = 1;
				bob.ay.acc.count = 0;
				bob.ay.dec.count = 0;
				bob.ay.acc.limit = 6;
				bob.ay.dec.limit = 10;
				bob.ay.max = SPD_MAX_DRAGONFLY_Y;
				break;
			case FORM_TURTLE:
			case FORM_SNAIL:
				if (form == FORM_SNAIL) {
					bob.ax.max = SPD_MAX_SNAIL_X;
					bob.ay.max = SPD_MAX_SNAIL_Y;
					bob.ax.dec.ds = 8;
					snailTurtleVelY = SPD_MAX_SNAIL_Y;
				} else {
					bob.ax.max = SPD_MAX_TURTLE_X;
					bob.ay.max = SPD_MAX_TURTLE_Y;
					bob.ax.dec.ds = 2;
					snailTurtleVelY = SPD_MAX_TURTLE_Y;
				}

				PlatzSetVelocity(&bob.pa.vy,MIN(bob.pa.vy.vel,snailTurtleVelY),&bob.pa.trLoc.y);
				bob.moveFunc = AquaticMovement;
				bob.ax.acc.ds = 8;
				bob.ax.acc.count = 0;
				bob.ax.dec.count = 0;
				bob.ax.acc.limit = 0;
				bob.ax.dec.limit = 12;
				bob.ay.acc.ds = 1;
				bob.ay.dec.ds = 1;
				bob.ay.acc.count = 0;
				bob.ay.dec.count = 0;
				bob.ay.acc.limit = 16;
				bob.ay.dec.limit = 32;
				break;
			case FORM_LYNX:
			default:
				bob.moveFunc = TerrestrialMovement;
				bob.ax.acc.ds = 2;
				bob.ax.dec.ds = 2;
				bob.ax.acc.count = 0;
				bob.ax.dec.count = 0;
				bob.ax.acc.limit = 8;
				bob.ax.dec.limit = 5;
				bob.ax.max = SPD_MAX_LYNX_GND_X;
				bob.ay.acc.ds = 2;
				bob.ay.dec.ds = 16;
				bob.ay.acc.count = 0;
				bob.ay.dec.count = 0;
				bob.ay.acc.limit = 2;
				bob.ay.dec.limit = 2;
				bob.ay.max = SPD_MAX_LYNX_GND_Y;
				break;
		}

		if (puptmrs[PUP_BOOTS])
			bob.ax.max <<= 1;
	} else {
		bob.form = prevForm;
		retVal = 1;
	}

	LoadBobAnimations(dirX);
	PlatzMapSprite(bob.sprite, bob.anim.wid, bob.anim.hgt, bob.anim.frames+bob.anim.currFrame*bob.anim.size);
	return retVal;
}


void AerialMovement(u16 move) {
	char xVel = bob.pa.vx.vel, yVel = bob.pa.vy.vel;

	// X axis - Apply wind resistance
	if ((((move&(MOVE_LEFT|MOVE_RIGHT)) == 0) && xVel) || ((move&MOVE_LEFT) && (xVel > 0)) || ((move&MOVE_RIGHT) && (xVel < 0))) {			
		if (++(bob.ax.dec.count) >= bob.ax.dec.limit) {
			bob.ax.dec.count = 0;
			xVel += (xVel > 0)?MAX(-xVel,-bob.ax.dec.ds):MIN(-xVel,bob.ax.dec.ds);
		}						
	} else {
		bob.ax.dec.count = 0;
	}

	// Apply player input
	if (move&MOVE_LEFT) {
		if ((xVel > -bob.ax.max) && (++bob.ax.acc.count >= bob.ax.acc.limit)) {
			bob.ax.acc.count = 0;
			xVel -= bob.ax.acc.ds;
		}
	} else if (move&MOVE_RIGHT) {
		if ((xVel < bob.ax.max) && (++bob.ax.acc.count >= bob.ax.acc.limit)) {
			bob.ax.acc.count = 0;
			xVel += bob.ax.acc.ds;
		}
	}

	// Y axis - Apply gravity
	if ((yVel < bob.ay.max) && (++bob.ay.dec.count >= bob.ay.dec.limit)) {
		bob.ay.dec.count = 0;
		yVel += bob.ay.dec.ds;			
	}

	if (move&MOVE_UP) {
		if ((yVel > -bob.ay.max) && (++bob.ay.acc.count >= bob.ay.acc.limit)) {
			bob.ay.acc.count = 0;
			yVel -= bob.ay.acc.ds;
		}
	} else if (move&MOVE_DOWN) {
		if ((yVel < bob.ay.max) && (++bob.ay.acc.count >= bob.ay.acc.limit)) {
			bob.ay.acc.count = 0;
			yVel += bob.ay.acc.ds;
		}
	}

	PlatzSetVelocity(&bob.pa.vx,xVel,&bob.pa.trLoc.x);
	PlatzSetVelocity(&bob.pa.vy,yVel,&bob.pa.trLoc.y);
}


void AerialAnimation(char dir, char prevDir) {
	// Check for direction change
	if (dir != prevDir) {
		bob.animFlag |= 3;
	}

	if (++bob.anim.disp >= bob.anim.dpf) {
		// Bob has moved far enough to warrant a new frame
		bob.anim.disp = 0;
		bob.anim.currFrame++;

		if (bob.anim.currFrame == bob.anim.frameCount) {
			bob.anim.currFrame = 0;
		}

		bob.animFlag |= 0x02;
	}
}


void TerrestrialMovement(u16 move) {
	static u8 jumpPeakCount = 8;
	char xVel = bob.pa.vx.vel, yVel = bob.pa.vy.vel,xDir = 0;

	// X axis
	if (yVel == 0 && (bob.ay.state == TM_YSTATE_IDLE)) {	// Don't let lynx decelerate x axis while airborne
		// Apply ground friction (Note: could use a trigger to disable this for an oil-slick surface)
		if ((((move&(MOVE_LEFT|MOVE_RIGHT)) == 0) && xVel) || ((move&MOVE_LEFT) && (xVel > 0)) || ((move&MOVE_RIGHT) && (xVel < 0))) {
			if (++(bob.ax.dec.count) >= bob.ax.dec.limit) {
				bob.ax.dec.count = 0;
				xVel += (xVel > 0)?MAX(-xVel,-bob.ax.dec.ds):MIN(-xVel,bob.ax.dec.ds);
			}
		} else {
			bob.ax.dec.count = 0;
		}
	}

	// Apply player input
	if ((move&MOVE_LEFT) && (xVel > -bob.ax.max))
		xDir = -1;
	else if ((move&MOVE_RIGHT) && (xVel < bob.ax.max))
		xDir = 1;

	if (xDir && (++bob.ax.acc.count >= bob.ax.acc.limit)) {
		bob.ax.acc.count = 0;

		if (yVel == 0 && (bob.ay.state == TM_YSTATE_IDLE))
			xVel += bob.ax.acc.ds*xDir;
		else if (xVel == 0)
			xVel += bob.ax.acc.ds*xDir;
	}

	// Y axis - Apply player input
	if ((yVel == 0) && (bob.ay.state == TM_YSTATE_IDLE)) {
		if (move&MOVE_JUMP) {
			bob.ay.state = TM_YSTATE_TAKEOFF;
		}
	}

	// Jumping
	switch (bob.ay.state) {
		case TM_YSTATE_TAKEOFF:
			bob.ay.acc.count = 0;	// Ensure all jumps are of equal height
			yVel = -bob.ay.dec.ds;
#ifdef LOW_FLASH
			bob.anim.currFrame = 0;
#else
			bob.anim.currFrame = 1;
#endif
			bob.state |= BS_MOVING;
			bob.animFlag |= 0x03;
			bob.ay.state = TM_YSTATE_RISING;
			break;
		case TM_YSTATE_RISING:
			if (yVel == 0) {
#ifdef LOW_FLASH
				bob.anim.currFrame = 1;
#else
				bob.anim.currFrame = 2;
#endif
				bob.animFlag |= 0x02;
				jumpPeakCount = hangtime;
				bob.ay.state = TM_YSTATE_PEAK;
			}
			break;
		case TM_YSTATE_PEAK:
			if (--jumpPeakCount == 0) {
#ifdef LOW_FLASH
				bob.anim.currFrame = 2;
#else
				bob.anim.currFrame = 3
#endif
				bob.animFlag |= 0x02;
				bob.ay.state = TM_YSTATE_FALLING;
			}
			break;
		case TM_YSTATE_FALLING:	// Ground collision finalises the jump
		case TM_YSTATE_IDLE:
		default:
			// Do nothing
			break;
	}

	// Apply gravity (excluding jump peak)
	if ((bob.ay.state != TM_YSTATE_PEAK) && (yVel < bob.ay.max) && (++bob.ay.acc.count >= bob.ay.acc.limit)) {
		bob.ay.acc.count = 0;
		yVel += bob.ay.acc.ds;
	}

	PlatzSetVelocity(&bob.pa.vx,xVel,&bob.pa.trLoc.x);
	PlatzSetVelocity(&bob.pa.vy,yVel,&bob.pa.trLoc.y);
}


void TerrestrialAnimation(char prevDir, u16 move, u8 collMask) {
	char yVel = GET_VEL(bob.pa.vy);

	// Check for direction change
	if (bob.pa.vx.dir != prevDir) {
		bob.animFlag |= 3;
	}
	
	if (collMask&4) {
		// Y-Axis collision detected
		if ((bob.pa.vx.vel == 0) && (bob.ay.state != TM_YSTATE_IDLE)) {
			bob.animFlag |= 3;
		}
		bob.ay.state = TM_YSTATE_IDLE;	// End of jump/fall
	} else if ((collMask&8) && (bob.ay.state != TM_YSTATE_IDLE)) {
		bob.ay.state = TM_YSTATE_FALLING;

	}

	if ((yVel > 0) && (bob.ay.state == TM_YSTATE_IDLE)) {
		if ((bob.state&BS_MOVING) == 0) {
			bob.state |= BS_MOVING;
		}
		bob.ay.state = TM_YSTATE_FALLING;
		bob.animFlag |= 3;
	}

	// Animations
	if (((move&(MOVE_LEFT|MOVE_RIGHT)) == 0) && ((bob.pa.vx.vel | bob.pa.vy.vel) == 0) && (bob.ay.state == TM_YSTATE_IDLE) && (bob.state&BS_MOVING)) {
		bob.state&=~BS_MOVING;
		bob.animFlag |= 0x03;
	} else if (((bob.state&BS_MOVING) == 0) && bob.pa.vx.vel){
		bob.state |= BS_MOVING;
		bob.animFlag |= 0x03;
	}

	if ((yVel == 0) && (bob.ay.state == TM_YSTATE_IDLE)) {
		bob.anim.disp++;

		if (bob.anim.disp >= bob.anim.dpf) {
			// Bob has moved far enough to warrant a new frame
			bob.anim.disp = 0;
			bob.anim.currFrame++;

			if (bob.anim.currFrame == bob.anim.frameCount) {
				bob.anim.currFrame = 0;
			}

			bob.animFlag |= 0x02;
		}
	} else if (bob.pa.vy.vel && (bob.ay.state == TM_YSTATE_FALLING)) {
		// Special frame for falling
#ifdef LOW_FLASH
		bob.anim.currFrame = 2;
#else
		bob.anim.currFrame = 3;
#endif
		bob.animFlag |= 0x02;
	}
}

void AquaticMovement(u16 move) {
	char xVel = bob.pa.vx.vel, yVel = bob.pa.vy.vel;

	// X axis
	if (xVel) {
		// Apply friction
		if (++bob.ax.dec.count >= bob.ax.dec.limit) {
			bob.ax.dec.count = 0;
			xVel += (xVel > 0)?MAX(-xVel,-bob.ax.dec.ds):MIN(-xVel,bob.ax.dec.ds);
		}
	} else if (move&MOVE_LEFT) {
		xVel = -bob.ax.acc.ds;
	} else if (move&MOVE_RIGHT) {
		xVel = bob.ax.acc.ds;
	}

	// Y axis - Apply buoyancy
	if (bob.form == FORM_TURTLE) {
		if ((yVel >= 0) && (++bob.ay.dec.count >= bob.ay.dec.limit)) {
			bob.ay.dec.count = 0;			
			yVel -= bob.ay.dec.ds;
		}

		if (move&MOVE_UP) {
			if ((yVel > -bob.ay.max) && (++bob.ay.acc.count >= bob.ay.acc.limit)) {
				bob.ay.acc.count = 0;
				yVel -= bob.ay.acc.ds;
			}
		} else if (move&MOVE_DOWN) {
			if ((yVel < bob.ay.max) && (++bob.ay.acc.count >= bob.ay.acc.limit)) {
				bob.ay.acc.count = 0;
				yVel += bob.ay.acc.ds;
			}
		}
	}

	if (bob.form == FORM_SNAIL) {
		if (xVel > bob.ax.max) {
			xVel = bob.ax.max;
		} else if (xVel < -bob.ax.max) {
			xVel = -bob.ax.max;
		}
		if (yVel < SPD_MAX_SNAIL_Y)
			yVel += 2;
	}

	PlatzSetVelocity(&bob.pa.vx,xVel,&bob.pa.trLoc.x);
	PlatzSetVelocity(&bob.pa.vy,yVel,&bob.pa.trLoc.y);
}


void AquaticAnimation(char prevDir, u16 move) {
	// Check for direction change
	if (bob.pa.vx.dir != prevDir) {
		bob.animFlag |= 3;
	}

	if (bob.form == FORM_TURTLE) {
		if (bob.pa.vx.vel == 0) {
			if (move&(MOVE_UP|MOVE_DOWN)) {
				bob.anim.disp++;

				if (bob.anim.disp >= bob.anim.dpf) {
					bob.anim.disp = 0;
					bob.anim.currFrame = (bob.anim.currFrame == 1) ? 2 : 1;
					bob.animFlag |= 0x02;
				}
			} else if (bob.anim.currFrame != 0) {
				bob.anim.currFrame = 0;
				bob.animFlag |= 0x02;
			}
		} else if (ABS(bob.pa.vx.vel) <= 4) {
			if (bob.anim.currFrame != 1) {
				bob.anim.currFrame = 1;
				bob.animFlag |= 0x02;
			}
		} else {
			if (bob.anim.currFrame != 2) {
				bob.anim.currFrame = 2;
				bob.animFlag |= 0x02;
			}
		}
	} else {
		bob.anim.currFrame = 0;
	}
}


int main(void) {
	u8 besttmr = 0;
	u16 fmove = MOVE_NONE;		// Final movement instruction after poisons etc
	u8 collFlag = 0;			// Collision flag
	char prevDir = 0;
	u16 btnPrev = 0;			// Previous buttons that were held
	u16 btnHeld = 0;    		// Buttons that are held right now
	u16 btnPressed = 0;  		// Buttons that were pressed this frame
	u16 btnReleased = 0;		// Buttons that were released this frame

	// Init kernel
	LoadBestTime();
	InitMusicPlayer(patches);
	SetMasterVolume(0xb0);
	SetTileTable(tileset);
	SetSpritesTileTable(spriteset);
	SetSpriteVisibility(true);
	Screen.overlayHeight = OVERLAY_LINES;

	// Init platz pointers
	PlatzSetTriggerCallback(ActivateTrigger);
	PlatzSetMutCallback(BgMutator);
	PlatzSetMovingPlatformTable(pgmPlatforms);
	PlatzSetMovingPlatformDirectory(pgmPlatformDir);
	PlatzSetMapsTable(pgmMaps);
	PlatzSetAnimatedBgTable(bgAnimations);
	PlatzSetAnimatedBgDirectory(pgmAnimDir);
	PlatzSetObjectTable(pgmObjects);
	PlatzSetInnerBgTable(pgmBgsInner);
	PlatzSetOuterBgTable(pgmBgsOuter);
	PlatzSetBgDirectory(pgmBgDir);

	// Local inits
	bob.sprite = SPRITE_BOB;
	bob.state = 0;
	SetForm(FORM_DRAGONFLY,DIR_RIGHT);
	memset(mutmap,0xff,MUT_BG_COUNT);
	
	// Init platz actor
	bob.pa.loc = (pt){170,48};
	bob.pa.bbx = bob.anim.wid<<2;	// Set to half of animation's wid in pixels
	bob.pa.bby = bob.anim.hgt<<2;	// Set to half of animation's hgt in pixels
	bob.pa.sprx = 160;				// Just going to default to Platz initializing our viewport for the title screen
	bob.pa.trLoc = (pt){0,0};		// Trigger loc can start at origin as Lynx is our largest player sprite
	bob.pa.vx.dir = DIR_RIGHT;
	bob.pa.vy.dir = DIR_DOWN;
	PlatzSetVelocity(&bob.pa.vx,0,&bob.pa.trLoc.x);
	PlatzSetVelocity(&bob.pa.vy,0,&bob.pa.trLoc.y);

	// Draw snail on title screen
	PlatzMapSprite(4,2,2,animSnailRt);
	MoveSprite(4,48,40,2,2);

	// Init platz scene
	PlatzSetMovingPlatformTiles(199,177,176);
	PlatzInit(&bob.pa,57);
	PlatzMoveToSlice(&bob.pa,57);
	
	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();
			btnHeld = ReadJoypad(0);
        	btnPressed = btnHeld&(btnHeld^btnPrev);
        	btnReleased = btnPrev&(btnHeld^btnPrev);
			btnPrev = btnHeld;

			// Sound effects
			if (besttmr) {
				if (besttmr == 0) {
					TriggerNote(2,SFX_QUIET,60,5);
				} else if ((besttmr&3) == 0)
					TRIGGER_NOTE(SFX_COLLECT_COIN,100-besttmr,SFX_VOL_COLLECT_COIN,4);	// Deliberately left macro half in/half out of conditional
				--besttmr;
			}

			if (wfall) {
				if (--wfalltmr == 0) {
					wfalltmr = 15;
					TriggerNote(3,SFX_WATERFALL,85,SFX_VOL_WATERFALL);
				}
			}

			if (sfxtmr) {
				if (--sfxtmr == 0)
					TriggerNote(2,SFX_QUIET,60,5);
			}

			// Check for finish line crossing
			if (checkpoints == 0x0F) {
				TRIGGER_NOTE(SFX_COLLECT_COIN,60,SFX_VOL_COLLECT_COIN,20);
				checkpoints = 0;
				AdjustTime((psntmrs[PSN_SNAIL]+psntmrs[PSN_REV_CTLS]+psntmrs[PSN_FREEZE])>>6);
				AdjustTime(-puphud[PUP_COINS]);
				PrintHudTime();

				if ((tCurr.min < tBest.min) || ((tCurr.min == tBest.min) && (tCurr.sec10 < tBest.sec10)) || 
						((tCurr.min == tBest.min) && (tCurr.sec10 == tBest.sec10) && (tCurr.sec1 < tBest.sec1))) {
					besttmr = 40;
					tBest = tCurr;
					PrintHudBestTime();
					SaveBestTime();
				}
				ResetGame();
			}

			// Begin game and pause/unpause
			if (btnPressed&BTN_START) {
				if (gstate == GSTATE_INTRO) {
					PlatzHideSprite(4,2,2);
					bob.pa.loc = (pt){LOC_BOB_X,LOC_BOB_Y};
					bob.pa.sprx = (PLATZ_SCRN_WID>>1)-bob.pa.bbx;
					PlatzSetViewport(bob.pa.sprx,0);
					PlatzMoveToSlice(&bob.pa,56);
					InitHud();
					PrintHudTime();
					prng = MAX(prng,1);		// Don't seed lfsr with zero
					StartSong(crazy_caroms_song);
					gstate = GSTATE_PLAYING;
				} else if (gstate == GSTATE_PAUSED) {
					gstate = GSTATE_PLAYING;
				} else if (gstate == GSTATE_PLAYING) {
					gstate = GSTATE_PAUSED;
				}
			}

			// Reset and move to start
			if (btnPressed&BTN_SELECT) {
				if (gstate != GSTATE_INTRO) {
					ResetGame();
					bob.pa.loc = (pt){LOC_BOB_X,LOC_BOB_Y};
					PlatzMoveToSlice(&bob.pa,56);
					SetForm(FORM_LYNX,DIR_RIGHT);
					PlatzSetVelocity(&bob.pa.vx,0,&bob.pa.trLoc.x);
					bob.pa.vx.dir = 1;
					gstate = GSTATE_PLAYING;
				}
			}

			if (gstate == GSTATE_INTRO)
				++prng;
			else if (gstate == GSTATE_PAUSED)
				continue;

			// Scroll through pups
			if (btnPressed&BTN_SR) {
				pupsel = (pupsel+1)&3;
			} else if (btnPressed&BTN_SL) {
				pupsel = (pupsel-1)&3;
			}

			if (btnPressed&BTN_B) {
				// Activate selected power-up
				switch (pupsel) {
					case PUP_EGG:
						if (puphud[PUP_EGG] && (bob.form == FORM_LYNX)) {
							--puphud[PUP_EGG];
							puptmrs[PUP_EGG] = 4*HZ;

							if (bob.form == FORM_LYNX)
								SetForm(FORM_DRAGONFLY,bob.pa.vx.dir);
						}
						break;
					case PUP_BOOTS:
						if (puphud[PUP_BOOTS]) {
							--puphud[PUP_BOOTS];
							if (puptmrs[PUP_BOOTS] == 0)
								bob.ax.max <<= 1;
							puptmrs[PUP_BOOTS] = 5*HZ;
						}
						break;
					case PUP_BUDDHA:
						if ((bob.form != FORM_TURTLE) && puphud[PUP_BUDDHA]) {
							--puphud[PUP_BUDDHA];
							puptmrs[PUP_BUDDHA] = 6*HZ;
						}
						break;
					case PUP_COINS:
						if (puphud[PUP_COINS] >= 5) {
							puphud[PUP_COINS] -= 5;
							++puphud[PUP_BOOTS];
							++puphud[PUP_EGG];
							++puphud[PUP_BUDDHA];
						}
						break;
				}
			}

			if (psntmrs[PSN_FREEZE]) {
				// Enforce freeze
				fmove = move = 0;
				PlatzSetVelocity(&bob.pa.vx,0,&bob.pa.trLoc.x);
				PlatzSetVelocity(&bob.pa.vy,0,&bob.pa.trLoc.y);
			} else {
				move &=~ btnReleased;
				move |= btnPressed|btnHeld;
				fmove = move;

				if (psntmrs[PSN_REV_CTLS]) {
					// Left = right, up = down
					fmove &= ~0x00F0;
					fmove |= ((move&BTN_RIGHT)>>1)|((move&BTN_LEFT)<<1)|((move&BTN_DOWN)>>1)|((move&BTN_UP)<<1);
				}
					
				bob.animFlag = 0;			
				prevDir = bob.pa.vx.dir;

				if (gstate == GSTATE_PLAYING)
					bob.moveFunc(fmove);
			}

			collFlag = PlatzMove(&bob.pa);

			switch(bob.form) {
				case FORM_DRAGONFLY:
					if (psntmrs[PSN_FREEZE] == 0)
						AerialAnimation(bob.pa.vx.dir,prevDir);
					break;
				case FORM_LYNX:
					TerrestrialAnimation(prevDir,fmove,collFlag); break;
				case FORM_TURTLE:
				case FORM_SNAIL:
					AquaticAnimation(prevDir,fmove); break;
			}
			
			
			if (bob.animFlag&1) LoadBobAnimations(bob.pa.vx.dir);
			if (bob.animFlag&2) PlatzMapSprite(bob.sprite,bob.anim.wid,bob.anim.hgt,bob.anim.frames+bob.anim.currFrame*bob.anim.size);
			MoveSprite(bob.sprite,bob.pa.sprx-bob.pa.bbx,bob.pa.loc.y-bob.pa.bby+1,bob.anim.wid,bob.anim.hgt);
			
			if (gstate == GSTATE_PLAYING) {
				UpdateTimers();
				UpdateHudPups();

				if (checkpoints) {
					if (++fps >= 60) {	// Can be incremented elsewhere by environmental triggers
						fps = 0;

						if (hrgtmr)
							--hrgtmr;
						else
							AdjustTime(1);
						PrintHudTime();
					}
				}
			}
			PlatzTick();
		}
	}
}



