 /*
 * LED.c
 *
 *  Created on: Nov 5, 2023
 *  Author: MATIAS SOSA
 */
#include <ti/devices/msp/msp.h>
#include "../inc/LaunchPad.h"

#define RED (1<<17)
#define YELLOW (1<<28) //Info
#define GREEN (1<<31)


// LaunchPad.h defines all the indices into the PINCM table

// initialize your LEDs
void LED_Init(void){
    IOMUX->SECCFG.PINCM[PA17INDEX]  = 0x0000081;
    IOMUX->SECCFG.PINCM[PA28INDEX]  = 0x0000081;
    IOMUX->SECCFG.PINCM[PA31INDEX]  = 0x0000081;
    GPIOA->DOESET31_0 = RED | YELLOW | GREEN;
    
}
// data specifies which LED to turn on
void LED_On(uint32_t data){
    // use DOUTSET31_0 register so it does not interfere with other GPIO
    GPIOA->DOUTSET31_0 = data;
}

// data specifies which LED to turn off
void LED_Off(uint32_t data){
    // use DOUTCLR31_0 register so it does not interfere with other GPIO
    GPIOA->DOUTCLR31_0 = data;
}

// data specifies which LED to toggle
void LED_Toggle(uint32_t data){
    // use DOUTTGL31_0 register so it does not interfere with other GPIO
    GPIOA->DOUTTGL31_0 = data;
}
