// GameEngine.c
// Matias Sosa & Krishna Sura
// Last Modified: 06/28/2024

#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
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

#define RED (1 << 17)
#define YELLOW (1 << 28)
#define GREEN (1 << 31)

#define UP (1<<24)
#define DOWN (1<<26)
#define LEFT (1<<25)
#define RIGHT (1<<27)

extern uint32_t ADCData;
extern Entity_t Platforms[NumOfPlatforms];
extern Entity_t PHatP;
extern Entity_t PHatD;
extern Entity_t Doodler;
extern uint32_t switchDataOld;
extern uint32_t switchData;

extern uint8_t GAMEEND;
extern uint8_t MAINMENU;
extern uint8_t GAMESTART;
extern uint8_t PAUSE;
extern uint8_t STORE;

uint32_t Score = 0;

uint8_t PlatformCollisionFlag = 0;
uint8_t GrabPowerUpFlag = 0;
uint8_t ResumeDoodlerMovement = 0;
uint8_t sameYCnt = 0;

uint32_t GameConvert(uint32_t input)    //Takes input values from slidepot and converts them to values usable to the game
{
    int32_t val;
    val = input >> 9;
    val -= 4;
    if(val < 0) val++;
    return val*2;
}

void DoodlerInit(Entity_t *doodler, Entity_t *hat, uint16_t x, uint16_t y)  //initializes the Doodler (alien thing)
{
    doodler->x = x;
    doodler->y = y;    //90 spaces between platform and doodler head
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ax = 0;
    doodler->ay = 4;
    doodler->visible = 1;
    doodler->w = 19;
    doodler->h = 21;
    doodler->xOld = doodler->x;
    doodler->yOld = doodler->y;
    doodler->FIXEDPTy = (doodler->y)*10;
    PHatDInit(doodler, hat);
}


void PHatDInit(Entity_t *doodler, Entity_t *hat)    //initializes the propeller hat on the Doodler's head
{
    hat->x = doodler->x + 2;
    hat->y = doodler->y - 21;
    hat->visible = 0;
    hat->w = 10;
    hat->h = 7;
    hat->xOld = hat->x;
    hat->yOld = hat->y;
}

void PHatPInit(Entity_t *hat)   //initializes the propeller hat on the platforms
{
    hat->x = 0;
    hat->y = 0;
    hat->visible = 0;
    hat->w = 10;
    hat->h = 7;
    hat->xOld = hat->x;
    hat->yOld = hat->y;
}

void PlatformsInit()    //initializes an amount of platforms depended on what "NumOfPlatforms" is set to
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].x = Random(101);
        Platforms[i].y = Random(160);
        //Platforms[i].x = 55;    //for debug
        //Platforms[i].y = 160;   //for debug - 153 is max for contact
        Platforms[i].w = 27;
        Platforms[i].h = 7;
        Platforms[i].vy = 0;
        Platforms[i].ay = 0;
        Platforms[i].FIXEDPTy = (Platforms[i].y)*10;

        //Here I am comparing the location of the current platform to previously created platforms and adjusting the location accordingly
        if(i != 0)  //If I just made the first platform, there are no other platforms to compare it to
        {
            for(uint8_t j = 0; j < NumOfPlatforms; j++)
            {
                if(i != j)
                {
                    int16_t xDiff = Platforms[i].x - Platforms[j].x;
                    int16_t yDiff = Platforms[i].y - Platforms[j].y;
                    if(xDiff < 0) xDiff *= -1;
                    if(yDiff < 0) yDiff *= -1;

                    while((xDiff < 35) && (yDiff < 21))
                    {
                        Platforms[i].x = Random(101);
                        Platforms[i].y = Random(160);
                        xDiff = Platforms[i].x - Platforms[j].x;
                        yDiff = Platforms[i].y - Platforms[j].y;
                        if(xDiff < 0) xDiff *= -1;
                        if(yDiff < 0) yDiff *= -1;
                    }

                    Platforms[i].FIXEDPTy = (Platforms[i].y)*10;
                }


            }
        }
    }
}

