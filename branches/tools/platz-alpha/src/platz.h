
/*
 *  Platz - A platformer framework for the Uzebox (supports VIDEO_MODE 3)
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

#ifndef PLATZ_H
#define PLATZ_H

/****************************************
 *			  	 Defines				*
 ****************************************/

#ifndef MAX_ANIMATED_BGS		// Maximum animated bgs per world slice
	#define MAX_ANIMATED_BGS 0
#endif

#ifndef MAX_MOVING_PLATFORMS	// Maximum moving platforms per world slice
	#define MAX_MOVING_PLATFORMS 0
#endif

#ifndef VIEWPORT_SLACK			// How far from the viewport anchor should the player move before we scroll
	#define VIEWPORT_SLACK 0
#endif

#ifdef DUAL_AXIS_SCROLLING
	#define MAX_VIS_SLICES 2	// 4 - Not yet supported
#else
	#define MAX_VIS_SLICES 2
#endif

#define GET_VEL(v)	(((v).frames&(v).mod)?(v).disp:(v).modDisp)		// Takes a velocity struct v

// Constants
#define L_INTERSECT		1
#define R_INTERSECT		2
#define V_INTERSECT		(L_INTERSECT|R_INTERSECT)
#define T_INTERSECT		4
#define B_INTERSECT		8
#define H_INTERSECT		(T_INTERSECT|B_INTERSECT)
#define AXIS_X			1		// Platform axes
#define AXIS_Y			2		//
#define ORI_LRUD		0		// Trigger orientations - Left/Right Up/Down
#define ORI_RLDU		1		// Right/Left Down/Up
#define PF_ZERO			0xff	// Invalid platform directory index - no platforms exist in this slice
#define MAX_SLICES		0xfe	// 0xff left for invalid flag
#define DIR_RIGHT 		1
#define DIR_LEFT		-1
#define DIR_DOWN		1
#define DIR_UP			-1
#define DIR_NONE		0
#define PLATZ_SCRN_WID	240
#define PLATZ_SCRN_HGT	224

// Level slice bit-flags
#define BGC		0x01	// Collidable
#define BGP		0x02	// Patterned
#define BGA		0x04	// Animated (automatically repeats like a pattern)
#define BGTH	0x08	// Triggerable - horizontal
#define BGTV	0x10	// Triggerable - vertical
#define BGI		0x20	// Invisible

/****************************************
 *			Type declarations			*
 ****************************************/

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef struct pt {
	u8	x;
	u8	y;
} pt;

typedef struct pt16 {
	int	x;
	int	y;
} pt16;

typedef struct rect {
	u8 left;
	u8 right;
	u8 top;
	u8 btm;
} rect;

typedef struct rect16 {
	int left;
	int right;
	int top;
	int btm;
} rect16;

typedef struct line16 {
	pt16 p1;
	pt16 p2;
} line16;

typedef struct velocity {		// Single axis velocity
	char 	vel;				// Base velocity - implemented as mod, modDisp and disp
	char	dir;				// Direction of travel - useful for when spd == 0
	u8		frames;				// Counts the game frames for use by mod
	u8 		mod;				// Fractional speed adjustment (n-1)
	char	modDisp;			// Special case displacement when frames&mod == 0
	char	disp;				// Typical displacement
} velocity;

typedef struct animation {
	u8			size;			// wid*hgt
	u8			wid;			// Width of each frame (in tiles)
	u8			hgt;			// Height of each frame (in tiles)
	u8			frameCount;		// # of frames in an animation cycle
	u8 			currFrame;		// The frame that is currently displayed
	u8			disp;			// Displacement counter
	u8			dpf;			// Displacement per frame (scales frame rate to movement speed)
	const char	*frames;		// Stored in flash
} animation;

typedef struct patternDirectory {
	u8 wid;
	u8 hgt;
	u8 index;					// Position in patTbl at which this pattern begins
} patternDirectory;

typedef struct bgInner {
	u8 type;					// 0|BGA|BGP
	u8 tile;
	rect r;
} bgInner;

