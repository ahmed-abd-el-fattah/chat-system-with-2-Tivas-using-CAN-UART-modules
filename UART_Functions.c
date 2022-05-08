/*
 * UART_Functions.c
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */
#include "includes.h"

// Initialization of the UART
void UART_INIT()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    int x = SysCtlClockGet();
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
    UART_CONFIG_PAR_NONE));


    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    UARTFIFOEnable(UART0_BASE);
    UARTDisable(UART0_BASE);

}



