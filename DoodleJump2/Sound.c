// Sound.c
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jonathan Valvano
// 11/15/2021 
// Matias Sosa & Krishna Sura
// Last Modified: 06/28/2024
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "Sound.h"
#include "sounds/sounds.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"


uint32_t x; //index
void SysTick_IntArm(uint32_t period, uint32_t priority){
  // write this
    x = 0;
    SysTick->CTRL = 0;
    SysTick->LOAD = (period - 1);
    SCB->SHP[1] = (SCB->SHP[1]&(~0xC0000000)) | priority<<30; //Copied from textbook
    SysTick->VAL = 0;
    SysTick->CTRL = 0x07;
}
// initialize a 11kHz SysTick, however no sound should be started
// initialize any global variables
// Initialize the 5 bit DAC
void Sound_Init(void){
// write this
    SysTick_IntArm(1, 1);
    DAC5_Init();

  
}
//volatile uint32_t Count;
uint8_t soundPick;
void SysTick_Handler(void){ // called at 11 kHz
  // output one value to DAC if a sound is active
  /*GPIOB->DOUTTGL31_0 = (1<<27);
  GPIOB->DOUTTGL31_0 = (1<<27);
  Count++;
  GPIOB->DOUTTGL31_0 = (1<<27);*/
  /*GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 & (~0x0F)) | */
    if (soundPick == 1){
       Sound_Bounce();
    }
    if (soundPick == 2){
        Sound_Killed();
    }
    
}

//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the SysTick interrupt.
// It starts the sound, and the SysTick ISR does the output
// feel free to change the parameters
// Sound should play once and stop
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Start(uint8_t select, uint32_t count){
// write this
    SysTick->LOAD = (count - 1);
    SysTick->VAL = 0;
    soundPick = select;
    x = 0;


}
void Sound_Stop(){
    SysTick->LOAD = 0;
    x = 0;
}
void Sound_Bounce(void){
// write this
    if (x > 1402){
        Sound_Stop();
    }
    DAC5_Out(Pop[x]);
    x++;

  
}
void Sound_Killed(void){
// write this
    if (x > 4606){
            Sound_Stop();
        }
        DAC5_Out(Fall[x]);
        x++;
  
}
void Sound_Explosion(void){
// write this
 
}

void Sound_Fastinvader1(void){
  
}
void Sound_Fastinvader2(void){
  
}
void Sound_Fastinvader3(void){
  
}
void Sound_Fastinvader4(void){
  
}
void Sound_Highpitch(void){
  
}
