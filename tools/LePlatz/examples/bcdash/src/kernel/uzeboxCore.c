/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
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
 *
 *  Uzebox is a reserved trade mark
*/

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "uzebox.h"

//#if VIDEO_MODE == 3 
//		extern struct SpriteStruct sprites[];
//		extern struct ScreenSectionStruct screenSections[];
//#endif

#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");


//#if INTRO_LOGO !=0
//		#include "data/uzeboxlogo.pic.inc"
//		#include "data/uzeboxlogo.map.inc"
//#endif





//Callbakcs defined in each video modes module
extern void DisplayLogo(); 
extern void InitializeVideoMode();


void ReadButtons();


extern unsigned char sync_phase;
extern unsigned char sync_pulse;
extern unsigned char curr_field;
extern unsigned char  curr_frame;
extern struct TrackStruct tracks[CHANNELS];

extern unsigned char burstOffset;
extern unsigned char vsync_phase;
extern volatile unsigned int joypad1_status_lo,joypad2_status_lo,joypad1_status_hi,joypad2_status_hi;
extern unsigned char tileheight, textheight;
extern unsigned char line_buffer[];
extern unsigned char render_start;
extern unsigned char playback_start;


bool snesMouseEnabled=false;

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void Initialize(void) __attribute__((naked)) __attribute__((section(".init8")));


void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

/**
 * Performs a software reset
 */
void SoftReset(void){        
	wdt_enable(WDTO_15MS);  
	while(1);
}

/*
void logo(){

	#if INTRO_LOGO !=0

		InitMusicPlayer(initPatches);

		#if VIDEO_MODE == 2
			#define LOGO_X_POS 8
			screenSections[0].tileTableAdress=uzeboxlogo;
		#elif VIDEO_MODE==3
			#define LOGO_X_POS 13
			SetTileTable(logo_tileset);
		#else			
			#define LOGO_X_POS 18
			SetTileTable(uzeboxlogo);
			SetFontTable(uzeboxlogo);
		#endif
			
		//draw logo
		ClearVram();
		WaitVsync(15);
		


		#if INTRO_LOGO == 1 
			InitMusicPlayer(initPatches);
			TriggerFx(0,0xff,true);
		#endif

		#if VIDEO_MODE == 3
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);
			WaitVsync(3);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo2);
			WaitVsync(2);
			DrawMap2(LOGO_X_POS,12,map_uzeboxlogo);
		#else
			DrawMap(LOGO_X_POS,12,map_uzeboxlogo);
			WaitVsync(3);
			DrawMap(LOGO_X_POS,12,map_uzeboxlogo2);
			WaitVsync(2);
			DrawMap(LOGO_X_POS,12,map_uzeboxlogo);
		#endif	

		#if INTRO_LOGO == 2

			SetMasterVolume(0xc0);
			TriggerNote(3,0,16,0xff);


		#endif 
		
		WaitVsync(20);

		#if VIDEO_MODE == 2
			WaitVsync(80);
		#else
			WaitVsync(10);
		#endif

		ClearVram();
		WaitVsync(20);
	#endif

}

*/

/**
 * Called by the assembler initialization routines, should not be called directly.
 */