void UpdateDoodlerPosition(Entity_t *doodler)   //updates doodler stats (position, velocity)
{
    SetOldPosition(doodler);

    int16_t xtemp = doodler->x + doodler->vx;    //x position + x velocity

    //for screen wrap
    if(xtemp < 0) doodler->x = 109;
    else if(xtemp > 109) doodler->x = 0;
    else doodler->x = xtemp;

    doodler->vy = doodler->vy + doodler->ay;    //y velocity += y acceleration
    doodler->FIXEDPTy = doodler->FIXEDPTy + doodler->vy;   //y position += y velocity

    doodler->y = (doodler->FIXEDPTy)/10;

    if(doodler->hasPowerUp == 1)
    {
        PHatD.xOld = PHatD.x;
        PHatD.yOld = PHatD.y;
        PHatD.x = doodler->x + 2;
        PHatD.y = doodler->y - 21;
    }

    if((doodler->FIXEDPTy > 1650) && (doodler->FIXEDPTy < 60000)) GameEnd(doodler);

}

void UpdateDoodlerSpeed(Entity_t *doodler)
{
    doodler->vx = GameConvert(ADCData);
}

void SetOldPosition(Entity_t *entity)
{
    entity->xOld = entity->x;
    entity->yOld = entity->y;
}

void CheckForCollision(Entity_t *doodler)   //checks for collision between Doodler and any of the platforms
{
    int16_t x = doodler->x;
    int16_t y = doodler->y;
    if(doodler->vy > 0)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            int16_t xDiff = doodler->x - Platforms[i].x;
            if((xDiff < 27) && (xDiff > -19))
            {
                if((Platforms[i].y > doodler->y) && (doodler->y > (Platforms[i].y - 8)))
                {
                    Sound_Start(1, 3500);   //Pop sound
                    doodler->vy = -55;
                    PlatformCollisionFlag = 1;

                    if(Platforms[i].hasPowerUp == 1)
                    {
                        GrabPowerUpFlag = 1;
                        doodler->hasPowerUp = 1;
                        PHatD.visible = 1;

                        doodler->storedvy = doodler->vy;
                        doodler->vy = 0;
                        doodler->ay = 0;
                    }

                }
            }
        }
     }
}


void UpdatePlatforms()  /*updates stats of all platforms (position, velocity, whether they have propeller hats or not) as well as
gives them a new position at the top of the screen once a platform goes below the screen*/
{

    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        if(Doodler.hasPowerUp == 1) //if the player has a propeller hat, make the platforms accelerate faster
        {
            Platforms[i].ay = 10;
        }
        Platforms[i].xOld = Platforms[i].x;
        Platforms[i].yOld = Platforms[i].y;
        Platforms[i].vy = Platforms[i].vy + Platforms[i].ay;
        Platforms[i].FIXEDPTy = Platforms[i].FIXEDPTy + Platforms[i].vy;
        Platforms[i].y = (Platforms[i].FIXEDPTy)/10;

        //if a platform has a propeller hat and the player grabs it, erase the propeller hat on the platform
        if((GrabPowerUpFlag == 1) && (Platforms[i].hasPowerUp == 1))
        {
            QuickErasePHat(&PHatP); //I know I shouldn't display in interrupt but it is a small and periodic sprite and easier this way
            Platforms[i].hasPowerUp = 0;
            PHatP.visible = 0;
            PHatP.x = 0;
            PHatP.y = 0;
            GrabPowerUpFlag = 0;
        }

        //if a platform has a propeller hat, update the position of the hat
        if(Platforms[i].hasPowerUp == 1)
        {
            PHatP.xOld = PHatP.x;
            PHatP.yOld = PHatP.y;
            PHatP.x = Platforms[i].x + 8;
            PHatP.y = Platforms[i].y - 7;
        }

        //if a platform goes below the bottom of the screen
        if(Platforms[i].FIXEDPTy > 1650)
        {
            Platforms[i].FIXEDPTy = 0;
            Platforms[i].y = 0;
            Platforms[i].x = Random(101);

            for(uint8_t j = 0; j < NumOfPlatforms; j++)
                        {
                            if(i != j)
                            {
                                int16_t xDiff = Platforms[i].x - Platforms[j].x;
                                int16_t yDiff = Platforms[i].y - Platforms[j].y;
                                if(xDiff < 0) xDiff *= -1;
                                if(yDiff < 0) yDiff *= -1;


                                if(yDiff < 30)
                                {
                                    if(sameYCnt < 2)
                                    {
                                        while(xDiff < 30)
                                        {
                                            Platforms[i].x = Random(101);
                                            xDiff = Platforms[i].x - Platforms[j].x;
                                            if(xDiff < 0) xDiff *= -1;
                                        }
                                    }
                                    else
                                    {
                                        Platforms[i].FIXEDPTy = 1700;
                                        Platforms[i].y = 1700;
                                    }
                                }


                                if(yDiff < 30) sameYCnt++;

                                if(yDiff >= 30) sameYCnt = 0;
                            }
                        }
            sameYCnt = 0;

            if(Platforms[i].hasPowerUp == 1)
            {
                QuickErasePHat(&PHatP);   //I know I shouldn't display in interrupt but it is a small and periodic sprite and easier this way
                Platforms[i].hasPowerUp = 0;
                PHatP.visible = 0;
                PHatP.x = 0;
                PHatP.y = 0;
            }

            if(PHatP.visible == 0) AddPHat(i);
        }

    }


    //if the Doodler collides with a platform, make them all move down (to make it seem the Doodler is moving up)
    if(PlatformCollisionFlag == 1)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            Platforms[i].vy = 120;
            Platforms[i].ay = -7;
        }
        PlatformCollisionFlag = 0;
    }


    //once a platform reaches zero velocity after accelerating downwards, make all platforms stop
    if(Platforms[0].vy <= 0)
    {
        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            Platforms[i].ay = 0;
            Platforms[i].vy = 0;
        }

        if(ResumeDoodlerMovement == 1)  //after power up is done this needs to be reset
        {
            ResumeDoodlerMovement = 0;
            Doodler.vy = Doodler.storedvy;
            Doodler.ay = 4;
        }

    }


    UpdateScore();

}

