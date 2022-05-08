
/*
 * States.c
 *
 *  Created on: Apr 12, 2022
 *      Author: Lenovo
 */
#include "includes.h"
#include "GPIO_Functions.h"
#include "CAN_Functions.h"

extern char receiveBufferSize;                      // variable holding the message size
extern char receivedText[201];                      // array to save the message for presenting it
extern volatile uint32_t receivedMessageCount;      // a variable to show how many bytes of message have been received
extern volatile bool g_bRXFlag;                     // A flag for the interrupt handler to indicate that a message was received.
extern volatile bool g_sizeFlag;                    // A flag to indicate a size message was received
extern char stateFlag;                              // general token indicating the states



//*****************************************************************************
//
//This is the mapping array for encryption and decryption of the messages
//
//*****************************************************************************
char mapping[255]={'A',156,157,158,159,140,141,142,143,144,145,146,
                   147,148,149,130,131,132,133,134,135,136,137,138,139,129,128,127,126,125,124,
                   'a',163,164,165,166,167,168,169,150,151,152,153,154,155,174,162,'b','B','C','c',189,
                   170,171,172,173,175,188,176,177,178,179,160,161,182,183,184,185,186,187,'M','m','N',
                   'n','V','v',192,193,194,195,196,197,198,199,180,181,'1','2','3','4',255,254,253,252,
                   251,250,'5','6','7','8','9','q','w','r','t','y','e','s','S','d','D','z','Z','X','x',
                   'L','l','P','p','K','k','F','f',249,248,247,246,245,244,243,242,241,240,239,238,237,
                   236,235,234,233,232,231,230,220,221,222,223,224,225,226,227,228,229,210,211,212,213,
                   214,215,216,217,218,219,200,201,202,203,204,205,206,207,208,209,190,191};

//*****************************************************************************
//
// A function for encryption
//
//*****************************************************************************
void encrypt (char receivedArray[] , int size)
{
    int i=0;                                            //looping counter
    for(i=0;i<size;i++)
    {
        receivedArray[i]=mapping[receivedArray[i]];     //fill the array with encrypted values
    }
}

//*****************************************************************************
//
// A function for decryption
//
//*****************************************************************************
void decrypt (char encryptedMessage[] , int size)
{
    int i=0 , j=0;                                      //i loop on size, j loop on mapping array size

    // loop on the message size
    for(i=0;i<size;i++)
    {
        //loop on mapping array size
        for(j=0;j<=255;j++)
        {
            if (mapping[j]==encryptedMessage[i])
            {
                break;
            }
        }
        encryptedMessage[i]=j;                          //decrypt the message
    }
}



//*****************************************************************************
//
// presenting state
//
//*****************************************************************************
void dataPresentingState(char receivedArray[], char receivedCount)
{
    int i;
    state_red();                                        //turn red led
    UARTEnable(UART0_BASE);                             //enable the UART

    //loop on the received array
        for (i = 0; i<= receivedCount ; i++)
        {
            UARTCharPut(UART0_BASE, receivedArray[i]);      //Show data in the PC
            SysCtlDelay(200000);
            while(UARTBusy(UART0_BASE)){}                   //wait for UART to be available
        }

        // make enter for new line
        UARTCharPut(UART0_BASE, '\r');
        UARTCharPut(UART0_BASE, '\n');

    decrypt(receivedArray, receivedCount);              //decrypt the message

    //loop on the received array
    for (i = 0; i<= receivedCount ; i++)
    {
        UARTCharPut(UART0_BASE, receivedArray[i]);      //Show data in the PC
        SysCtlDelay(200000);
        while(UARTBusy(UART0_BASE)){}                   //wait for UART to be available
    }

    // make enter for new line
    UARTCharPut(UART0_BASE, '\r');
    UARTCharPut(UART0_BASE, '\n');

    UARTDisable(UART0_BASE);                            // disable UART

}


//*****************************************************************************
//
//receiving state
//
//*****************************************************************************
void receivingState()
{
    int i=0;                                                                // counter for loop
    state_red();                                                            // turn red led

    // if size message is received
    if(g_sizeFlag == 1 && g_bRXFlag == 1)
    {
        // zero fill the received text array
        for ( i=0; i<=200; i++)
        {
            receivedText[i] = 0;
        }
        CAN_Receive_Size(&receiveBufferSize);                               //set the size of the message
        g_sizeFlag = 0;                                                     //unflag message flag
    }

    //if message object received and the message is not all received yet
    if(g_bRXFlag == 1 && receivedMessageCount <= receiveBufferSize)
    {
        CAN_Receive_Data(receivedMessageCount);                             //receive 8 bytes of the text
    }

    //if all message is received
    if(receivedMessageCount > receiveBufferSize)
    {
        receivedMessageCount = 0;                                           //clear the message count for next operation
        dataPresentingState(receivedText, receiveBufferSize);               //start the presenting state
        return;                                                             // return to idle state
    }
}



//*****************************************************************************
//
// retreiving state
//
//*****************************************************************************
void retrieve_state()
{
    state_green();                                                               // turn green led
    char retrieved_array[201] = {0} ;                                              // array to hold data from the UART
    char retrieved_array_count = 0;
    UARTEnable(UART0_BASE);                                                      //enable UART

    while (1)
    {
    //if characters are available
    if (UARTCharsAvail(UART0_BASE))
    {
        retrieved_array[retrieved_array_count] = UARTCharGet(UART0_BASE);
        retrieved_array_count++;
    }

    //wait for UART to be available
    while (UARTBusy(UART0_BASE))
    {}

    //if characters of 200 arrived or PB is pressed
    if (retrieved_array_count == 200  ||  button_status() == PUSHBUTTON2_PRESSED)
    {
        encrypt(retrieved_array, retrieved_array_count);                        //encrypt the message
        transmit_state(retrieved_array , retrieved_array_count);                //start transmitting
        UARTDisable(UART0_BASE);                                                //disable UART
        return;                                                                 //return to idle state
    }
}
}

//*****************************************************************************
//
// Idle state
//
//*****************************************************************************
void Idle_state()
{
    while(1)
    {

        state_blue();                                                               //turn blue led

        //if the state is idle and PB is pressed
        if (button_status() == PUSHBUTTON1_PRESSED && stateFlag == stateIdle )
        {
            stateFlag = stateBusy ;                     //make the state busy
            canSendState(stateFlag);                    //send the new state to the MCU by Can
            retrieve_state();                           //go to retrieve state
        }

    }
}


//*****************************************************************************
//
//transmit state
//
//*****************************************************************************
void transmit_state(char retrieved_array[],char retrieved_array_count)
{
    int j ;                                                 //counter
    state_green();                                          //turn green led
    can_send_size(retrieved_array_count);                   //send the size of the message

    //loop to send the message 8 bytes per loop
    for (j = 0; j <= retrieved_array_count ; j +=8)
    {

        can_send_data(retrieved_array );                    //send pointer to first location holding 8 bytes
        SysCtlDelay(3000);
        retrieved_array+=8;                                 //setting the next offset for next 8 bytes

    }
    stateFlag = stateIdle;                                  // set the state flag to idle
    canSendState(stateFlag);                                //send the state flag
}