int i;
void Initialize(void){


	cli();

	//Initialize the mixer buffer
	for(i=0;i<MIX_BANK_SIZE*2;i++){
		mix_buf[i]=0x80;
	}	
	
	mix_pos=mix_buf;
	mix_bank=0;

	for(i=0;i<CHANNELS;i++){
		mixer.channels.all[i].volume=0;
	}

	
	#if MIXER_CHAN4_TYPE == 0
		//initialize LFSR		
		tr4_barrel_lo=1;
		tr4_barrel_hi=1;		
		tr4_params=0b00000001; //15 bits no divider (1)
	#endif

	#if UART_RX_BUFFER == 1
		uart_rx_buf_start=0;
		uart_rx_buf_end=0;
	#endif

	#if MIDI_IN == 1
		UCSR0B=(1<<RXEN0); //set UART for MIDI in
		UCSR0C=(1<<UCSZ01)+(1<<UCSZ00);
		UBRR0L=56; //31250 bauds (.5% error)
	#endif

	
	//stop timers
	TCCR1B=0;
	TCCR0B=0;
	
	//set ports
	DDRC=0xff; //video dac
	DDRB=0xff; //h-sync for ad725
	DDRD=(1<<PD7)+(1<<PD4)+(1<<PD3); //audio-out, midi-in +led + switch
	PORTD|=(1<<PD4); //turn on led


	//setup port A for joypads
	DDRA =0b00001100; //set only control lines as outputs
	PORTA=0b11111011; //activate pullups on the data lines
	
	//PORTD=0;
	
	//set sync parameters. starts at odd field, in pre-eq pulses, line 1
	sync_phase=SYNC_PHASE_PRE_EQ;
	sync_pulse=SYNC_PRE_EQ_PULSES;

	//clear timers
	TCNT1H=0;
	TCNT1L=0;

	//set sync generator counter on TIMER1
	OCR1AH=HDRIVE_CL_TWICE>>8;
	OCR1AL=HDRIVE_CL_TWICE&0xff;

	TCCR1B=(1<<WGM12)+(1<<CS10);//CTC mode, use OCR1A for match
	TIMSK1=(1<<OCIE1A);			//generate interrupt on match

	//set clock divider counter for AD725 on TIMER0
	//outputs 14.31818Mhz (4FSC)
	TCCR0A=(1<<COM0A0)+(1<<WGM01); //toggle on compare match + CTC
	OCR0A=0; //divide main clock by 2
	TCCR0B=(1<<CS00); //enable timer, no pre-scaler

	//set sound PWM on TIMER2
	TCCR2A=(1<<COM2A1)+(1<<WGM21)+(1<<WGM20); //Fast PWM	
	OCR2A=0; //duty cycle (amplitude)
	TCCR2B=(1<<CS20);  //enable timer, no pre-scaler

	SYNC_PORT=(1<<SYNC_PIN)|(1<<VIDEOCE_PIN); //set sync & chip enable line to hi

	burstOffset=0;
	curr_frame=0;
	vsync_phase=0;
	joypad1_status_hi=0;
	joypad2_status_hi=0;
	snesMouseEnabled=false;

	//enable color correction
	ReadButtons();
	if(ReadJoypad(0)&BTN_B){
		SetColorBurstOffset(4);
	}

	InitializeVideoMode();

	sei();
	
	DisplayLogo();
	
}

void ReadButtons(){
	unsigned int p1ButtonsLo=0,p2ButtonsLo=0;
	unsigned char i;

	//latch controllers
	JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
	if(snesMouseEnabled){
		WaitUs(1);
	}else{
		Wait200ns();
	}	
	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));


	//read button states
	for(i=0;i<16;i++){
		
		p1ButtonsLo>>=1;
		p2ButtonsLo>>=1;
	
		//pulse clock pin		
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
		if(snesMouseEnabled){
			WaitUs(5);
		}else{
			Wait200ns();
		}
		
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsLo|=(1<<15);
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsLo|=(1<<15);
		
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		if(snesMouseEnabled){
			WaitUs(5);
		}else{
			Wait200ns();
		}

	}

	joypad1_status_lo=p1ButtonsLo;
	joypad2_status_lo=p2ButtonsLo;

}

void ReadControllers(){
	unsigned int p1ButtonsHi=0,p2ButtonsHi=0;
	unsigned char i;

	//read the standard buttons
	ReadButtons();

	//read the extended bits. Applies only if the mouse is plugged.
	//if bit 15 of standard word is 1, a mouse is plugged.
	if(joypad1_status_lo&(1<<15) || joypad2_status_lo&(1<<15)){

		WaitUs(1);

		for(i=0;i<16;i++){
		
			p1ButtonsHi<<=1;
			p2ButtonsHi<<=1;
	
			//pulse clock pin		
			JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
			Wait200ns();
			Wait200ns();
		
			if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsHi|=1;
			if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsHi|=1;

			JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
			WaitUs(8);
		}
		
		joypad1_status_hi=p1ButtonsHi;
		joypad2_status_hi=p2ButtonsHi;

	}

}


/*
 This method activates teh code to read the mouse. 
 Currently reading the mouse takes a much a 2.5 scanlines.
*/
unsigned char playDevice=0,playPort=0,mouseSpriteIndex,mouseWidth,mouseHeight;
unsigned int actionButton;
int mx=0,my=0;

