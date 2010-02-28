
/*
 *  Emuze - An Eeprom Management Tool for the Uzebox
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
#include "data/font.pic.inc"
#include "data/tiles.pic.inc"
#include "data/tiles.map.inc"


/****************************************
 *			  	 Defines				*
 ****************************************/
#define GAMES_COUNT 20

#define EEPROM_BLOCK_COUNT 64
#define STATE_COUNT 6
#define MENU_OPTIONS_COUNT 6
#define MAX_MENU_WID 10
#define MAX_MENU_HGT (MENU_OPTIONS_COUNT+2)

#define OPTION_EDIT 0
#define OPTION_COPY 1
#define OPTION_PASTE 2
#define OPTION_RELOAD 3
#define OPTION_SAVE 4
#define OPTION_FORMAT 5

#define CLEAR_TILE 0
#define MENU_TOP_LEFT 1
#define MENU_TOP_RIGHT 2
#define MENU_HORIZ_TOP 7
#define MENU_HORIZ_BTM 8
#define MENU_BTM_LEFT 5
#define MENU_BTM_RIGHT 6
#define MENU_VERT_LEFT 3
#define MENU_VERT_RIGHT 4

#define CURSOR_HGREEN 18
#define CURSOR_HRED 19
#define CURSOR_VGREEN 16
#define CURSOR_VRED 17

#define HEX_DUMP_X 5
#define HEX_DUMP_Y 23

#define PAGE_COUNT (EEPROM_BLOCK_COUNT / GAMES_PER_PAGE + EEPROM_BLOCK_COUNT % GAMES_PER_PAGE)
#define GAMES_PER_PAGE 7
#define BLOCK_MENU_LOC_X 6
#define BLOCK_MENU_LOC_Y 2
#define BLOCK_MENU_CELL_WID (SCREEN_TILES_H - (BLOCK_MENU_LOC_X + 1))
#define BLOCK_MENU_CELL_HGT 2
#define GAME_ICON_HGT 2
#define GAME_ICON_WID 3

#define PAGE_NO_LOC_X 15
#define PAGE_NO_LOC_Y 24
#define PAGE_NO_WID 10
#define PAGE_NO_HGT 1


// Utility macros
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define ABS(x) (((x) > 0) ? (x) : -(x))


/****************************************
 *			Type declarations			*
 ****************************************/

// Navigation states (determines how we interpret user input)
typedef enum { 
	navBlock, navMenu, navMsgBox, navHexDump, navHexCell
} navStates;

typedef struct {
	u8 x;
	u8 y;
} pt;

typedef struct {
	u8 w;
	u8 h;
} size;

typedef struct {
	u8 min;
	u8 mid;
	u8 max;
} range;

typedef struct {
	u16 id;
	const char *title;
	const int *icon;
} gameDetails;

typedef struct {
	navStates curr;
	navStates *prev;
	navStates history[STATE_COUNT+1];
	u8 indexes[STATE_COUNT];
} EmuzeState;

typedef struct {
	u8 tile;
	range r;
	pt loc;
} menuCursor;

typedef struct tileMenu tileMenu;

struct tileMenu {
	u8 isVisible;
	pt loc;
	size s;
	menuCursor cursor;
	void (*menuActivate)(tileMenu*);
	u8 *buffer;
};
/****************************************
 *			File-level variables		*
 ****************************************/
navStates msgBoxLock;
EmuzeState state;
tileMenu blockMenu;
tileMenu menu;
tileMenu hexMenu;
struct EepromBlockStruct ebs;
struct EepromBlockStruct clipboard;
int page;
u16 pageIds[GAMES_PER_PAGE];
u16 gameIds[GAMES_COUNT];

const char freeBlockStr[] PROGMEM = "FREE BLOCK";
const char pageStr[] PROGMEM = "PAGE";
const char fwdSlashString[] PROGMEM = "/";

u8 menuBuffer[MAX_MENU_WID*MAX_MENU_HGT*VRAM_ADDR_SIZE];

const char editStr[] PROGMEM = "EDIT";
const char copyStr[] PROGMEM = "COPY";
const char pasteStr[] PROGMEM = "PASTE";
const char reloadStr[] PROGMEM = "RE-LOAD";
const char saveStr[] PROGMEM = "SAVE";
const char formatStr[] PROGMEM = "FORMAT";

const char *menuStrings[] PROGMEM = {
	editStr,
	copyStr,
	pasteStr,
	reloadStr,
	saveStr,
	formatStr
};

