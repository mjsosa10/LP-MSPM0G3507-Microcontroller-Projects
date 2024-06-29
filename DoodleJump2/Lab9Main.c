// Lab9Main.c
// Runs on MSPM0G3507
// Matias Sosa & Krishna Sura
// Last Modified: 06/28/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/ADC1.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
//#include "images/images.h"
#include "GameEngine.h"
#include "Graphics.h"
#include "Lab9Main.h"
#include "MenuHandler.h"

#define UP (1<<24)
#define DOWN (1<<26)
#define LEFT (1<<25)
#define RIGHT (1<<27)

#define RED (1<<17)
#define YELLOW (1<<28) //Info
#define GREEN (1<<31)

uint32_t ADCData;
uint32_t Flag;  // semaphore
uint32_t switchDataOld;
uint32_t switchData;

Entity_t Doodler;
Entity_t Platforms[NumOfPlatforms];
Entity_t PHatD;
Entity_t PHatP;

uint8_t MAINMENU = 1;
uint8_t GAMESTART;
uint8_t PAUSE;
uint8_t GAMEINITDONE;
uint8_t ENGLISH = 1;
uint8_t SPANISH;
uint8_t LANGSWITCH;
uint8_t OPTIONS;
uint8_t TRANSITION = 1;
uint8_t GAMEEND;
uint8_t STORE;

extern uint32_t Score;

// ****note to students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

uint32_t M=1;

void Random_Init(uint32_t val)
{
    M = val;
}

uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

void SysTick_Start(uint32_t period){
  // write this
  // set reload value
  // write any value to VAL, cause reload
    SysTick->LOAD = (period - 1);
    SysTick->VAL = 0;
}


// games engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites
    // 4) start sounds
    // 5) set semaphore
    if(GAMESTART && !PAUSE)
    {
        ADCData = ADCin();
        switchDataOld = switchData;
        switchData = Switch_In();
        UpdateDoodlerSpeed(&Doodler);
        UpdateDoodlerPosition(&Doodler);
        CheckForCollision(&Doodler);
        UpdatePlatforms();
        CheckForPause(&Doodler);
        CheckForEndOfPowerUp();
    }
    if(MAINMENU)
    {
        UpdateDoodlerPosition(&Doodler);
        CheckForCollision(&Doodler);

        switchDataOld = switchData;
        switchData = Switch_In();
    }
    if(PAUSE)
    {
        switchDataOld = switchData;
        switchData = Switch_In();
        CheckforUnpause(&Doodler);
    }

    Flag = 1;
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}



// ALL ST7735 OUTPUT MUST OCCUR IN MAIN
  int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ADCinit();     //PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  Sound_Init();  // initialize sound
  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    // initialize interrupts on TimerG12 at 30 Hz
  TimerG12_IntArm(80000000/30,2);
  // initialize all data structures
  __enable_irq();
  //Initialize SysTick
  //SysTick_Init();
  //SysTick_Start(4294967295);

  Random_Init(SysTick->VAL);

  MenuInit();

  while(1){
      // wait for semaphore
       // clear semaphore
       // update ST7735R
    // check for end game or level switch
      if(Flag)
      {
          if(GAMESTART && !PAUSE)
          {                                   // If game has started run this
              if(!GAMEINITDONE)
              {
                  ClearMenu();
                  DoodlerInit(&Doodler, &PHatD, 60, 80);
                  PlatformsInit();
                  PHatPInit(&PHatP);
                  Score = 0;
                  GAMEINITDONE = 1;
              }
              DisplayPlatforms();
              DisplayDoodler();
              DisplayScore();
          }
          else if(GAMESTART && PAUSE)
          {
              //if game is paused, do nothing
          }
          else if(MAINMENU) //game has not started, show menu
          {
              MenuHandler();                     // Handles menus
          }

          Flag = 0;
      }
  }
}