char EnableSnesMouse(unsigned char spriteIndex,const char *spriteMap){
	snesMouseEnabled=true;
	if(DetectControllers()!=0){
		mouseWidth=pgm_read_byte(&(spriteMap[0]));
		mouseHeight=pgm_read_byte(&(spriteMap[1]));

		mx=120;
		my=120;
		mouseSpriteIndex=spriteIndex;
		MapSprite(spriteIndex,spriteMap);
		MoveSprite(spriteIndex,mx,my,mouseWidth,mouseHeight);
		return 0;
	}else{
		snesMouseEnabled=false;
		return -1;
	}

}

unsigned char GetMouseX(){
	return mx;
}

unsigned char GetMouseY(){
	return my;
}

unsigned int GetActionButton(){
	return actionButton;
}

unsigned char GetMouseSensitivity(){
	unsigned char sens=-1;

	if(snesMouseEnabled){
		ReadButtons();

		if(joypad1_status_lo&(1<<15)){
			sens=(joypad1_status_lo>>10)&3;
		}else if(joypad2_status_lo&(1<<15)){
			sens=(joypad2_status_lo>>10)&3;
		}

	}

	return sens;
}

bool SetMouseSensitivity(unsigned char value){
	unsigned char i,retries=6;

	if(snesMouseEnabled){	
		while(retries>0){
			
			if(GetMouseSensitivity()==value){
				return true;
			}

			WaitUs(1000);

			for(i=0;i<31;i++){	
				JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);	

				//pulse clock pin		
				JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
				Wait200ns();
				Wait200ns();
				Wait200ns();			
				Wait100ns();			
				JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);

				JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));	
			
				WaitUs(2);
				Wait200ns();
				Wait200ns();
				Wait200ns();			
				Wait100ns();
			}	
			
			retries++;
		}
	}

	return false;
}





void ProcessMouseMovement(void){
	unsigned int joy;
	
	if(snesMouseEnabled){

		//check in case its a SNES pad

		if(playDevice==0){
			joy=ReadJoypad(playPort);

			if(joy&BTN_LEFT){
				mx-=2;
				if(mx<0) mx=0; 
			}
			if(joy&BTN_RIGHT){
				mx+=2;
				if(mx>231) mx=231;
			}
			if(joy&BTN_UP){
				my-=2;
				if(my<0)my=0;
			}
			if(joy&BTN_DOWN){
				my+=2;
				if(my>215)my=215;
			}

		}else{
	
			joy=ReadJoypadExt(playPort);

			if(joy&0x80){
				mx-=(joy&0x7f);
				if(mx<0) mx=0; 
			}else{
				mx+=(joy&0x7f);
				if(mx>231) mx=231;
			}
	
			if(joy&0x8000){
				my-=((joy>>8)&0x7f);
				if(my<0)my=0;
			}else{
				my+=((joy>>8)&0x7f);
				if(my>215)my=215;
			}
	
		}

		#if SPRITES_ENABLED !=0
			MoveSprite(mouseSpriteIndex,mx,my,mouseWidth,mouseHeight);
		#endif
	}
}


/*
 * Returns: 0=controller in P1
 *          1=mouse in P1
 */
unsigned char DetectControllers(){
	unsigned int joy;
	unsigned char resp=0;
	//wait a frame for mouse to settle
	WaitVsync(8);
	joy=ReadJoypad(0);
	if((joy&0x8000)!=0){
		//we have a mouse in player 1 port
		playDevice=1;
		playPort=0;
		actionButton=BTN_MOUSE_LEFT;
		resp=1;
		return resp;
	}else{
		playDevice=0;
		playPort=0;
		actionButton=BTN_A;
	}

	joy=ReadJoypad(1);
	if((joy&0x8000)!=0){
		//we have a mouse in player 2 port
		playDevice=1;
		playPort=1;
		actionButton=BTN_MOUSE_LEFT;
		resp=1<<4;
	}

	return resp;
}


