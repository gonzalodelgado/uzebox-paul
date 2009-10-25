
const animation bobAnimations[] PROGMEM = {
	/* Lynx form */
	{6,3,2,1,0,0,0,0,animLynxIdleLt},
	{6,3,2,1,0,0,0,0,animLynxIdleRt},
	{6,3,2,5,0,0,4,0,animLynxRunLt},
	{6,3,2,5,0,0,4,0,animLynxRunRt},
	/* Dragonfly form */
	{4,2,2,3,0,0,4,0,animDFlyLt},
	{4,2,2,3,0,0,4,0,animDFlyRt},
	/* Turtle form */
	{2,2,1,3,0,0,8,0,animTurtleLt},
	{2,2,1,3,0,0,8,0,animTurtleRt},
	/* Snail form */
	{1,1,1,1,0,0,128,0,animSnailLt},
	{1,1,1,1,0,0,128,0,animSnailRt}
};

const animation bgAnimations[] PROGMEM = {
	{2,2,1,4,0,0,8,1,animWfallTop},
	{8,2,4,4,0,0,8,1,animWfallBtm},
	{1,1,1,4,0,0,16,1,animWave},
	{4,2,2,2,0,0,8,0,animFire},
	{4,2,2,4,0,0,10,0,animCoins},
	{1,1,1,4,0,0,16,0,animWave},	// Not synchronized
	{14,14,1,3,0,0,128,0,animPressStart}
};
