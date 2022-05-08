
/*
 * States.h
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */

#include "includes.h"


#ifndef STATES_H_
#define STATES_H_

extern char stateFlag;

void receivingState();
void dataPresentingState(char receivedArray[], char receivedCount);
void transmit_state(char retrieved_array[],char retrieved_array_count);
void Idle_state();
void retrieve_state();
void encrypt (char [] , int );
void decrypt (char [] , int );


#endif /* STATES_H_ */