//makes a propeller hate appear on a platform at a random moment
void AddPHat(uint8_t index)
{
    uint16_t tempRandom = Random(1000);
    if(tempRandom < 10)
    {
        PHatP.x = Platforms[index].x + 8;
        PHatP.y = Platforms[index].x - 7;
        PHatP.visible = 1;
        Platforms[index].hasPowerUp = 1;
    }
}

void UpdateScore()
{
    if(Platforms[0].vy >= 0)
    {
        Score += (Platforms[0].vy / 10);
    }
}

void RemovePlatforms()
{
    for(uint8_t i = 0; i < NumOfPlatforms; i++)
    {
        Platforms[i].x = 0;
        Platforms[i].y = 0;
        Platforms[i].w = 27;
        Platforms[i].h = 7;
        Platforms[i].vy = 0;
        Platforms[i].ay = 0;
        Platforms[i].FIXEDPTy = (Platforms[i].y)*10;
    }
}

void RemoveDoodler(Entity_t *doodler)
{
    doodler->x = 0;
    doodler->y = 0;    //90 spaces between platform and doodler head
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ax = 0;
    doodler->ay = 0;
    doodler->visible = 1;
    doodler->xOld = doodler->x;
    doodler->yOld = doodler->y;
    doodler->FIXEDPTy = (doodler->y)*10;
}

void GameEnd(Entity_t *doodler)
{
    Sound_Start(2, 5000);   //Fall sound

    GAMESTART = 0;
    MAINMENU = 1;
    GAMEEND = 1;

    RemovePlatforms();
    RemoveDoodler(doodler);
}

void CheckForPause(Entity_t *doodler)
{
    if(STORE == 1)
    {
        UnFreezeDoodler(doodler);
        STORE = 0;
    }

    if(((switchData & UP) == UP) && (switchDataOld == 0))
    {
        PAUSE = 1;
        STORE = 1;
    }
}

void CheckforUnpause(Entity_t *doodler)
{
    if(STORE == 1)
    {
        FreezeDoodler(doodler);
        STORE = 0;
    }

    if(((switchData & UP) == UP) && (switchDataOld == 0))
        {
            PAUSE = 0;
            STORE = 1;
        }
}

void FreezeDoodler(Entity_t *doodler)
{
    doodler->storedvx = doodler->vx;
    doodler->storedvy = doodler->vy;
    doodler->storeday = doodler->ay;
    doodler->vx = 0;
    doodler->vy = 0;
    doodler->ay = 0;
}

void UnFreezeDoodler(Entity_t *doodler)
{
    doodler->vx = doodler->storedvx;
    doodler->vy = doodler->storedvy;
    doodler->ay = doodler->storeday;
}

void CheckForEndOfPowerUp()
{
    static uint16_t PowerUpCount;
    if(Doodler.hasPowerUp)
    {
        PowerUpCount++;
    }
    if(PowerUpCount > 10)      //65000
    {
        PowerUpCount = 0;

        Doodler.hasPowerUp = 0;
        PHatD.visible = 0;

        for(uint8_t i = 0; i < NumOfPlatforms; i++)
        {
            Platforms[i].ay = -10;
        }

        ResumeDoodlerMovement = 1;
    }
}

