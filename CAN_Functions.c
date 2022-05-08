/*
 * CAN.c
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */
#include "includes.h"
#include "States.h"
#include "CAN_Functions.h"
/*
VARIABLES NEEDED BY THE CAN FUNCTIONS
 */

// Store the number of bytes recieved in the data retrieval state
char receiveBufferSize;

// Array to store upto 200 chars (bytes) to be used in the data presenting state
char receivedText[201];

// A variable to indicate the number of messages recieved in the transmission state
volatile uint32_t receivedMessageCount = 0;

// A flag for the interrupt handler to indicate that a message was received.
volatile bool g_bRXFlag = 0;

// A flag to indicate that some reception error occurred.
volatile bool g_bErrFlag = 0;

// A flag to indicate that recieved data is for DATA Size .
volatile bool g_sizeFlag = 0;

// A variable indicating the state of the MCU either Busy or IDLE to be sent to other MCU
char stateFlag = stateIdle;


// Recieve Message objects
tCANMsgObject sCANMessage;      // Object for the State Messages
tCANMsgObject tCANMessage;      // Object for the Size Messages
tCANMsgObject CANMessage;       // Object for the Data messages

uint8_t pui8MsgData[8];         // Buffer to store the 8 Bytes recieved from CANBUS
uint8_t MsgSize[8];             // Buffer to store the Size of data recieved from CANBUS
uint8_t MsgState[8];            // Buffer to store the State of the MCU recieved from the CANBUS



//*****************************************************************************
//
// Initialization of the CAN Controller
//
//*****************************************************************************

void CAN_Init(){
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 1000000);
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    IntEnable(INT_CAN0);
    CANEnable(CAN0_BASE);
    IntPrioritySet(INT_CAN0, 0x20);
}


//*****************************************************************************
//
// CAN Interrupt Handler
//
//*****************************************************************************
void CANIntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    if(ui32Status == CAN_INT_INTID_STATUS)
    {

        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        g_bErrFlag = 1;
    }
    if(ui32Status == 1)             // if the cause is receive Data object
    {
        CANIntClear(CAN0_BASE, 1);  // clear the interuppt
        g_bRXFlag = 1;              // raise a flag
        g_bErrFlag = 0;             // indicate no errors
        receivingState();           // go to the recieving state
    }
    if(ui32Status == 4)             // if CAN recieved the Size of the data
    {
        CANIntClear(CAN0_BASE, 4);
        g_bRXFlag = 1;
        g_bErrFlag = 0;
        g_sizeFlag = 1;             // raise the size flag to use it in the recieving state
        receivingState();           // go to the recieving state
    }
    if(ui32Status == 3)             // if the CAN recieved the state of other MCU
    {
        CANIntClear(CAN0_BASE, 3);
        g_bRXFlag = 1;
        g_bErrFlag = 0;
        CAN_Receive_State(&stateFlag);  // Save the State of the other MCU to use it as a token
    }

    if(ui32Status == 5)                 // if the MCU is transmitting its State on the bus
    {
        CANIntClear(CAN0_BASE, 5);      // clear the interuppt
        g_bErrFlag = 0;                 // indicate no errors
    }
    if(ui32Status == 2)                 // if the MCU is transmitting DATA SIZE on the bus
    {
        CANIntClear(CAN0_BASE, 2);
        g_bErrFlag = 0;
    }

    if(ui32Status == 6)                 // if the MCU is transmitting DATA  on the bus
    {
        CANIntClear(CAN0_BASE, 6);
        g_bErrFlag = 0;

    }

}

/*
 * CAN Receive
 */



//*****************************************************************************
//
// a Function to recieve the data (8 bytes) from the other MCU using the CAN bus
//
//*****************************************************************************

void CAN_Receive_Data(int receivedValue)                    // parameter to show the number of Bytes recieved from the other MCU till now
{
    if(g_bRXFlag == 1)                                      // flag set in the handler to 1 meaning a message is received
    {
        CANMessage.pui8MsgData = pui8MsgData;               //Point to the local Size buffer of the MCU

        CANMessageGet(CAN0_BASE, 1, &CANMessage, 0);        // get the message in the first object
        int j;                                              // Counter for the for loop
        for(j=0;j<8;j++)
        {
            receivedText[receivedValue+j]=pui8MsgData[j];   //save each byte in its corresponding position in the array to be presented
        }
        receivedMessageCount+=8;                            // increment recieved bytes by 8


        g_bRXFlag = 0;                                      // set to zero for upcoming interrupt
        if(CANMessage.ui32Flags & MSG_OBJ_DATA_LOST)
        {
            //stay here if message has been lost
        }
    }
}



//*****************************************************************************
//
// a Function to recieve the size of data in bytes from the other MCU using the CAN bus
//
//*****************************************************************************

void CAN_Receive_Size(char *receivedValue)
{
    if(g_bRXFlag == 1)                                              // flag set in the handler to 1 meaning a message is received
    {
        tCANMessage.pui8MsgData = MsgSize;                          //Point to the local Size buffer of the MCU

        CANMessageGet(CAN0_BASE, 4, &tCANMessage, 0);               // get the message

        *receivedValue = MsgSize[0];                                // Save the message data in the buffer

        g_bRXFlag = 0;                                              // set to zero for upcoming interrupt
        if(tCANMessage.ui32Flags & MSG_OBJ_DATA_LOST)
        {
            //stay here if message has been lost
        }
    }
}


