
/*
 *  Space Invaders (for Uzebox)
 *  Copyright (C) 2010 Paul McPhee
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

#include <uzebox.h>
#include <avr/pgmspace.h>
#include <string.h>


/****************************************
 *			  	 Data					*
 ****************************************/
// Tiles
#include "data/tiles.pic.inc"
#include "data/tiles.map.inc"
// Sprites
#include "data/sprites.pic.inc"
#include "data/sprites.map.inc"

/****************************************
 *			  	 Defines				*
 ****************************************/
#define HZ							60
#define SCRN_HGT					(SCREEN_TILES_H * TILE_WIDTH)
#define SCRN_WID					(SCREEN_TILES_V * TILE_HEIGHT)
#define TILE_WID2					(TILE_WIDTH / 2)
#define TILE_HGT2					(TILE_HEIGHT / 2)
#define HIT_QUEUE_SIZE				4

#define BOUNDARY_LEFT				(4 * TILE_WIDTH)
#define BOUNDARY_RIGHT				(26 * TILE_WIDTH)
#define BOUNDARY_TOP				(6 * TILE_HEIGHT)
#define BOUNDARY_BTM				(23 * TILE_HEIGHT)

#define PLAYER_START_LOC_X 			((SCRN_HGT>>1) - ((animShip_Width*TILE_WIDTH)>>1))
#define PLAYER_START_LOC_Y			(22 * TILE_HEIGHT)
#define PLAYER_START_LIVES			3
#define PLAYER_LIVES_LOC_X			26
#define PLAYER_LIVES_LOC_Y			20
#define PLAYER_SPEED_X				1
#define PLAYER_PROJECTILE_SPD		4

#define MONSTERS_PER_ROW			8
#define MONSTER_ROW_COUNT			3
#define MONSTER_COUNT				(MONSTERS_PER_ROW * MONSTER_ROW_COUNT)
#define MONSTER_SPACING_X			2
#define MONSTER_SPACING_Y			2
#define MONSTER_HIT_DURATION		(HZ>>2)

#define INVADERS_MIN_LOC_X			5
#define INVADERS_MAX_LOC_X			(INVADERS_MIN_LOC_X + 5)
#define INVADERS_MIN_LOC_Y			7
#define INVADERS_MAX_LOC_Y			21
#define INVADERS_HEIGHT				5
#define INVADERS_DISPLAY_STATE_COUNT 4
#define INVADERS_PROJECTILE_SPD		2
#define INVADERS_MOVE_INTERVAL		HZ
#define INVADERS_PROJ_INTERVAL		(2*HZ)

#define SHELTER_LOC_X				6
#define SHELTER_LOC_Y				19
#define SHELTER_COUNT				4
#define SHELTER_LIMIT				4
#define SHELTER_SPACING_X			5

#define PLAYING_FIELD_LOC_X			4
#define PLAYING_FIELD_LOC_Y			6
#define PLAYING_FIELD_WID			22
#define PLAYING_FIELD_HGT			16

#define SPRITE_PLAYER_SHIP			0
#define SPRITE_PLAYER_PROJ			2
#define SPRITE_INVADERS_PROJ		3
#define SPRITE_UFO					4

#define ANIM_INDEX_PLAYER_SHIP		0
#define ANIM_INDEX_PLAYER_DEAD		1
#define ANIM_INDEX_PLAYER_SHOOT		2
#define ANIM_INDEX_INVADERS_SHOOT	3

#define PRESS_START_LOC_X			9
#define PRESS_START_LOC_Y			13
#define GAME_OVER_LOC_X				10
#define GAME_OVER_LOC_Y				13

#define ONE_UP_LOC_X				4
#define ONE_UP_LOC_Y				5
#define HISCORE_LOC_X				16
#define HISCORE_LOC_Y				5

#define CLEAR_TILE					0
#define LIVES_CLEAR_TILE			3
#define SCORE_DIGIT_COUNT			7
#define SPACE_INVADERS_ID			12
#define DIGIT_OFFSET				67	// Index of first tile of the sequentially-stored numerals in flash

#define DIR_RIGHT					1
#define DIR_LEFT					-1

#define MIN(x,y)  	((x)<(y) ? (x) : (y))
#define MAX(x,y)  	((x)>(y) ? (x) : (y))
#define ABS(x)		(((x) > 0) ? (x) : -(x))

#define GET_MONSTER_WID()	((ai.dispState == single || ai.dispState == dblVert) ? 1: 2)
#define GET_MONSTER_HGT()	((ai.dispState == single || ai.dispState == dblHoriz) ? 1: 2)