const char kernelGame[] PROGMEM = "RESERVED";
const char megatrisGame[] PROGMEM = "MEGATRIS";
const char whackAMoleGame[] PROGMEM = "WHACK-A-MOLE";
const char voidFighterGame[] PROGMEM = "VOID FIGHTER";
const char pongGame[] PROGMEM = "PONG";
const char arkanoidGame[] PROGMEM = "ARKANOID";
const char drMarioGame[] PROGMEM = "DR. MARIO";
const char lodeRunnerGame[] PROGMEM = "LODE RUNNER";
const char unkownGame[] PROGMEM = "???";
const char spaceInvadersGame[] PROGMEM = "SPACE INVADERS";
const char pacmanGame[] PROGMEM = "PAC-MAN";
const char bcdashGame[] PROGMEM = "B.C. DASH";
const char sokobanWorldGame[] PROGMEM = "SOKOBAN WORLD";
const char advOfLoloGame[] PROGMEM = "ADVENTURES OF LOLO";
const char zombienatorGame[] PROGMEM = "ZOMBIENATOR";
const char corridaNebososaGame[] PROGMEM = "CORRIDA NEBOSOSA";
const char castlevaniaGame[] PROGMEM = "CASTLEVANIA: VENGEANCE";

gameDetails games[] PROGMEM = {
	{ 0, kernelGame, mapKernelIcon },
	{ 1, kernelGame, mapKernelIcon },
	{ 2, megatrisGame, mapMegatrisIcon },
	{ 3, whackAMoleGame, mapWhackAMoleIcon },
	{ 4, voidFighterGame, mapVoidFighterIcon },
	{ 5, pongGame, mapPongIcon },
	{ 6, arkanoidGame, mapArkanoidIcon },
	{ 7, drMarioGame, mapUnknownIcon },
	{ 8, lodeRunnerGame, mapUnknownIcon },
	{ 11, unkownGame, mapUnknownIcon },
	{ 12, spaceInvadersGame, mapSpaceInvadersIcon },
	{ 13, pacmanGame, mapPacmanIcon },
	{ 14, bcdashGame, mapBcDashIcon },
	{ 15, bcdashGame, mapBcDashIcon },
	{ 61, sokobanWorldGame, mapUnknownIcon },
	{ 62, sokobanWorldGame, mapUnknownIcon },
	{ 63, advOfLoloGame, mapAdvOfLolo },
	{ 89, zombienatorGame, mapZombienatorIcon },
	{ 569, corridaNebososaGame, mapUnknownIcon },
	{ 666, castlevaniaGame, mapUnknownIcon }
};

/****************************************
 *			Function declarations		*
 ****************************************/

void fillRegion(u8 x, u8 y, u8 width, u8 height, u16 tile);
void drawLogo();
void drawMenu(tileMenu *m);
void closeMenu(tileMenu *m);
void moveCursor(menuCursor *cursor, char dist);
void drawCursor(menuCursor *cursor);
void hideCursor(menuCursor *cursor);
void printPgmStrings(u8 x, u8 y, u8 count, PGM_P *strs);
void setState(u8 newState);
void initMenus(void);
void readBlock(void);
void writeBlock(void);
void formatBlock(int index);
void flipPage(char dir);
void dumpHex(struct EepromBlockStruct *ebs);
void loadGameIds(void);
void printPageNumber(void);
void clearPageNumber(void);

/****************************************
 *			Function definitions		*
 ****************************************/

void fillRegion(u8 x, u8 y, u8 width, u8 height, u16 tile) {
	for (u8 i = y; i < (y + height); i++) {
		for (u8 j = x; j < (x + width); j++)
			SetTile(j, i, tile);
	}
}


void drawLogo(void) {
	u8 logoTimer = 4;
	int i = SCREEN_TILES_V - mapEmuzeLogo_Height;
	
	while (i > 0) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			if (--logoTimer == 0) {
				DrawMap((i&1)*2, i, mapEmuzeLogo);
				logoTimer = 4;
				i-=3;
			}
		}
	}
}


u16 findGameIndex(u16 id) {
	for (u16 i = 0; i < GAMES_COUNT; i++) {
		if (gameIds[i] == id)
			return i;
	}
	return 9; // Return unknown icon
}


