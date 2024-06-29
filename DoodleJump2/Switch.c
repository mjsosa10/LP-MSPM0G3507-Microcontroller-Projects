/*
 * Switch.c
 *
 *  Created on: Nov 5, 2023
 *      Author: MATIAS SOSA
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"

#define UP (1<<24)
#define DOWN (1<<26)
#define LEFT (1<<25)
#define RIGHT (1<<27)

// LaunchPad.h defines all the indices into the PINCM table
void Switch_Init(void)
{
    IOMUX->SECCFG.PINCM[PA24INDEX] = (uint32_t) 0x00050081; // input, pull down
    IOMUX->SECCFG.PINCM[PA25INDEX] = (uint32_t) 0x00050081; // input, pull down
    IOMUX->SECCFG.PINCM[PA26INDEX] = (uint32_t) 0x00050081; // input, pull down
    IOMUX->SECCFG.PINCM[PA27INDEX] = (uint32_t) 0x00050081; // input, pull down
    IOMUX->SECCFG.PINCM[PB16INDEX] = (uint32_t) 0x00050081; // input, pull down
}
// return current state of switches
uint32_t Switch_In(void)
{
    return (GPIOA->DIN31_0 & (UP + DOWN + LEFT + RIGHT));
}
