/*
 * includes.h
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "tm4c123gh6pm.h"

#define stateBusy       0
#define stateIdle       1


#define PUSHBUTTONS_UNPRESSED      1
#define PUSHBUTTON1_PRESSED        2
#define PUSHBUTTON2_PRESSED        3

#ifndef INCLUDES_H_
#define INCLUDES_H_



#endif /* INCLUDES_H_ */