void flipPage(char dir) {
	if (dir)
		dir = dir / ABS(dir); // Only single unit movement
	page = (page + dir) % PAGE_COUNT;

	if (page < 0)
		page = PAGE_COUNT - 1;
	// Load page ids
	u16 offset;

	for (u8 i = 0; i < GAMES_PER_PAGE; i++) {
		offset = (i + page * GAMES_PER_PAGE) * EEPROM_BLOCK_SIZE;

		if (offset < 64*EEPROM_BLOCK_SIZE)
			pageIds[i] = (u16)ReadEeprom(offset) + ((u16)ReadEeprom(offset+1)<<8);
		else
			break;
	}
	state.indexes[navBlock] = page * GAMES_PER_PAGE + state.indexes[navBlock] % GAMES_PER_PAGE;
	// Clamp to our index bounds
	moveCursor(&blockMenu.cursor, 0);
}


void drawMenu(tileMenu *m) {
	if (m->isVisible)
		return;
	m->isVisible = 1;

	if (m == &blockMenu) {
		u8 x = m->loc.x, y = m->loc.y;
		u16 index = 0;
		gameDetails details;

		for (u8 i = 0; i < GAMES_PER_PAGE; i++, y += GAME_ICON_HGT+1) {
			if ((page * GAMES_PER_PAGE + i) >= EEPROM_BLOCK_COUNT)
				break;
			// Find game id that matches idList for this page
			index = findGameIndex(pageIds[i]);
			memcpy_P(&details, games+index, sizeof(gameDetails));
			// Draw map and print text for game that matches id
			DrawMap(x, y, details.icon);
			PrintInt(x + GAME_ICON_WID + 6, y+1, pageIds[i], 1);

			if (pageIds[i] == EEPROM_FREE_BLOCK)
				Print(x + GAME_ICON_WID + 8, y+1, freeBlockStr);
			else
				Print(x + GAME_ICON_WID + 8, y+1, details.title);
		}
	} else if (m == &menu) {
		u8 x = m->loc.x, y = m->loc.y, w = m->s.w, h = m->s.h;

		// Save underlying tiles
		for (u8 i = y; i < (y+h); i++)
			memcpy(m->buffer + (i - y)*w*2, vram + (x + i*VRAM_TILES_H)*2, w*2);

		// Draw menu
		SetTile(x, y, MENU_TOP_LEFT);
		fillRegion(x+1, y, w-2, 1, MENU_HORIZ_TOP);
		SetTile(x+w-1, y, MENU_TOP_RIGHT);
		++y;
	
		for (u8 i = 1; i < (h-1); i++) {
			SetTile(x, y, MENU_VERT_LEFT);
			fillRegion(x+1, y, w-2, 1, CLEAR_TILE);
			SetTile(x+w-1, y, MENU_VERT_RIGHT);
			++y;
		}
	
		SetTile(x, y, MENU_BTM_LEFT);
		fillRegion(x+1, y, w-2, 1, MENU_HORIZ_BTM);
		SetTile(x+w-1, y, MENU_BTM_RIGHT);
	} else if (m == &hexMenu) {
		readBlock();
		dumpHex(&ebs);
	}
}


void closeMenu(tileMenu *m) {
	if (!m->isVisible)
		return;
	m->isVisible = 0;

	u8 y = m->loc.y;

	if (m == &blockMenu) {
		for (u8 i = 0; i < GAMES_PER_PAGE; i++, y += GAME_ICON_HGT+1)
			fillRegion(m->loc.x, y, BLOCK_MENU_CELL_WID, BLOCK_MENU_CELL_HGT, CLEAR_TILE);
		hideCursor(&m->cursor);
	} else if (m == &menu) {
		// Restore underlying tiles
		for (u8 i =m->loc.y; i < (m->loc.y + m->s.h); i++)
			memcpy(vram + (m->loc.x + i*VRAM_TILES_H)*2, m->buffer + (i - m->loc.y)*m->s.w*2, m->s.w*2);
	} else if (m == &hexMenu) {
		fillRegion(hexMenu.loc.x, hexMenu.loc.y, 32, 3, CLEAR_TILE);
		hideCursor(&m->cursor);
	}
}


void hideCursor(menuCursor *cursor) {
	SetTile(cursor->loc.x, cursor->loc.y, CLEAR_TILE);
}