/****************************************
 *			Type declarations			*
 ****************************************/
typedef enum { 
	title, playing, playerDead, paused, gameOver
} GameState;

typedef enum {
	alive, dead
} PlayerState;

typedef enum {
	mAlive, mHit, mDead
} MonsterState;

typedef enum {
	idle, active
} ProjectileState;

typedef enum {
	single, dblHoriz, dblVert, quad
} InvaderDisplayState;

typedef enum {
	green, pink, yellow
} MonsterType;

typedef struct {
	u8	x;
	u8	y;
} pt;

typedef struct {
	u8 left;
	u8 right;
	u8 top;
	u8 btm;
} rect;

typedef struct {
	u8 isValid;
	rect r;
} hitQueue;

typedef struct {
	u8			size;			// wid*hgt
	u8			wid;			// Width of each frame (in tiles)
	u8			hgt;			// Height of each frame (in tiles)
	u8			frameCount;		// # of frames in an animation cycle
	u8 			currFrame;		// The frame that is currently displayed
	u8			disp;			// Displacement counter
	u8			dpf;			// Displacement per frame (scales frame rate to movement speed)
	u8			synch;			// Flag to indicate animation should be synchronized with others of similar type
	const char	*frames;		// Stored in flash
} animation;

typedef struct {
	ProjectileState state;
	pt loc;
	char vel;
	u8 sprite;
	u8 animIndex;
	animation anim;
} projectile;

typedef struct {
	pt loc;
	u8 dmg[mapShelter_Width * mapShelter_Height];
} shelter;

typedef struct {
	PlayerState state;
	pt loc;
	u8 spd;
	u8 lives;
	u8 sprite;
	u32 score;
	projectile prj;
	animation anim;
} player;

typedef struct {
	u8 hitTimer;
	MonsterState state;
	MonsterType type;
} monster;

typedef struct {
	InvaderDisplayState dispState;
	pt tileLoc;								// Top left monster
	pt absLoc;
	char dir;
	u8 moveTimer;
	u8 maxMoveTimer;
	u8 prjTimer;
	u8 maxPrjTimer;
	u8 monstersRemaining;
	projectile prj;
	u8 attackers[MONSTERS_PER_ROW];
	monster monsters[MONSTER_COUNT];
} invaders;

/****************************************
 *			File-level variables		*
 ****************************************/
const animation animations[] PROGMEM = {
	/* Player */
	{2,2,1,1,0,0,0,0,animShip},
	{2,2,1,2,0,0,4,0,animShipDestroyed},
	{1,1,1,1,0,0,0,0,animPlayerShoot},
	/* Enemy */
	{1,1,1,3,0,0,8,0,animInvaderShoot}
};

GameState gameState;
player p1;
invaders ai;
shelter shelters[SHELTER_COUNT];
hitQueue hitQ[4];
u8 hqIndex;
u32 hiScore;
u32 freeLife;
u8 prng;

const char *monsterMaps[] PROGMEM = { 	mapMonsterGreen0, mapMonsterGreen1, mapMonsterGreen2, mapMonsterGreen3,
										mapMonsterPink0, mapMonsterPink1, mapMonsterPink2, mapMonsterPink3,
										mapMonsterYellow0, mapMonsterYellow1, mapMonsterYellow2, mapMonsterYellow3
};

const char *monsterDeadMaps[] PROGMEM = { mapMonsterDead0, mapMonsterDead1, mapMonsterDead2, mapMonsterDead3 };

const char *shelterMaps[] PROGMEM = { mapShelterDmg0, mapShelterDmg1, mapShelterDmg2, mapShelterDmg3 };

/****************************************
 *			Function declarations		*
 ****************************************/
