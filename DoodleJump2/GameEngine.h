// GameEngine.h
// Matias Sosa & Krishna Sura
// Last Modified: 06/28/2024

#ifndef GAMEENGINE_H_
#define GAMEENGINE_H_

struct Entity
{
    uint16_t x;  //x position 0-128
    uint16_t y;  //y position 0-160
    int16_t vx;  //x velocity
    int16_t vy;  //y velocity
    int16_t ax;  //x acceleration
    int16_t ay;  //y acceleration
    uint8_t health; //for if we have bad guys
    uint8_t visible;    //1 for visible, 0 for invisible
    uint8_t h, w;    //height and width of sprite
    uint16_t xOld, yOld;
    uint16_t FIXEDPTy;
    int16_t storedvx;
    int16_t storedvy;
    int16_t storeday;
    uint8_t hasPowerUp; //for platforms

};
typedef struct Entity Entity_t;

uint32_t GameConvert(uint32_t input);

void DoodlerInit(Entity_t *doodler, Entity_t *hat, uint16_t x, uint16_t y);

void PHatDInit(Entity_t *doodler, Entity_t *hat);

void PHatPInit(Entity_t *hat);

void PlatformsInit();

void UpdateDoodlerPosition(Entity_t *doodler);

void UpdateDoodlerSpeed(Entity_t *doodler);

void SetOldPosition(Entity_t *entity);

void CheckForCollision(Entity_t *doodler);

void UpdatePlatforms();

void AddPHat(uint8_t index);

void UpdateScore();

void RemovePlatforms();

void RemoveDoodler(Entity_t *doodler);

void GameEnd(Entity_t *doodler);

void CheckForPause(Entity_t *doodler);

void CheckforUnpause(Entity_t *doodler);

void FreezeDoodler(Entity_t *doodler);

void UnFreezeDoodler(Entity_t *doodler);

#endif
