
/***** Change Log *****/

Platz 1.0.0
- Demo game created with LePlatz v1.0 available:
	http://code.google.com/p/uzebox-paul/downloads/list
- Eliminated compiler warnings of unused variables when setting MAX_MOVING_PLATFORMS=0 or MAX_ANIMATED_BGS=0.
- Fixed buffer overflow when using more than the default number of moving platforms per slice.
- Fixed painting of initial frame for slices with bg animations that exist in multiple Outer bgs.
- SLOW_BG_PATTERNS now incurs only a very slight cycle and flash penalty.
- BG flags BGTH and BGTV have been merged into BGT. BGTV's bit flag has been reassigned.
- Patterned BGs (BGP) are now stored as traditional maps. This makes large patterns far more manageable
  from a development standpoint.
- Platforms can now move in steps. This reduces required tiles from 44 to 1. Movement precision is
  tile-based for stepped moving platforms. These are best used for horizontal movement or when
  the player will not be carried by the platform.
- Platforms now specify their clear tile individually. This allows for placement of platforms
  against different single-tile backgrounds within a single game. Note that a full set of tiles
  are required for each background if smooth platforms are utilised.
- Platform clear tiles should be in the first 64 tiles (i.e masked with 0x3F).
- BGM flag indicates a mutable BG. These BGs can be dynamically customized and generate events
  upon player interaction. Customizations may persist for the current session or be saved to
  EEPROM and persist indefinitely (say for a saved game). BGs can be changed visually and collision-wise,
  such that deformable BGs are possible (NB: the deformed BG must be a valid BG and cannot be split
  into multiple BGs).
- BG animations (BGA) no longer have a positional requirement in their Inner/Outer BG index - instead,
  they have a separate directory aswell as entries in the main BG directory.
- Synch attribute added to BG animations. Setting to non-zero ensures animations of a similar type
  are synchronized accross slice boundaries.
- Programs suffering compatibility issues with Platz 1.0.0 can find Platz 0.3.2 at:
	http://code.google.com/p/uzebox-paul/source/browse/#svn/trunk/tools/platz-0.31
  Alternatively, queries regarding steps to make your pre-1.0 program 1.0 compatible can be posted
  to the official Uzebox forums: http://uzebox.org/forums/viewtopic.php?f=6&t=527
- PlatzMapSprite now takes an extra argument for sprite flipping
- Objects now index into the same maps table used by Patterned Inner Bgs. The object data structure is
  25% smaller due to not having to store the map's address internally.

Platz 0.3.1
- Fixed LRUD Right/Bottom half trigger to pass correct trig parameter to trigger callback.

Platz 0.3.0
- Initial alpha release



/***** Build Instructions *****/

To build, checkout platz folder and place Uzebox kernel (beta5 included in checkout) in src directory
at top level. Alternatively, edit the makefile to point to the location of the kernel.

The kernel is available at:
http://code.google.com/p/uzebox/source/browse/#svn/branches/rev-beta5


The following is optional:

Additionally, make the following kernel changes in uzeboxSoundEngine.c:
- In the function "TriggerFx", change the first conditional block from this:

///////////////////////////////////////
if(type==1 || type==2){
	//noise or PCM channel fx
	channel=3;
}else if(tracks[1].priority==0 || (tracks[1].fxPatchNo==patch && tracks[1].priority>0 && retrig==true)){ //fx already playing
	channel=1;
}else if(tracks[2].priority==0 || (tracks[2].fxPatchNo==patch && tracks[2].priority>0 && retrig==true)){ //fx already playing				
	channel=2;
}else{
	//both channels have fx playing, use the oldest one
	if(tracks[1].patchPlayingTime>tracks[2].patchPlayingTime){
		channel=1;
	}else{
		channel=2;
	}
}
//////////////////////////////////////

to this:

//////////////////////////////////////
if(type==1 || type==2){
      //noise or PCM channel fx
        channel=3;
}else if(tracks[2].priority==0 || (tracks[2].fxPatchNo==patch && tracks[2].priority>0 && retrig==true)){ //fx already playing                           
        channel=2;
}else if(tracks[1].priority==0 || (tracks[1].fxPatchNo==patch && tracks[1].priority>0 && retrig==true)){ //fx already playing
        channel=1;
}else{
        //both channels have fx playing, use the oldest one
        if(tracks[1].patchPlayingTime>tracks[2].patchPlayingTime){
                channel=1;
        }else{
                channel=2;
        }
}
//////////////////////////////////////

This ensures that sound effects do not clobber the music track.