void LoadHighScore(void);
void SaveHighScore(void);
void FlashPressStart(void);
void InitRound(u8 round);
void PrintPlayerScore(void);
void PrintHiScore(void);
void PrintScores(void);
void PrintNumbers(pt *pts, u8 count, u32 val, u8 digits);
void FillRegion(u8 x, u8 y, u8 width, u8 height, u8 tile);
void ClearMonster(u8 x, u8 y, u8 width, u8 height, u8 clrTile);
void ClearMonsters(void);
void MoveMonsters(void);
void SetInvaderDisplayState(void);
void DrawMonsters(void);
void DrawShelters(void);
void DrawLives(void);
void MoveProjectiles(u8 count, projectile **projectiles);
void HideSprite(u8 spriteIndex, u8 wid, u8 hgt);
u8 PointInRect(const pt *p, const rect *r);
u8 RectsIntersect(const rect *r1, const rect *r2);
void ProcessShelterCollision(projectile *prj);
void ProcessCollisions(void);
void PlayerMove(u16 move);
void PlayerAttack(u16 cmd);
void MonstersAttack(void);
pt GetMonsterLoc(u8 index);
pt GetMonsterLocAbs(u8 index);
void AnimateProjectiles(u8 count, projectile **projectiles);
void DrawProjectiles(u8 count, projectile **projectiles);
void AnimatePlayer(void);
void DrawPlayer(void);
void SetPlayerState(PlayerState state);
void SetMonsterState(u8 index, MonsterState state);
void SetProjectileState(projectile *p, ProjectileState state, char vel, const pt *loc);
void AdjustScore(int val);
u8 EnqueueHitMonster(const rect *r);
rect* DequeueHitMonster(void);
void UpdateAttackers(void);
void SpaceInvadersMapSprite(u8 index, u8 wid, u8 hgt, const char *map, u8 spriteFlags);

/****************************************
 *			Function definitions		*
 ****************************************/
void LoadHighScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = SPACE_INVADERS_ID;

	if (!isEepromFormatted() || EepromReadBlock(SPACE_INVADERS_ID, &ebs))
		return;
	hiScore = 0;

	for (u8 i = 0; i < 4; i++)
		hiScore |= ((u32)(ebs.data[i]))<<(24-(i<<3));
}


void SaveHighScore(void) {
	struct EepromBlockStruct ebs;

	ebs.id = SPACE_INVADERS_ID;

	if (!isEepromFormatted())
		return;
	for (u8 i = 0; i < 4; i++)
		ebs.data[i] = hiScore>>(24-i*8);
	EepromWriteBlock(&ebs);
}


void FlashPressStart(void) {
	static char flashState = 1;

	if (flashState == 1)
		DrawMap2(PRESS_START_LOC_X, PRESS_START_LOC_Y, mapPressStart);
	else
		FillRegion(PRESS_START_LOC_X, PRESS_START_LOC_Y, mapPressStart_Width, mapPressStart_Height, CLEAR_TILE);
	flashState *= -1;
}


void InitRound(u8 round) {
	for (u8 i = 0; i < HIT_QUEUE_SIZE; i++)
		hitQ[i].isValid = 0;
	ai.dispState = single;
	ai.tileLoc = (pt){ INVADERS_MIN_LOC_X, INVADERS_MIN_LOC_Y };
	ai.absLoc = (pt){ ai.tileLoc.x<<3, ai.tileLoc.y<<3 };
	ai.dir = DIR_RIGHT;
	ai.moveTimer = ai.maxMoveTimer = MAX(INVADERS_MOVE_INTERVAL-(round<<2), INVADERS_MOVE_INTERVAL>>4);
	ai.prjTimer = ai.maxPrjTimer = INVADERS_PROJ_INTERVAL-(round<<2);
	ai.monstersRemaining = MONSTER_COUNT;
	SetProjectileState(&ai.prj, idle, 0, 0);
	ai.prj.sprite = SPRITE_INVADERS_PROJ;
	ai.prj.animIndex = ANIM_INDEX_INVADERS_SHOOT;
	memcpy_P(&ai.prj.anim, animations + ANIM_INDEX_INVADERS_SHOOT, sizeof(ai.prj.anim));

	// Init to front-line monsters
	for (u8 i = 0; i < MONSTERS_PER_ROW; i++)
		ai.attackers[i] = i + MONSTER_COUNT - MONSTERS_PER_ROW;

	for (u8 i = 0; i < MONSTER_COUNT; i++) {
		SetMonsterState(i, mAlive);
		ai.monsters[i].type = i / MONSTERS_PER_ROW;
	}

	for (u8 i = 0; i < SHELTER_COUNT; i++) {
		shelters[i].loc = (pt){SHELTER_LOC_X + i * SHELTER_SPACING_X, SHELTER_LOC_Y};

		for (u8 j = 0; j < (mapShelter_Width * mapShelter_Height); j++) {
			if (j != 4)
				shelters[i].dmg[j] = 0;
			else
				shelters[i].dmg[j] = SHELTER_LIMIT;
		}
	}

	if (round == 0) {
		p1.score = 0;
		p1.lives = PLAYER_START_LIVES;
		freeLife = 25000;
	}
	SetPlayerState(alive);
	p1.loc.x = PLAYER_START_LOC_X;
	p1.loc.y = PLAYER_START_LOC_Y;
	p1.spd = PLAYER_SPEED_X;
	p1.sprite = SPRITE_PLAYER_SHIP;
	p1.prj.sprite = SPRITE_PLAYER_PROJ;
	p1.prj.animIndex = ANIM_INDEX_PLAYER_SHOOT;
	SetProjectileState(&p1.prj, idle, 0, 0);

	// Clear playing field
	DrawMap2(0, 0, mapBackground);

	// Populate playing field
	DrawMonsters();
	DrawShelters();
	DrawLives();
	DrawMap2(ONE_UP_LOC_X, ONE_UP_LOC_Y, map1Up);
	DrawMap2(HISCORE_LOC_X, HISCORE_LOC_Y, mapHi);
	PrintPlayerScore();
	PrintHiScore();
	gameState = playing;
}


