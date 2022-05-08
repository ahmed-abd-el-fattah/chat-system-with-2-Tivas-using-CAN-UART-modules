#include "CAN_Functions.h"
#include "GPIO_Functions.h"
#include "States.h"
#include "UART_Functions.h"

int main(void)
{
    CAN_Init();                             //initialization of Can
    GPIOF_Init();                           //initialization of GPIO
    UART_INIT();                            //initialization of UART
    init_MessageObj_data();                 //initialization of data message object
    init_MessageObj_data_size();            //initialization of data size message object
    init_MessageObj_State();                //initialization of state message object

        while(1)
        {
            Idle_state();                   //start idle state
        }

}
