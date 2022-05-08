/*
 * CAN_Functions.h
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */
#include "States.h"

#ifndef CAN_FUNCTIONS_H_
#define CAN_FUNCTIONS_H_

void CAN_Init();
void CAN_Receive_Data(int receivedValue);
void CAN_Receive_Size(char *receivedValue);
void CANIntHandler(void);
void init_MessageObj_data_size();
void init_MessageObj_data();
void init_MessageObj_State();
void canSendState(char);
void can_send_size(char);
void can_send_data(char*);
void CAN_Receive_State(char *);
#endif /* CAN_FUNCTIONS_H_ */