void FillRegion(u8 x, u8 y, u8 width, u8 height, u8 tile) {
	for (u8 i = y; i < (y + height); i++) {
		for (u8 j = x; j < (x + width); j++) {
			SetTile(j, i, tile);
		}
	}
}


void ClearMonster(u8 x, u8 y, u8 width, u8 height, u8 clrTile) {
	for (u8 i = y; i < (y + height); i++) {
		for (u8 j = x; j < (x + width); j++)
			SetTile(j, i, clrTile);
	}
}

void ClearMonsters(void) {
	for (u8 i = 0; i < MONSTER_ROW_COUNT; i++) {
		for (u8 j = 0; j < MONSTERS_PER_ROW; j++) {
			if (ai.monsters[i * MONSTERS_PER_ROW + j].state == mAlive)
				ClearMonster(ai.tileLoc.x + j * MONSTER_SPACING_X, ai.tileLoc.y + i * MONSTER_SPACING_Y, 
					GET_MONSTER_WID(), GET_MONSTER_HGT(), CLEAR_TILE);
		}
	}
}

void MoveMonsters(void) {
	if (--ai.moveTimer)
		return;
	ai.moveTimer = ai.maxMoveTimer;
	ai.absLoc.x += TILE_WID2 * ai.dir;

	if ((ai.dir == DIR_RIGHT && (ai.absLoc.x&7) == 0) || (ai.dir == DIR_LEFT && (ai.absLoc.x&7)))
		ai.tileLoc.x += ai.dir;
	if ((ai.dir == DIR_RIGHT && ai.absLoc.x > (INVADERS_MAX_LOC_X<<3)) || (ai.dir == DIR_LEFT && ai.absLoc.x < (INVADERS_MIN_LOC_X<<3))) {
		ai.dir *= -1;
		ai.absLoc.y += TILE_HGT2;
		
		if ((ai.absLoc.y&7) == 0)
			++ai.tileLoc.y;
	}
	SetInvaderDisplayState();
}


void SetInvaderDisplayState(void) {
	if (ai.absLoc.x&7) {
		if (ai.absLoc.y&7)
			ai.dispState = quad;
		else
			ai.dispState = dblHoriz;
	} else {
		if (ai.absLoc.y&7)
			ai.dispState = dblVert;
		else
			ai.dispState = single;
	}
}


void DrawMonsters(void) {
	MonsterType type;
	MonsterState monState;

	for (u8 i = 0; i < MONSTER_ROW_COUNT; i++) {
		for (u8 j = 0; j < MONSTERS_PER_ROW; j++) {
			monState = ai.monsters[i * MONSTERS_PER_ROW + j].state;
			type = ai.monsters[i * MONSTERS_PER_ROW + j].type;

			if (monState == mAlive) {
				DrawMap2(ai.tileLoc.x + j * MONSTER_SPACING_X, ai.tileLoc.y + i * MONSTER_SPACING_Y, 
					(const char*)pgm_read_word(monsterMaps + type * INVADERS_DISPLAY_STATE_COUNT + ai.dispState));
			} else if (monState == mHit) {
				if (--ai.monsters[i * MONSTERS_PER_ROW + j].hitTimer == 0)
					SetMonsterState(i * MONSTERS_PER_ROW + j, mDead);
			}
		}
	}
}


void DrawShelters(void) {
	for (u8 i = 0; i < SHELTER_COUNT; i++)
		DrawMap2(shelters[i].loc.x, shelters[i].loc.y, mapShelter);
}


void DrawLives(void) {
	if (p1.lives == 0)
		return;
	for (u8 i = 0; i < (p1.lives - 1); i++)
		DrawMap2(PLAYER_LIVES_LOC_X, PLAYER_LIVES_LOC_Y - i, mapLives);
	FillRegion(PLAYER_LIVES_LOC_X, PLAYER_LIVES_LOC_Y - (p1.lives - 1), mapLives_Width, mapLives_Height, LIVES_CLEAR_TILE);
}