void moveCursor(menuCursor *cursor, char dist) {
	int index = state.indexes[state.curr];

	hideCursor(cursor);

	if (cursor == &blockMenu.cursor) {
		index += dist;

		if (index < 0)
			index = GAMES_PER_PAGE + index;
		state.indexes[state.curr] = (page * GAMES_PER_PAGE) + (index % GAMES_PER_PAGE);
		cursor->loc.y = (BLOCK_MENU_LOC_Y+1) + (GAME_ICON_HGT+1) * (state.indexes[state.curr] % GAMES_PER_PAGE);

		if (state.indexes[navBlock] >= EEPROM_BLOCK_COUNT) {
			if (dist < 0)
				moveCursor(cursor, -1);
			else
				moveCursor(cursor, page * GAMES_PER_PAGE - state.indexes[navBlock]);
		}
	} else if (cursor == &menu.cursor) {
		if (index == cursor->r.min && dist < 0) {
			cursor->loc.y += cursor->r.max;
			state.indexes[state.curr] = cursor->r.max;
		} else if (index == cursor->r.max && dist == 1) {
			cursor->loc.y -= cursor->r.max;
			state.indexes[state.curr] = cursor->r.min;
		} else {
			cursor->loc.y += dist;
			state.indexes[state.curr] += dist;
		}
	} else if (cursor == &hexMenu.cursor) {
		index = (index + dist) % (cursor->r.max);

		if (index < 0)
			index = cursor->r.max + index;
		state.indexes[state.curr] = index;
		cursor->loc.x = HEX_DUMP_X + index % cursor->r.mid;
		cursor->loc.y = (HEX_DUMP_Y - 1) + 2 * (index / cursor->r.mid);
	}
	drawCursor(cursor);
}


void drawCursor(menuCursor *cursor) {
	SetTile(cursor->loc.x, cursor->loc.y, cursor->tile);
}


void printPgmStrings(u8 x, u8 y, u8 count, PGM_P *strs) {
	u8 i = 0;

	while (count--)
		Print(x, y++, (PGM_P)pgm_read_word(strs+i++));
}


void printPageNumber(void) {
	u8 x = (page > 8) ? 1 : 0;

	clearPageNumber();
	PrintInt(PAGE_NO_LOC_X + 8 + x, PAGE_NO_LOC_Y, PAGE_COUNT, 0);
	Print(PAGE_NO_LOC_X + 6 + x, PAGE_NO_LOC_Y, fwdSlashString);
	PrintInt(PAGE_NO_LOC_X + 5 + x, PAGE_NO_LOC_Y, page+1, 0);
	Print(PAGE_NO_LOC_X, PAGE_NO_LOC_Y, pageStr);
}


void clearPageNumber(void) {
	fillRegion(PAGE_NO_LOC_X, PAGE_NO_LOC_Y, PAGE_NO_WID, PAGE_NO_HGT, CLEAR_TILE);
}

void setState(u8 newState) {
	u8 oldState = state.curr;

	if (newState < navBlock || newState > navHexCell)
		return;
	if (newState > state.curr) {
		*state.prev = state.curr;
		++state.prev;
		state.curr = newState;
	} else if (newState < state.curr) {
		while (state.curr > newState) {
			--state.prev;
			state.curr = *state.prev;
		}
	}

	switch (state.curr) {
		case navBlock:
			closeMenu(&hexMenu);
			closeMenu(&menu);
			printPageNumber();
			blockMenu.cursor.tile = CURSOR_VGREEN;
			drawCursor(&blockMenu.cursor);
			break;
		case navMenu:
			clearPageNumber();

			if (oldState == navBlock) {
				drawMenu(&menu);
				printPgmStrings(17, 9, MENU_OPTIONS_COUNT, menuStrings);
				drawMenu(&hexMenu);
			} else if (oldState == navHexDump) {
				hideCursor(&hexMenu.cursor);
			}
			menu.cursor.tile = CURSOR_VGREEN;
			drawCursor(&menu.cursor);
			break;
		case navMsgBox:
			break;
		case navHexDump:
			hexMenu.cursor.tile = CURSOR_HGREEN;
			drawCursor(&hexMenu.cursor);
			break;
		case navHexCell:
			break;
	}
}


void dumpHex(struct EepromBlockStruct *ebs) {
	u8 x = HEX_DUMP_X, y = HEX_DUMP_Y;
	u8 *dptr = (u8*)ebs;

	for (u8 i = 0; i < EEPROM_BLOCK_SIZE; i++, x+=2) {
		if (i && ((i&15) == 0)) {
			x = HEX_DUMP_X;
			y += 2;
		}
		PrintHexByte(x, y, *dptr++);
	}
}


