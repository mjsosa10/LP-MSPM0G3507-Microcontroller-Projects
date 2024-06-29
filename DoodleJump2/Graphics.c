// Graphics.c
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
#include "images/images.h"
#include "Lab9Main.h"

extern Entity_t Platforms[NumOfPlatforms];
extern Entity_t Doodler;
extern Entity_t PHatP;
extern Entity_t PHatD;

extern uint8_t MAINMENU;
extern uint8_t GAMESTART;
extern uint8_t PAUSE;
extern uint8_t GAMEINITDONE;
extern uint8_t ENGLISH;
extern uint8_t SPANISH;
extern uint8_t TRANSITION;

extern uint32_t Score;

void DisplayPlatforms()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        ST7735_DrawBitmap(Platforms[i].xOld, Platforms[i].yOld, PlatformBlank, Platforms[i].w, Platforms[i].h);
        ST7735_DrawBitmap(Platforms[i].x, Platforms[i].y, PlatformGreen, Platforms[i].w, Platforms[i].h);

        if((Platforms[i].hasPowerUp == 1) && (PHatP.visible == 1))
        {
            ST7735_DrawBitmap(PHatP.xOld, PHatP.yOld, PHatBlank, PHatP.w, PHatP.h);
            ST7735_DrawBitmap(PHatP.x, PHatP.y, PHat, PHatP.w, PHatP.h);
        }
    }
}

void DisplayDoodler()
{
    ST7735_DrawBitmap(Doodler.xOld, Doodler.yOld, DoodlerBlank, Doodler.w, Doodler.h);
    ST7735_DrawBitmap(Doodler.x, Doodler.y, DoodlerRight, Doodler.w, Doodler.h);

    if((Doodler.hasPowerUp == 1) && (PHatD.visible == 1))
            {
                ST7735_DrawBitmap(PHatD.xOld, PHatD.yOld, PHatBlank, PHatD.w, PHatD.h);
                ST7735_DrawBitmap(PHatD.x, PHatD.y, PHat, PHatD.w, PHatD.h);
            }
}

void DisplayTitle()
{
    if(TRANSITION)
    {
        ST7735_FillScreen(ST7735_WHITE);
        TRANSITION = 0;
    }
    ST7735_DrawBitmap(8, 42, title, 112, 32);
    if(ENGLISH)
    {
        ST7735_DrawBitmap(66, 80, playbutton, 56, 32);
        ST7735_DrawBitmap(56, 120, optionsbutton, 56, 32);
    }
    if(SPANISH)
    {
        ST7735_DrawBitmap(66, 80, playbuttonspanish, 56, 32);
        ST7735_DrawBitmap(56, 120, optionsbuttonspanish, 56, 32);
    }

}

void DisplayMenuPlatform()
{
    ST7735_DrawBitmap(Platforms[0].x, Platforms[0].y, PlatformGreen, Platforms[0].w, Platforms[0].h);
}

void ClearMenu()
{
    ST7735_FillScreen(ST7735_WHITE);
}

void DisplayOptions()
{
    if(TRANSITION)
    {
        ClearMenu();
        TRANSITION = 0;
    }
    ST7735_SetTextColor(ST7735_BLACK);
    ST7735_SetCursor(10, 7);
    if(ENGLISH)
    {
        ST7735_OutString("LANGUAGE:");
        ST7735_SetCursor(10, 8);
        ST7735_SetTextColor(ST7735_RED);
        ST7735_OutString("ENG");
        ST7735_SetCursor(14, 8);
        ST7735_SetTextColor(ST7735_BLACK);
        ST7735_OutString("SPAN");
    }
    if(SPANISH)
    {
        ST7735_OutString("LENGUAGE:");
        ST7735_SetCursor(10, 8);
        ST7735_SetTextColor(ST7735_BLACK);
        ST7735_OutString("ING");
        ST7735_SetCursor(14, 8);
        ST7735_SetTextColor(ST7735_RED);
        ST7735_OutString("ESP");
    }
}

void DisplayScore()
{
    if(ENGLISH)
    {
        ST7735_DrawBitmap(36, 6, PlatformBlank, Platforms[0].w, Platforms[0].h);
        ST7735_DrawBitmap(63, 6, PlatformBlank, Platforms[0].w, Platforms[0].h);

        ST7735_SetTextColor(ST7735_BLACK);
        ST7735_SetCursor(0, 0);
        ST7735_OutString("SCORE:");
        ST7735_SetCursor(6, 0);
        //ST7735_OutString(Score);
        printf("%i", Score);
    }
    if(SPANISH)
    {
        ST7735_DrawBitmap(48, 6, PlatformBlank, Platforms[0].w, Platforms[0].h);
        ST7735_DrawBitmap(75, 6, PlatformBlank, Platforms[0].w, Platforms[0].h);

        ST7735_SetTextColor(ST7735_BLACK);
        ST7735_SetCursor(0, 0);
        ST7735_OutString("PUNTAJE:");
        ST7735_SetCursor(8, 0);
        //ST7735_OutString(Score);
        printf("%i", Score);
    }
}

void DisplayGameOver()
{
    if(ENGLISH)
    {
        ST7735_SetTextColor(ST7735_RED);
        ST7735_SetCursor(6, 6);
        ST7735_OutString("GAME OVER");
        ST7735_SetTextColor(ST7735_BLACK);
        ST7735_SetCursor(0, 10);
        ST7735_OutString("Press DOWN to return");
        ST7735_SetCursor(4, 11);
        ST7735_OutString(" to Main Menu");
    }
    if(SPANISH)
    {
        ST7735_SetTextColor(ST7735_RED);
        ST7735_SetCursor(3, 6);
        ST7735_OutString("JUEGO TERMINADO");
        ST7735_SetTextColor(ST7735_BLACK);
        ST7735_SetCursor(0, 10);
        ST7735_OutString(" Presione ABAJO para");
        ST7735_SetCursor(2, 11);
        ST7735_OutString("regresar al men\x97");
        ST7735_SetCursor(5, 12);
        ST7735_OutString("principal");
    }
}

void QuickErasePHat(Entity_t *hat)
{
    ST7735_DrawBitmap(hat->xOld, hat->yOld, PHatBlank, hat->w, hat->h);
    ST7735_DrawBitmap(hat->x, hat->y, PHatBlank, hat->w, hat->h);
}