void PrintPlayerScore(void) {
	PrintNumbers(&(pt){ONE_UP_LOC_X + map1Up_Width, ONE_UP_LOC_Y}, 1, p1.score, SCORE_DIGIT_COUNT);
}


void PrintHiScore(void) {
	PrintNumbers(&(pt){HISCORE_LOC_X + mapHi_Width, HISCORE_LOC_Y}, 1, hiScore, SCORE_DIGIT_COUNT);
}


void PrintScores(void) {
	pt pts[2] = { {ONE_UP_LOC_X + map1Up_Width, ONE_UP_LOC_Y}, {HISCORE_LOC_X + mapHi_Width, HISCORE_LOC_Y} };
	PrintNumbers(pts, 2, hiScore, SCORE_DIGIT_COUNT);
}


void PrintNumbers(pt *pts, u8 count, u32 val, u8 digits) {
	u8 numeral;

	for (u8 i = 0; i < digits; i++, val /= 10) {
		numeral = val % 10;

		for (u8 j = 0; j < count; j++)
			SetTile(pts[j].x + digits - i, pts[j].y, DIGIT_OFFSET + numeral);
	}
}


void MoveProjectiles(u8 count, projectile **projectiles) {
	for (u8 i = 0; i < count; i++) {
		if (projectiles[i]->state == active)
			projectiles[i]->loc.y += projectiles[i]->vel;
	}
}


// Moves a sprite array's position offscreen so that it is not drawn
void HideSprite(u8 spriteIndex, u8 wid, u8 hgt) {
	u8 size = wid*hgt;

	for (int i = 0; i < size; i++)
		MoveSprite(spriteIndex+i,SCREEN_TILES_H<<3,0,1,1);
}


u8 PointInRect(const pt *p, const rect *r) {
	return p->x >= r->left && p->x < r->right && p->y >= r->top && p->y < r->btm;
}


u8 RectsIntersect(const rect *r1, const rect *r2) {
	if ((r1->btm < r2->top) || (r1->right < r2->left) || (r1->left > r2->right) || (r1->top > r2->btm))
		return 0;
	return 1;
}


u8 DestroyShelter(pt p, u8 index) {
	pt sLoc = shelters[index].loc;

	p.x >>= 3;
	p.y >>= 3;
	p.x -= sLoc.x;
	p.y -= sLoc.y;

	u8 *dmg = &shelters[index].dmg[p.x + p.y * mapShelter_Width];

	if (*dmg < SHELTER_LIMIT) {
		DrawMap2(sLoc.x + p.x, sLoc.y + p.y, (const char*)pgm_read_word(shelterMaps + *dmg));
		*dmg += 1;
		return 0;
	} else {
		return 1;
	}
}


void ProcessShelterCollision(projectile *prj) {
	if (prj->state != active)
		return;
	rect shelterRect = (rect){ 0, 0, SHELTER_LOC_Y<<3, (SHELTER_LOC_Y<<3) + (mapShelter_Height<<3) };

	for (u8 i = 0; i < SHELTER_COUNT; i++) {
		shelterRect.left = shelters[i].loc.x<<3;
		shelterRect.right = shelterRect.left + (mapShelter_Width<<3);
		
		if (PointInRect(&prj->loc, &shelterRect)) {
			if (!DestroyShelter(prj->loc, i)) {
				SetProjectileState(prj, idle, 0, 0);
				break;
			}
		}
	}	
}


void ProcessInvadersCollision(void) {
	if (p1.prj.state != active)
		return;
	u8 hit = 0;
	pt relativeLoc = { p1.prj.loc.x - ai.absLoc.x, p1.prj.loc.y - ai.absLoc.y };

	switch (ai.dispState) {
		case single:
			if (((relativeLoc.x&15) < 8) && ((relativeLoc.y&15) < 8))
				hit = 1;
			break;
		case dblHoriz:
			if ((((relativeLoc.x&15) > 3) || ((relativeLoc.x&15) < 12)) && ((relativeLoc.y&15) < 8))
				hit = 1;
			break;
		case dblVert:
			if (((relativeLoc.x&15) < 8) && (((relativeLoc.y&15) > 3) || ((relativeLoc.y&15) < 12)))
				hit = 1;
			break;
		case quad:
			if ((((relativeLoc.x&15) > 3) || ((relativeLoc.x&15) < 12)) && (((relativeLoc.y&15) > 3) || ((relativeLoc.y&15) < 12)))
				hit = 1;
			break;
	}

	if (hit) {
		u8 index = (relativeLoc.x / 16) + (relativeLoc.y / 16) * MONSTERS_PER_ROW;

		if (index < MONSTER_COUNT && ai.monsters[index].state == mAlive) {
			SetMonsterState(index, mHit);
			SetProjectileState(&p1.prj, idle, 0, 0);
		}
	}
}