/*
void DetectControllers(){
	unsigned int joy;
	//wait a frame for mouse to settle
	WaitVsync(4);
	joy=ReadJoypad(0);
	if((joy&0x8000)!=0){
		//we have a mouse in player 1 port
		playDevice=1;
		playPort=0;
		actionButton=BTN_MOUSE_LEFT;
		return;
	}else{
		playDevice=0;
		playPort=0;
		actionButton=BTN_A;
	}

	joy=ReadJoypad(1);
	if((joy&0x8000)!=0){
		//we have a mouse in player 2 port
		playDevice=1;
		playPort=1;
		actionButton=BTN_MOUSE_LEFT;
	}
}
*/



/* Buttons stuff */

#define DBL_CLICK_DELAY 15

ButtonHandler btnHandlerPtr;
Button *_buttons;
unsigned char btnCount=0;
bool buttonsHandlerActive=false;

void createButton(Button *button, unsigned char x,unsigned char y,const char *normalMapPtr,const char *pushedMapPtr){	

	button->x=x;
	button->y=y;
	button->normalMapPtr=normalMapPtr;
	button->pushedMapPtr=pushedMapPtr;
	button->state=0;
	button->width=pgm_read_byte(&(normalMapPtr[0]));
	button->height=pgm_read_byte(&(normalMapPtr[1]));
	button->clicked=false;
	DrawMap2(x,y,normalMapPtr);

	btnCount++;	
}

void createAreaButton(Button *button, unsigned char x,unsigned char y,unsigned char width,unsigned char height){	

	button->x=x;
	button->y=y;
	button->width=width;
	button->height=height;
	button->normalMapPtr=NULL;
	button->pushedMapPtr=NULL;
	button->state=0;
	button->clicked=false;

	btnCount++;	
}

void registerButtonHandler(ButtonHandler fptr, Button *btns){
	btnHandlerPtr=fptr;
	_buttons=btns;
	buttonsHandlerActive=true;
}

void unregisterButtonHandler(){
	btnHandlerPtr=NULL;
	_buttons=NULL;
	buttonsHandlerActive=false;
}

static unsigned char debugNo2=0;
void debug2(char no){

	PrintHexByte(2+(no*3),3,debugNo2);

}


//char debug=0;
void processButtons(){
	unsigned char i,tx,ty;
	unsigned int joy=ReadJoypad(playPort);
	static unsigned int lastButtons=0;
	static unsigned char dblClickDelay=0;

	tx=mx>>3;
	ty=my>>3;

	for(i=0;i<btnCount;i++){

		if(tx>=_buttons[i].x && tx<(_buttons[i].x+_buttons[i].width) && ty>=_buttons[i].y && ty<(_buttons[i].y+_buttons[i].height)){		
		

			if(joy&actionButton && _buttons[i].state==BUTTON_UP){
				//button pushed
				if(_buttons[i].pushedMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].pushedMapPtr);				
				_buttons[i].state=BUTTON_DOWN;
				btnHandlerPtr(i,BUTTON_DOWN);					
				lastButtons=joy;	
				return;
			}
		
			if((joy&actionButton)==0 && _buttons[i].state==BUTTON_DOWN && _buttons[i].clicked==true){
				//button clicked
				if(_buttons[i].normalMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].normalMapPtr);
				_buttons[i].state=BUTTON_UP;
				_buttons[i].clicked=false;				
				btnHandlerPtr(i,BUTTON_DBLCLICK);					
				lastButtons=joy;	
				return;
			}
	
			if((joy&actionButton)==0 && _buttons[i].state==BUTTON_DOWN && !_buttons[i].clicked){
				//button clicked
				if(_buttons[i].normalMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].normalMapPtr);
				_buttons[i].state=BUTTON_UP;
				_buttons[i].clicked=true;				
				btnHandlerPtr(i,BUTTON_CLICK);					
				lastButtons=joy;
				dblClickDelay=0;	
				return;
			}			
	


			if(dblClickDelay==DBL_CLICK_DELAY && _buttons[i].clicked==true){
				//btnHandlerPtr(i,BUTTON_CLICK);
				_buttons[i].clicked=false;
				lastButtons=joy;	
				return;
			}
		
		}else{
		
			if((joy&actionButton)==0 && _buttons[i].state==BUTTON_DOWN){
				//button released outside of button
				if(_buttons[i].normalMapPtr!=NULL) DrawMap2(_buttons[i].x,_buttons[i].y,_buttons[i].normalMapPtr);
				_buttons[i].state=BUTTON_UP;
				btnHandlerPtr(i,BUTTON_UP);					
				lastButtons=joy;	
				return;
			}
		

		}

	

	}

	dblClickDelay++;
	lastButtons=joy;
}






