// MenuHandler.c
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
#include "GameEngine.h"
#include "Lab9Main.h"
#include "Graphics.h"
#include "MenuHandler.h"

#define UP (1<<24)
#define DOWN (1<<26)
#define LEFT (1<<25)
#define RIGHT (1<<27)

extern Entity_t Doodler;
extern Entity_t Platforms[NumOfPlatforms];
extern Entity_t PHatD;

extern uint32_t switchDataOld;
extern uint32_t switchData;

extern uint8_t MAINMENU;
extern uint8_t GAMESTART;
extern uint8_t PAUSE;
extern uint8_t GAMEINITDONE;
extern uint8_t ENGLISH;
extern uint8_t SPANISH;
extern uint8_t LANGSWITCH;
extern uint8_t OPTIONS;
extern uint8_t TRANSITION;
extern uint8_t GAMEEND;

extern uint32_t Score;

void MenuHandler()
{
    if(!GAMEEND)
    {
        DisplayDoodler();
        DisplayPlatforms();
    }
    if(MAINMENU && !OPTIONS && !GAMEEND) MainMenu();
    if(OPTIONS) Options();
    if(MAINMENU && GAMEEND) GameOver();
}

void MenuInit()
{
    DisplayTitle();
    DoodlerInit(&Doodler, &PHatD, 15, 80);
    MenuPlatformInit();
}

void MenuPlatformInit()
{
    Platforms[0].x = 10;
    Platforms[0].y = 140;
    Platforms[0].w = 27;
    Platforms[0].h = 7;
}

void MainMenu()
{
    DisplayTitle();
    if(((switchData & DOWN) == DOWN) && (switchDataOld == 0))
    {
        MAINMENU = 0;
        GAMESTART = 1;
        PAUSE = 0;
        TRANSITION = 1;
    }
    if(((switchData & UP) == UP) && (switchDataOld == 0))
    {
        OPTIONS = 1;
        TRANSITION = 1;
    }
}

void Options()
{
    DisplayOptions();
    if(((switchData & DOWN) == DOWN) && (switchDataOld == 0))
    {
        OPTIONS = 0;
        TRANSITION = 1;
    }
    if(((switchData & RIGHT) == RIGHT) && (switchDataOld == 0))
        {
            LangSwitch();
            TRANSITION = 1;
        }
}

void LangSwitch()
{
    if(ENGLISH)
    {
        ENGLISH = 0;
        SPANISH = 1;
    }
    else if(SPANISH)
    {
        ENGLISH = 1;
        SPANISH = 0;
    }
}

void GameOver()
{
    DisplayGameOver();
    if(((switchData & DOWN) == DOWN) && (switchDataOld == 0))
    {
        GAMEINITDONE = 0;
        GAMEEND = 0;
        TRANSITION = 1;
        MenuInit();
        TRANSITION = 1;
        Score = 0;
    }
}