void ProcessCollisions(void) {
	if (p1.state == dead)
		return;
	rect p1Rect = { p1.loc.x, p1.loc.x + (animShip_Width<<3), p1.loc.y, p1.loc.y + (animShip_Height<<3) };

	// Check for collision between invader projectile and player ship
	if (ai.prj.state == active) {
		if (PointInRect(&ai.prj.loc, &p1Rect)) {
			SetPlayerState(dead);
			return;
		}
	}

	// Check for collision between invader projectile and shelters
	ProcessShelterCollision(&ai.prj);
	
	// Check for collision between invader projectile and game boundary
	if (ai.prj.state == active) {
		if (ai.prj.loc.y >= BOUNDARY_BTM)
			SetProjectileState(&ai.prj, idle, 0, 0);
	}

	// Check for collision between player projectile and invader
	ProcessInvadersCollision();

	// Check for collision between player projectile and shelters
	ProcessShelterCollision(&p1.prj);

	// Check for collision between player projectile and game boundary
	if (p1.prj.state == active) {
		if (p1.prj.loc.y <= BOUNDARY_TOP)
			SetProjectileState(&p1.prj, idle, 0, 0);
	}
}


void PlayerMove(u16 move) {
	if (p1.state != alive)
		return;
	if (move & BTN_LEFT)
		p1.loc.x -= (p1.loc.x > BOUNDARY_LEFT) ? p1.spd : 0;
	else if (move & BTN_RIGHT)
		p1.loc.x += (p1.loc.x < (BOUNDARY_RIGHT - (animShip_Width<<3))) ? p1.spd : 0;
}


void PlayerAttack(u16 cmd) {
	if (p1.state != alive)
		return;
	if ((cmd & (BTN_A | BTN_B)) && p1.prj.state == idle)
		SetProjectileState(&p1.prj, active, -PLAYER_PROJECTILE_SPD, &(pt){p1.loc.x + TILE_WIDTH, p1.loc.y - TILE_HEIGHT});
}


void MonstersAttack(void) {
	if (ai.prj.state != idle)
		return;
	if (--ai.prjTimer == 0) {
		ai.prjTimer = ai.maxPrjTimer;
		ai.prj.loc = GetMonsterLocAbs(ai.attackers[prng&7]);	// Clamp prng to our attackers (8 MONSTERS_PER_ROW)
		ai.prj.loc.y += TILE_HEIGHT;
		SetProjectileState(&ai.prj, active, INVADERS_PROJECTILE_SPD, &ai.prj.loc);
	}
}


pt GetMonsterLoc(u8 index) {
	pt loc;

	loc.x = ai.tileLoc.x + (index&3) * MONSTER_SPACING_X;
	loc.y = ai.tileLoc.y + (index>>3) * MONSTER_SPACING_Y;
	return loc;
}


pt GetMonsterLocAbs(u8 index) {
	pt loc;

	loc.x = ai.absLoc.x + (index&7) * (MONSTER_SPACING_X<<3);
	loc.y = ai.absLoc.y + (index>>3) * (MONSTER_SPACING_Y<<3);
	return loc;
}


void AnimateProjectiles(u8 count, projectile **projectiles) {
	for (u8 i = 0; i < count; i++) {
		if (projectiles[i]->state == active) {
			if (++projectiles[i]->anim.disp >= projectiles[i]->anim.dpf) {
				projectiles[i]->anim.disp = 0;

				if (++projectiles[i]->anim.currFrame >= projectiles[i]->anim.frameCount)
					projectiles[i]->anim.currFrame = 0;
			}
		}
	}
}


void DrawProjectiles(u8 count, projectile **projectiles) {
	for (u8 i = 0; i < count; i++) {
		if (projectiles[i]->state == active) {
			SpaceInvadersMapSprite(projectiles[i]->sprite, projectiles[i]->anim.wid, projectiles[i]->anim.hgt, 
				projectiles[i]->anim.frames + projectiles[i]->anim.currFrame, 0);
			MoveSprite(projectiles[i]->sprite, projectiles[i]->loc.x, projectiles[i]->loc.y, 
				projectiles[i]->anim.wid, projectiles[i]->anim.hgt);
		}
	}
}


