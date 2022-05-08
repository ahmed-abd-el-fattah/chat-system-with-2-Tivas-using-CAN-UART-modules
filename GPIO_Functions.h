/*
 * GPIO_Functions.h
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */
#include "States.h"
#include "includes.h"
#ifndef GPIO_FUNCTIONS_H_
#define GPIO_FUNCTIONS_H_

void GPIOF_Init();
char button_status();
void state_red();
void state_blue();
void state_green();


#endif /* GPIO_FUNCTIONS_H_ */