void activateItem(u8 index) {
	switch (state.curr) {
		case navBlock:
			blockMenu.cursor.tile = CURSOR_VRED;
			drawCursor(&blockMenu.cursor);
			setState(navMenu);
			break;
		case navMenu:
			switch (index) {
				case OPTION_EDIT:
					menu.cursor.tile = CURSOR_VRED;
					drawCursor(&menu.cursor);
					setState(navHexDump);
					break;
				case OPTION_COPY:
					memcpy(&clipboard, &ebs, sizeof(clipboard));
					break;
				case OPTION_RELOAD:
					closeMenu(&hexMenu);
					drawMenu(&hexMenu);
					break;
				case OPTION_PASTE:
					if (clipboard.id == EEPROM_FREE_BLOCK)
						break;
					memcpy(&ebs, &clipboard, sizeof(ebs));
					// Allow fall through
				case OPTION_SAVE:
					writeBlock();
					setState(navBlock);
					flipPage(0);
					closeMenu(&blockMenu);
					drawMenu(&blockMenu);
					activateItem(state.indexes[state.curr]);
					break;
				case OPTION_FORMAT:
					formatBlock(state.indexes[navBlock]);
					setState(navBlock);
					flipPage(0);
					closeMenu(&blockMenu);
					drawMenu(&blockMenu);
					activateItem(state.indexes[state.curr]);
					break;
			}
			break;
		case navMsgBox:
			break;
		case navHexDump:
			hexMenu.cursor.tile = CURSOR_HRED;
			drawCursor(&hexMenu.cursor);
			setState(navHexCell);
			break;
		case navHexCell:
			break;
	}
}


void readBlock(void) {
	u8 *destPtr;
	u16 destAddr;

	destPtr = (u8*)&ebs;
	destAddr = state.indexes[navBlock] * EEPROM_BLOCK_SIZE;

	for(u8 j = 0; j < EEPROM_BLOCK_SIZE; j++)
		*destPtr++ = ReadEeprom(destAddr++);
}


void writeBlock(void) {
	u8 *dptr = (u8*)&ebs;

	for (u8 i = 0; i < EEPROM_BLOCK_SIZE; i++)
		WriteEeprom(state.indexes[navBlock]*EEPROM_BLOCK_SIZE+i, *dptr++);
}


void formatBlock(int index) {
	for (u8 i = 0; i < sizeof(ebs.id); i++)
		WriteEeprom(index*EEPROM_BLOCK_SIZE+i,(u8)EEPROM_FREE_BLOCK);
}


void PrintHexNibble(char x, char y, u8 byte) {
	if (byte <= 9){
		SetFont(x,y,byte+('0'-' '));
	} else {
		SetFont(x,y,byte+('A'-' '-10));
	}
}


void cycleHexCell(char dir) {
	u8 index = state.indexes[navHexDump]>>1;
	u8 *dptr = (u8*)&ebs + index;

	if (state.indexes[navHexDump]&1) {
		*dptr = ((*dptr)&0xf0) + (((*dptr)+dir)&0x0f);
		PrintHexNibble(hexMenu.cursor.loc.x, hexMenu.cursor.loc.y+1, (*dptr)&0x0f);
	} else {
		*dptr = (((*dptr)+dir*0x10)&0xf0) + ((*dptr)&0x0f);
		PrintHexNibble(hexMenu.cursor.loc.x, hexMenu.cursor.loc.y+1, (*dptr)>>4);
	}
}


void initMenus(void) {
	blockMenu.loc = (pt){BLOCK_MENU_LOC_X, BLOCK_MENU_LOC_Y};
	blockMenu.s = (size){0, 0};
	blockMenu.cursor.tile = CURSOR_VGREEN;
	blockMenu.cursor.r = (range){0, GAMES_PER_PAGE-1, GAMES_PER_PAGE-1};
	blockMenu.cursor.loc = (pt){blockMenu.loc.x-1, blockMenu.loc.y+1};
	blockMenu.buffer = 0;

	menu.loc = (pt){15, 8};
	menu.s = (size){10, MENU_OPTIONS_COUNT+2};
	menu.cursor.tile = CURSOR_VGREEN;
	menu.cursor.r = (range){0, MENU_OPTIONS_COUNT-1, MENU_OPTIONS_COUNT-1};
	menu.cursor.loc = (pt){menu.loc.x+1, menu.loc.y+1};
	menu.buffer = menuBuffer;

	hexMenu.loc = (pt){HEX_DUMP_X, HEX_DUMP_Y};
	hexMenu.s = (size){EEPROM_BLOCK_SIZE, 3};
	hexMenu.cursor.tile = CURSOR_HGREEN;
	hexMenu.cursor.r = (range){0, EEPROM_BLOCK_SIZE, EEPROM_BLOCK_SIZE<<1};
	hexMenu.cursor.loc = (pt){hexMenu.loc.x, hexMenu.loc.y-1};
	hexMenu.buffer = 0;
}