void AnimatePlayer(void) {
	if (++p1.anim.disp >= p1.anim.dpf) {
		p1.anim.disp = 0;

		if (++p1.anim.currFrame >= p1.anim.frameCount)
			p1.anim.currFrame = 0;
	}
}


void DrawPlayer(void) {
	SpaceInvadersMapSprite(p1.sprite, p1.anim.wid, p1.anim.hgt, p1.anim.frames + p1.anim.currFrame, 0);
	MoveSprite(p1.sprite, p1.loc.x, p1.loc.y, p1.anim.wid, p1.anim.hgt);
}


void SetPlayerState(PlayerState state) {
	switch (state) {
		case alive:
			memcpy_P(&p1.anim, animations + ANIM_INDEX_PLAYER_SHIP, sizeof(p1.anim));
			p1.state = alive;
			break;
		case dead:
			SetProjectileState(&p1.prj, idle, 0, 0);
			SetProjectileState(&ai.prj, idle, 0, 0);
			memcpy_P(&p1.anim, animations + ANIM_INDEX_PLAYER_DEAD, sizeof(p1.anim));
			--p1.lives;
			DrawLives();
			p1.state = dead;
			break;
	}
}


void SetMonsterState(u8 index, MonsterState state) {
	if (index > MONSTER_COUNT)
		return;
	monster *m = &ai.monsters[index];

	switch (state) {
		case mAlive:
			m->state = mAlive;
			break;
		case mHit:
		{
			rect r;

			DrawMap2(ai.tileLoc.x + (index&7) * MONSTER_SPACING_X, ai.tileLoc.y + (index>>3) * MONSTER_SPACING_Y,
					(const char*)pgm_read_word(monsterDeadMaps + ai.dispState));
			m->hitTimer = MONSTER_HIT_DURATION;			
			r.left = ai.tileLoc.x + (index&7) * MONSTER_SPACING_X;
			r.right = r.left + GET_MONSTER_WID();
			r.top = ai.tileLoc.y + (index>>3) * MONSTER_SPACING_Y;
			r.btm = r.top + GET_MONSTER_HGT();
			EnqueueHitMonster(&r);
			--ai.monstersRemaining;
			AdjustScore(100 + (yellow - m->type) * 100);
			m->state = mHit;
			break;
		}
		case mDead:
		{
			rect *r = DequeueHitMonster();

			if (r)
				FillRegion(r->left, r->top, r->right - r->left, r->btm - r->top, CLEAR_TILE);
			m->state = mDead;
			break;
		}
	}
}


void SetProjectileState(projectile *p, ProjectileState state, char vel, const pt *loc) {
	switch (state) {
		case idle:
			HideSprite(p->sprite, p->anim.wid, p->anim.hgt);
			p->state = idle;
			break;
		case active:
			if (loc)
				p->loc = *loc;
			p->vel = vel;
			memcpy_P(&p->anim, animations + p->animIndex, sizeof(p->anim));
			p->state = active;
			break;
	}
}


u8 EnqueueHitMonster(const rect *r) {
	if (!hitQ[hqIndex].isValid) {
		hitQ[hqIndex].isValid = 1;
		hitQ[hqIndex].r = *r;
		hqIndex = (hqIndex + 1)&3;
		return 1;
	}
	return 0;
}


void AdjustScore(int val) {
	p1.score += val;
	
	if (p1.score > hiScore) {
		hiScore = p1.score;
		PrintScores();
	} else {
		PrintPlayerScore();
	}

	if (p1.lives < 10) {
		if (p1.score > freeLife) {
			freeLife += 25000;
			++p1.lives;
			DrawLives();
		}
	}
}


rect* DequeueHitMonster(void) {
	u8 it = (hqIndex + 1)&3;

	for (u8 i = 0; i < HIT_QUEUE_SIZE; i++) {
		if (hitQ[it].isValid) {
			hitQ[it].isValid = 0;
			return &hitQ[it].r;
		}
		it = (it + 1)&3;
	}
	return 0;
}


void UpdateAttackers(void) {
	for (u8 i = 0; i < MONSTERS_PER_ROW; i++) {
		if (ai.monsters[ai.attackers[i]].state != mAlive) {
			if (ai.attackers[i] >= MONSTERS_PER_ROW)
				ai.attackers[i] -= MONSTERS_PER_ROW;
			else
				ai.attackers[i] = ai.attackers[(i+1)&3];
		}
	}
}