//*****************************************************************************
//
// a Function to recieve the State of the other MCU in bytes from the other MCU using the CAN bus
//
//*****************************************************************************

void CAN_Receive_State(char *receivedValue)
{
    if(g_bRXFlag == 1)                                                  // flag set in the handler to 1 meaning a message is received
    {
        sCANMessage.pui8MsgData = MsgState;                             //Point to the local Size buffer of the MCU

        CANMessageGet(CAN0_BASE, 3, &sCANMessage, 0);                   // get the message

        *receivedValue = MsgState[0];                                   // Save the message data in the buffer

        g_bRXFlag = 0;                                                  // set to zero for upcoming interrupt
        if(sCANMessage.ui32Flags & MSG_OBJ_DATA_LOST)
        {
            //stay here if message has been lost
        }
    }
}

/*
 *  Recieve Message Objects
 */


//*****************************************************************************
//
// Initialize the recieve message object of the data size for the CAN controller
//
//*****************************************************************************

void init_MessageObj_data_size()
{
    tCANMessage.ui32MsgID = 0x20;                                               //Set the ID as 0x20
    tCANMessage.ui32MsgIDMask = 0x30;                                           //Accept IDs upto 0x20 - 0x2F
    tCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;      //enable filtering and interrupt
    tCANMessage.ui32MsgLen = 8;                                                 // data length 8 Bytes
    CANMessageSet(CAN0_BASE, 4, &tCANMessage, MSG_OBJ_TYPE_RX);                 // set the message object with the upper configurations
}


//*****************************************************************************
//
// Initialize the recieve message object of the data for the CAN controller
//
//*****************************************************************************

void init_MessageObj_data()
{
    CANMessage.ui32MsgID = 0x10;                                                //Set the ID as 0x10
    CANMessage.ui32MsgIDMask = 0x10;                                            //Accept IDs upto 0x10 - 0x1F
    CANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;       //enable filtering and interrupt
    CANMessage.ui32MsgLen = 8;                                                  // data length 8 Bytes
    CANMessageSet(CAN0_BASE, 1, &CANMessage, MSG_OBJ_TYPE_RX);                  // set the message object with the upper configurations
}



//*****************************************************************************
//
// Initialize the recieve message object of the MCU State for the CAN controller
//
//*****************************************************************************

void init_MessageObj_State()
{
    sCANMessage.ui32MsgID = 0x400;                                              //Set the ID as 0x400
    sCANMessage.ui32MsgIDMask = 0x400;                                          //set the filtering mask
    sCANMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;      //enable filtering and interrupt
    sCANMessage.ui32MsgLen = 8;                                                 // data length 8 Bytes
    CANMessageSet(CAN0_BASE, 3, &sCANMessage, MSG_OBJ_TYPE_RX);                 // set the message object with the upper configurations
}



/**************************************************
 * CAN SEND
 */


//*****************************************************************************
//
// Function to transmit the State of MCU to the other MCU on the CAN bus
//
//*****************************************************************************

void canSendState(char value)
{
    tCANMsgObject sCANMessage;                                          //initialize a msg object
    sCANMessage.ui32MsgID = 0x400;                                      //set ID as 0x400
    sCANMessage.ui32MsgIDMask = 0;
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;                      //enable TX interuppt
    sCANMessage.ui32MsgLen = 8;                                         // data length 8 bytes
    sCANMessage.pui8MsgData =  & value;                                 // point to input parameter to get the state
    CANMessageSet(CAN0_BASE, 5, &sCANMessage, MSG_OBJ_TYPE_TX);         // Send the message on the bus
}


//*****************************************************************************
//
// Function to transmit the data From MCU to the other MCU on the CAN bus
//
//*****************************************************************************

void can_send_data(char * value)                                        // a pointer to the current 8 bytes to be sent in this CAN frame
{
    tCANMsgObject sCANMessage;                                          //initialize a msg object
    sCANMessage.ui32MsgID = 0x11;                                       //set ID as 0x11
    sCANMessage.ui32MsgIDMask = 0;
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;                      //enable TX interuppt
    sCANMessage.ui32MsgLen = 8;                                         // data length 8 bytes
    sCANMessage.pui8MsgData =   value;                                  // point to the current 8 bytes to be sent in this CAN frame
    CANMessageSet(CAN0_BASE, 6, &sCANMessage, MSG_OBJ_TYPE_TX);         // Send the message on the bus

}

//*****************************************************************************
//
// Function to transmit the Size of data to be transmitted to the other MCU on the CAN bus
//
//*****************************************************************************

void can_send_size(char value)
{
    tCANMsgObject sCANMessage;                                          //initialize a msg object
    sCANMessage.ui32MsgID = 0x20;                                       //set ID as 0x20
    sCANMessage.ui32MsgIDMask = 0;
    sCANMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;                      //enable TX interuppt
    sCANMessage.ui32MsgLen = 8;                                         // data length 8 bytes
    sCANMessage.pui8MsgData =  & value;                                 // point to input parameter to get the Size
    CANMessageSet(CAN0_BASE, 2, &sCANMessage, MSG_OBJ_TYPE_TX);         // Send the message on the bus


}