void loadGameIds(void) {
	for (u16 i = 0; i < GAMES_COUNT; i++)
		gameIds[i] = pgm_read_word(games+i);
}


int main(void) {
	u8 hexDumpTimer = 4;
	u8 menuActivationTimer = 0;
	u16 btnPrev = 0;			// Previous buttons that were held
	u16 btnHeld = 0;    		// Buttons that are held right now
	u16 btnPressed = 0;  		// Buttons that were pressed this frame
	u16 btnReleased = 0;		// Buttons that were released this frame

	SetFontTable(fontset);
	SetTileTable(tileset);
	ClearVram();
	drawLogo();
	clipboard.id = EEPROM_FREE_BLOCK;
	loadGameIds();
	initMenus();
	setState(navBlock);
	page = -1;
	flipPage(1);
	printPageNumber();
	state.indexes[navBlock] = 0;
	drawMenu(&blockMenu);

	while(1) {
		if (GetVsyncFlag()) {
			ClearVsyncFlag();

			btnHeld = ReadJoypad(0);
			btnPressed = btnHeld&(btnHeld^btnPrev);
        	btnReleased = btnPrev&(btnHeld^btnPrev);
			btnPrev = btnHeld;

			switch (state.curr) {
				case navBlock:
					if (btnPressed & BTN_A) {
						activateItem(state.indexes[state.curr]);
					} if (btnPressed & (BTN_UP|BTN_DOWN)) {
						moveCursor(&blockMenu.cursor, (btnPressed&BTN_UP) ? -1 : 1);
						drawCursor(&blockMenu.cursor);
					} else if (btnPressed & (BTN_LEFT|BTN_RIGHT)) {
						hideCursor(&blockMenu.cursor);
						flipPage((btnPressed&BTN_LEFT) ? -1 : 1);
						printPageNumber();
						closeMenu(&blockMenu);
						drawMenu(&blockMenu);
						drawCursor(&blockMenu.cursor);
					}
					break;
				case navMenu:
					if (!menu.isVisible) {
						setState(navBlock);
						continue;
					}
					if (menuActivationTimer) {
						if (--menuActivationTimer == 0) {
							menu.cursor.tile = CURSOR_VGREEN;
							drawCursor(&menu.cursor);
						} else {
							continue;
						}
					}
					if (btnPressed & (BTN_UP|BTN_DOWN)) {
						moveCursor(&menu.cursor, (btnPressed&BTN_UP) ? -1 : 1);
						drawCursor(&menu.cursor);
					} else if (btnPressed & BTN_A) {
						activateItem(state.indexes[state.curr]);

						// Visualize button depression for all but editing option
						if (state.indexes[state.curr]) {
							menu.cursor.tile = CURSOR_VRED;
							drawCursor(&menu.cursor);
							menuActivationTimer = 8;
						}
					} else if (btnPressed & BTN_B) {
						setState(navBlock);
					}
					break;
				case navMsgBox:

					break;
				case navHexDump:
					if (btnPressed & BTN_A) {
						activateItem(state.indexes[state.curr]);
					} else if (btnPressed & BTN_B) {
						setState(navMenu);
					} else if (btnPressed & (BTN_LEFT|BTN_RIGHT)) {
						moveCursor(&hexMenu.cursor, (btnPressed&BTN_LEFT) ? -1 : 1);
						drawCursor(&hexMenu.cursor);
						hexDumpTimer = 4;
					} else if (btnHeld & (BTN_LEFT|BTN_RIGHT)) {
						if (hexDumpTimer) {
							--hexDumpTimer;
						} else {
							moveCursor(&hexMenu.cursor, (btnHeld&BTN_LEFT) ? -1 : 1);
							drawCursor(&hexMenu.cursor);
							hexDumpTimer = 2;
						}
					} else if (btnPressed & (BTN_UP|BTN_DOWN)) {
						moveCursor(&hexMenu.cursor, (btnPressed&BTN_UP) ? -hexMenu.cursor.r.mid : hexMenu.cursor.r.mid);
						drawCursor(&hexMenu.cursor);
					}
					break;
				case navHexCell:
					if (btnPressed & BTN_B) {
						setState(navHexDump);
					} else if (btnPressed & (BTN_UP|BTN_DOWN)) {
						cycleHexCell((btnPressed&BTN_UP) ? 1 : -1);
					}
					break;
			}
		}		
	}
}