// Similar to kernel's MapSprite, but no wid/hgt header to read from flash
void SpaceInvadersMapSprite(u8 index, u8 wid, u8 hgt, const char *map, u8 spriteFlags) {
	u8 x,y,xStart,xEnd,xStep;

	if (spriteFlags&SPRITE_FLIP_X) {
		xStart = wid-1;
		xEnd = 255;
		xStep = -1;
	} else {
		xStart = 0;
		xEnd = wid;
		xStep = 1;
	}

	for (y = 0; y < hgt; y++) {
		for (x = xStart; x < xEnd; x += xStep,index++) {
			sprites[index].tileIndex = pgm_read_byte(&(map[(y*wid)+x]));
			sprites[index].flags = spriteFlags;
		}
	}
}


int main(void) {
	u16 btnPrev = 0;			// Previous buttons that were held
	u16 btnHeld = 0;    		// Buttons that are held right now
	u16 btnPressed = 0;  		// Buttons that were pressed this frame
	u16 btnReleased = 0;		// Buttons that were released this frame
	projectile *projectiles[] = { &p1.prj, &ai.prj };
	u8 round = 0, flashStartTimer = 0, playerDeadTimer = 0, gameOverTimer = 0;

	// Init
	gameState = title;
	hiScore = 0;
	LoadHighScore();
	SetTileTable(tileset);
	SetSpritesTileTable(spriteset);
	SetSpriteVisibility(true);
	ClearVram();
	DrawMap2(0, 0, mapBackground);
	flashStartTimer = 1;
	
	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			btnHeld = ReadJoypad(0);
			btnPressed = btnHeld&(btnHeld^btnPrev);
        	btnReleased = btnPrev&(btnHeld^btnPrev);
			btnPrev = btnHeld;

			if (btnPressed&BTN_SELECT) {
				if (p1.score >= hiScore)
					SaveHighScore();
				FillRegion(PLAYING_FIELD_LOC_X, PLAYING_FIELD_LOC_Y, PLAYING_FIELD_WID, PLAYING_FIELD_HGT-1, CLEAR_TILE);
				HideSprite(0, MAX_SPRITES, 1);
				gameState = title;
			}

			switch (gameState) {
				case title:						
					if (btnPressed&BTN_START) {
						InitRound(round = 0);
						gameState = playing;
					} else {
						if (--flashStartTimer == 0) {
							FlashPressStart();
							flashStartTimer = HZ>>1;
						}
					}
					++prng;
					prng = MAX(prng,1);
					break;
				case playing:
					prng = (prng>>1) | (((((prng^(prng>>2))^(prng>>3))^(prng>>4)))<<7);
					
					if (ai.monstersRemaining == 0)
						InitRound(++round);

					if (btnPressed&BTN_START) {
						gameState = paused;
						break;
					}

					MoveProjectiles(2, projectiles);
					ProcessCollisions();

					if ((ai.tileLoc.y + INVADERS_HEIGHT) >= INVADERS_MAX_LOC_Y)
						SetPlayerState(dead);

					if (p1.state == dead) {
						playerDeadTimer = 2*HZ;
						gameState = playerDead;
						break;
					}
					
					PlayerMove(btnHeld);
					PlayerAttack(btnPressed);
					ClearMonsters();
					MoveMonsters();
					MonstersAttack();
					AnimateProjectiles(2, projectiles);
					DrawProjectiles(2, projectiles);
					AnimatePlayer();
					DrawPlayer();
					DrawMonsters();
					UpdateAttackers();
					break;
				case playerDead:
					// Ignore pause game while dead
					if (--playerDeadTimer) {
						AnimatePlayer();
						DrawPlayer();
						break;
					}
					if (p1.lives == 0) {
						gameState = gameOver;
						break;
					} else if ((ai.tileLoc.y + INVADERS_HEIGHT) >= INVADERS_MAX_LOC_Y) {
						InitRound(++round);
					} else {
						SetPlayerState(alive);
						gameState = playing;
					}
					break;
				case paused:
					if (btnPressed&BTN_START)
						gameState = playing;
						break;
					break;
				case gameOver:
					if (gameOverTimer == 0) {
						if (p1.score >= hiScore)
							SaveHighScore();
						FillRegion(PLAYING_FIELD_LOC_X, PLAYING_FIELD_LOC_Y, PLAYING_FIELD_WID, PLAYING_FIELD_HGT-1, CLEAR_TILE);
						DrawMap2(GAME_OVER_LOC_X, GAME_OVER_LOC_Y, mapGameOver);
						HideSprite(0, MAX_SPRITES, 1);
						gameOverTimer = 3*HZ;
					} else if (--gameOverTimer == 0) {
						flashStartTimer = 1;
						gameState = title;
					}
					break;
			}
		}
	}
}