//returns true if the EEPROM has been setup to work with the kernel.
bool isEepromFormatted(){
	unsigned id;
	id=ReadEeprom(0)+(ReadEeprom(1)<<8);
	return (id==EEPROM_SIGNATURE);
}

/*
 * Write a data block in the specified block id. If the block does not exist, it is created.
 *
 * Returns: 0 on success or error codes
 */
char EepromWriteBlock(struct EepromBlockStruct *block){
	unsigned char i,nextFreeBlock=0,c;
	unsigned int destAddr=0,id;
	unsigned char *srcPtr=(unsigned char *)block;

	if(!isEepromFormatted()) return EEPROM_ERROR_NOT_FORMATTED;
	if(block->id==EEPROM_FREE_BLOCK || block->id==EEPROM_SIGNATURE) return EEPROM_ERROR_INVALID_BLOCK;

	//scan all blocks and get the adress of that block or the next free one.
	for(i=2;i<64;i++){
		id=ReadEeprom(i*EEPROM_BLOCK_SIZE)+(ReadEeprom((i*EEPROM_BLOCK_SIZE)+1)<<8);
		if(id==block->id){
			destAddr=i*EEPROM_BLOCK_SIZE;
			break;
		}
		if(id==0xffff && nextFreeBlock==0) nextFreeBlock=i;
	}

	if(destAddr==0 && nextFreeBlock==0) return EEPROM_ERROR_FULL;
	if(nextFreeBlock!=0) destAddr=nextFreeBlock*EEPROM_BLOCK_SIZE;

	for(i=0;i<EEPROM_BLOCK_SIZE;i++){
		c=*srcPtr;
		WriteEeprom(destAddr++,c);
		srcPtr++;	
	}
	
	return 0;
}

/*
 * Reads a data block in the specified structure.
 *
 * Returns: 0 on success or error codes
 */
char EepromReadBlock(unsigned int blockId,struct EepromBlockStruct *block){
	unsigned char i;
	unsigned int destAddr=0,id;
	unsigned char *destPtr=(unsigned char *)block;

	if(!isEepromFormatted()) return EEPROM_ERROR_NOT_FORMATTED;
	if(blockId==EEPROM_FREE_BLOCK) return EEPROM_ERROR_INVALID_BLOCK;

	//scan all blocks and get the adress of that block
	for(i=0;i<32;i++){
		id=ReadEeprom(i*EEPROM_BLOCK_SIZE)+(ReadEeprom((i*EEPROM_BLOCK_SIZE)+1)<<8);
		if(id==blockId){
			destAddr=i*EEPROM_BLOCK_SIZE;
			break;
		}
	}

	if(destAddr==0) return EEPROM_ERROR_BLOCK_NOT_FOUND;			

	for(i=0;i<EEPROM_BLOCK_SIZE;i++){
		*destPtr=ReadEeprom(destAddr++);
		destPtr++;	
	}
	
	return 0;
}


/*
 * UART Receive buffer function
 */
#if UART_RX_BUFFER == 1

	u8 uart_rx_buf_start;
	u8 uart_rx_buf_end;
	u8 uart_rx_buf[UART_RX_BUFFER_SIZE];

	void UartGoBack(unsigned char count){
		uart_rx_buf_start-=count;
		uart_rx_buf_start&=(UART_RX_BUFFER_SIZE-1);
	}

	unsigned char UartUnreadCount(){
		return (abs(uart_rx_buf_end-uart_rx_buf_start));
	}

	unsigned char UartReadChar(){
		unsigned char data=0;
		if(uart_rx_buf_end!=uart_rx_buf_start){
			data=uart_rx_buf[uart_rx_buf_start++];
			uart_rx_buf_start&=(UART_RX_BUFFER_SIZE-1);			
		}
		return data;
	}

	void UartInitRxBuffer(){
		uart_rx_buf_start=0;
		uart_rx_buf_end=0;
	}

#endif
