#include "includes.h"

/*
 * GPIO_Functions.c
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */

// initialization of GPIO port F
void GPIOF_Init()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);            // enable the clock for the Port
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}        //wait for the clock to be ready
    GPIO_PORTF_LOCK_R = 0x4c4f434b;
    GPIO_PORTF_CR_R = 0x01f;
    GPIO_PORTF_DIR_R=0xE;
    GPIO_PORTF_DEN_R=0x1f;
    GPIO_PORTF_PUR_R=0x11;
}

//A Function that Gets the status of the Pushbuttons
char button_status()
{
    char status = PUSHBUTTONS_UNPRESSED;            // set the state of the Button as Unpressed
    if ((GPIO_PORTF_DATA_R  & 0x1) == 0 )           // iF pushbutton 1 is pressed
    {
        status = PUSHBUTTON1_PRESSED;               // Set the status as PB1 Pressed
    }

    if ((GPIO_PORTF_DATA_R  & 0x10) == 0 )          // iF pushbutton 2 is pressed
    {
        status = PUSHBUTTON2_PRESSED;               // Set the status as PB1 Pressed
    }

    return status;
}

// Turn ON the Red LED
void state_red()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x02);

}

// Turn ON the GREEN LED
void state_green()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x8);

}

// Turn ON the BLUE LED
void state_blue()
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x4);
}

