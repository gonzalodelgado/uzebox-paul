// !SLICE_COUNT! 3

// !PLATZ_OBJECTS!
const object pgmObjects[] PROGMEM = {
	{{3,1},mapStar},
	{{1,6},mapSign},			// Slice 1 (2,2)
	{{1,4},mapStar},
	{{4,6},mapSign},
	{{13,1},mapStar},
	{{27,3},mapCloud},			// Slice 2 (4,6)
	{{3,4},mapStar},
	{{10,6},mapMoon},
	{{12,13},mapStar},
	{{14,2},mapCloud},
	{{17,6},mapStar},
	{{22,1},mapStar},
	{{25,3},mapCloud},
	{{27,13},mapStar}			// Slice 3 (8,14)
};

const char* pgmMaps[] PROGMEM = {
	mapGrassLt,					// 0
	mapGrassTopMid,				// 1
	mapGrassRt,					// 2
	mapGndMid,					// 3
	mapGndBtmLtWater,			// 4
	mapGndBtmRtWater,			// 5
	mapGndBtmMidWater,			// 6
	mapMushroom					// 7
};


const bgAnimIndex pgmAnimDir[] PROGMEM = {
	{4,0},{4,1},					// Slice 0 (2,2)	(count,total)
	{5,0},							// Slice 1 (1,3)
	{1,0},{3,0}						// Slice 2 (2,5)
};

const platformDirectory pgmPlatformDir[] PROGMEM = {

};

// NB: Platform speeds must be a factor of 32
const platform pgmPlatforms[] PROGMEM = {

};

const bgInner pgmBgsInner[] PROGMEM = {
	{BGP,1,{0,9,10,11}},
	{0,TILE_GRASS_RT,{9,10,10,11}},
	{BGP,3,{0,10,11,25}},
	{BGP,0,{9,10,0,2}},
	{BGP,1,{10,26,0,1}},
	{BGP,2,{26,27,0,2}},
	{BGA,0,{10,26,1,16}},
	{BGA,1,{10,26,16,20}},
	{0,TILE_WATER,{10,26,20,25}},
	{0,TILE_WATER,{26,32,23,25}},
	{0,TILE_GRASS_LT,{26,27,10,11}},
	{BGP,1,{27,32,10,11}},
	{BGP,3,{26,32,11,21}},
	{BGP,4,{26,27,21,23}},
	{BGP,6,{27,32,21,23}},					// Slice 0 (15,15)
	{BGP,2,{12,13,10,12}},
	{BGP,1,{0,12,10,11}},
	{BGP,3,{0,12,11,21}},
	{BGP,6,{0,12,21,23}},
	{BGP,2,{20,21,12,14}},
	{BGP,1,{12,20,12,13}},
	{BGP,3,{12,20,13,21}},
	{BGP,6,{12,19,21,23}},
	{BGP,5,{19,20,21,23}},
	{0,TILE_WATER,{0,20,23,25}},
	{BGA,2,{20,32,17,18}},
	{0,TILE_WATER,{20,32,18,25}},
	{BGP,7,{25,31,14,16}},					// Slice 1 (13,28)
	{BGA,2,{0,32,17,18}},
	{0,TILE_WATER,{0,32,18,25}},
	{BGP,7,{8,14,15,17}},
	{BGA|BGM,4,{22,24,10,12}},
	{0,MUT_COIN,{22,24,10,12}},
	{BGP,7,{20,26,12,14}}					// Slice 2 (5,33)
};

// !PLATZ_OUTER_BGS!
const bgOuter pgmBgsOuter[] PROGMEM = {
	{BGC,3,0x0000,{0,10,10,25}},
	{BGC,1,0x0003,{9,10,0,2}},
	{BGC,1,0x0004,{10,26,0,1}},
	{BGC,1,0x0005,{26,27,0,2}},
	{0,3,0x0006,{10,26,1,25}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{10,26,17,20}},		// NB: Index acts as trigger id for BGT
	{0,1,0x0009,{26,32,23,25}},
	{BGC,5,0x000a,{26,32,10,23}},							// Slice 0 (8,8)
	{BGC,1,0x000f,{12,13,10,12}},
	{BGC,3,0x0010,{0,12,10,23}},
	{BGC,1,0x0013,{20,21,12,14}},
	{BGC,4,0x0014,{12,20,12,23}},
	{0,1,0x0018,{0,20,23,25}},
	{0,2,0x0019,{20,32,17,25}},
	{BGC,1,0x001b,{25,31,14,16}},
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{20,32,17,20}},		// Slice 1 (7,15)
	{BGT|BGI,ORI_LRUD,TRIG_WATER,{0,32,17,20}},
	{0,2,0x001c,{0,32,17,25}},
	{BGC,1,0x001e,{8,14,15,17}},
	{BGM,2,0x001f,{22,24,10,12}},
	{BGC,1,0x0021,{20,26,12,14}}							// Slice 2 (5,20)
};

// Save a lot of flash by making maps of regularly used combinations. These can now be one Inner BG instead of many.
// !PLATZ_BG_DIRECTORY!
const bgDirectory pgmBgDir[] PROGMEM = {
	{0x0000,0x02,0x0000,0x08,1,7,1,2,0,PF_ZERO},			// Slice 0
	{0x0002,0x04,0x0008,0x08,2,6,2,1,2,PF_ZERO},			// Slice 1
	{0x0006,0x08,0x0010,0x05,1,0,1,2,3,PF_ZERO}				// Slice 2
};