typedef struct bgOuter {
	u8 type;					// 0|BGC|BGT*|BGI
	u8 count;					// # of inner bgs in this outer bg
	u16 index;					// The position in bgiTbl at which the inner bgs begin
	u8 animCount;				// The # of animated background elements in the bgOuter (always the first elements for easy loading)	
	rect r;
} bgOuter;

typedef struct object {			// Non-interactive objects (may be option interactivity in a future release)
	pt			begin;			// Position that top-left corner of object will be placed
	const char	*map;			// Object data
} object;

typedef struct bgDirectory {
	u16 objOffset;				// Index into pgmObjects flash array
	u8 objCount;				// The # of objects in the slice
	u16 bgoIndex;				// Index into pgmBgs flash array
	u8 bgoCount;				// The # of background elements in the slice
	u8 bgoBeginCount;			// The # of left-to-right seam collision bgs
	u8 bgoEndIndex;				// Points to right-to-left seam collision bgs
	u8 bgoEndCount;				// The # of right-to-left seam collision bgs
	u8 animCount;				// The # of animated background elements in the slice (always the first elements in the slice for easy loading)	
	u8 pdIndex;					// Index into platforms directory (PF_ZERO if none)
} bgDirectory;

typedef struct platform {
	u8		min;				// Highest/Leftmost position of platform movement
	u8		max;				// Lowest/Rightmost position of platform movement
	u8		axis;				// Horizontally or vertically moving
	char	v;					// Velocity
	rect 	r;					// Platform bounds
} platform;

typedef struct platformDirectory {
	u8 index;					// Position in platTbl that these platforms can be found
	u8 count;					// # of platforms to retrieve
} platformDirectory;

typedef struct platzActor {
	pt 			loc;			// Player location within the current slice
	u8 			sprx;			// Center of sprite on x axis
	pt 			trLoc;			// Trigger location relative to loc
	u8 			bbx;			// Bounding box x radius
	u8 			bby;			// Bounding box y radius
	velocity 	vx;				// X-axis velocity
	velocity	vy;				// Y-axis velocity
} platzActor;

typedef void (*trigCallback)(u8, u8, char);

/****************************************
 *			Function prototypes			*
 ****************************************/

// Platz main interface
u8 PlatzGetCollisionPointer(void);
u8 PlatzGetWorldSlicePointer(void);
void PlatzInit(platzActor *a, u8 sliceCount);
u8 PlatzMove(platzActor *a);
void PlatzMoveToSlice(platzActor *a, u8 sp);
u8 PlatzSetBoundingBoxDimensions(platzActor *a, u8 wid, u8 hgt);
void PlatzSetVelocity(velocity *v, char val, u8 *trPos);
void PlatzTick(void);

#if MAX_MOVING_PLATFORMS
	void PlatzSetMovingPlatformTiles(u8 hTilesIndex, u8 vTilesIndex, u8 clrTile);
#endif

// Platz initialization
void PlatzSetTriggerCallback(trigCallback tcb);
void PlatzSetMovingPlatformTable(const platform *p);
void PlatzSetMovingPlatformDirectory(const platformDirectory *pd);
void PlatzSetAnimatedBgTable(const animation *a);
void PlatzSetPatternTable(const char *p);
void PlatzSetPatternDirectory(const patternDirectory *tp);
void PlatzSetObjectTable(const object *obj);
void PlatzSetInnerBgTable(const bgInner *bgi);
void PlatzSetOuterBgTable(const bgOuter *bgo);
void PlatzSetBgDirectory(const bgDirectory *bgd);

// Platz utilities
char PlatzCcw(const pt16 *p0, const pt16 *p1, const pt16 *p2);
void PlatzHideSprite(u8 spriteIndex, u8 wid, u8 hgt);
u8 PlatzLinesIntersect(const line16 *l1, const line16 *l2);
void PlatzMapSprite(u8 offset, u8 wid, u8 hgt, const char *map);
u8 PlatzRectsIntersect(const rect *r1, const rect *r2);
u8 PlatzRectsIntersect16(const rect16 *r1, const rect16 *r2);

#endif

