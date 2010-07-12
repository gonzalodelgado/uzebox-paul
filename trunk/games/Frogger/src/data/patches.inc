
const char sfxHop0[] PROGMEM ={
0,PC_ENV_SPEED,-10,
0,PC_PITCH,55,
1,PC_NOTE_UP,5,  
1,PC_NOTE_UP,5, 
2,PC_NOTE_UP,5,  
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxHop1[] PROGMEM ={
0,PC_ENV_SPEED,-10,
0,PC_PITCH,55,
1,PC_NOTE_UP,6,  
1,PC_NOTE_UP,6, 
2,PC_NOTE_UP,6,  
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxHop2[] PROGMEM ={ 
0,PC_ENV_SPEED,-10,
0,PC_PITCH,55,
1,PC_NOTE_UP,7,  
1,PC_NOTE_UP,7, 
2,PC_NOTE_UP,7,  
2,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxBeep[] PROGMEM ={
0,PC_WAVE,9,
0,PC_PITCH,65,
0,PC_ENV_SPEED,-5,
10,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxDead[] PROGMEM ={
0,PC_WAVE,1,
0,PC_ENV_SPEED,-5,
0,PC_PITCH,50,
2,PC_NOTE_DOWN,6,  
2,PC_NOTE_DOWN,6, 
2,PC_NOTE_DOWN,6,  
4,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxBonus[] PROGMEM ={
0,PC_WAVE,2,
0,PC_ENV_VOL,0xff,
2,PC_NOTE_UP,6,
2,PC_NOTE_UP,6,
0,PC_ENV_SPEED,-10,
0,PATCH_END
};

const char sfxTimeBonus[] PROGMEM ={
0,PC_WAVE,7,
0,PC_PITCH,60,
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char sfxLifeLoss2P[] PROGMEM ={
0,PC_ENV_SPEED,-5,
0,PC_PITCH,60,
2,PC_NOTE_DOWN,6,  
2,PC_NOTE_DOWN,6, 
2,PC_NOTE_DOWN,6,  
4,PC_NOTE_CUT,0,
0,PATCH_END
};

/*
const char sfxTraffic[] PROGMEM ={
0,PC_WAVE,5,
0,PC_PITCH,45,
120,PC_ENV_SPEED,-3,
120,PC_NOTE_CUT,0,
0,PATCH_END
};
*/

const struct PatchStruct patches[] PROGMEM = {
{0,NULL,sfxHop0,0,0},
{0,NULL,sfxHop1,0,0},
{0,NULL,sfxHop1,0,0},
{0,NULL,sfxHop2,0,0},
{0,NULL,sfxBeep,0,0},
{0,NULL,sfxDead,0,0},
{0,NULL,sfxBonus,0,0},
{0,NULL,sfxTimeBonus,0,0},
{0,NULL,sfxLifeLoss2P,0,0},
//{0,NULL,sfxTraffic,0,0},
};
