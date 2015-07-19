#include <xc.h>
#include <stdint.h>
#include "leds.h"
#include "uart_loc.h"

static uint8_t counter = 0;
static int8_t ledState = 0;
static int8_t ledYState = 0;

static uint8_t UART_writeIndex = 0;
//extern uint8_t nbCharRecu;

void interrupt high_priority isr_hi(void)
{
    //IT TIMER
    if (INTCONbits.TMR0IE & INTCONbits.TMR0IF)
    {
        /*
        counter++;
        if (counter == 245)
        {
            counter = 0;
            if (ledState == 0)
            {
                ledState = 1;
                led_green_ON();
            }
            else
            {
                ledState = 0;
                led_green_OFF();
            }
        }*/

        //clear interrupt flags
        INTCONbits.TMR0IF = 0;
        return;
    }

    //IT UART RECEPTION
    if (PIE1bits.RC1IE & PIR1bits.RCIF)
    {
        uint8_t dummy;
        //nbCharRecu++;
        //USART character reception
        //get status firt RCSTA;
        if (RCSTAbits.FERR)
        {
           //framing error
           UART_errorStatus.framingError++;
           dummy = RCREG;
        }
        else if (RCSTAbits.OERR)
        {
            //overrun error, set reception again
            UART_errorStatus.overrunError++;
            dummy = RCREG;
            RCSTAbits.CREN = 0;
        }
        else if (UART_rxBufferNb < USART_RX_BUFFER_SIZE)
        {
            UART_rxBuffer[UART_writeIndex] = RCREG;
            UART_writeIndex++;
            if (UART_writeIndex >= USART_RX_BUFFER_SIZE)
                UART_writeIndex = 0;
            UART_rxBufferNb++;
        }
        else
        {
           //we can not store the char, read and indicate error
            UART_errorStatus.bufferOverrunError++;
           dummy = RCREG;
        }
        return ;
    }
}


/***
 *  Just to remember, will be not used - not necessary
 **/

/*
void interrupt low_priority isr_lo(void)
{
    gData[0]= 0;
}
*/



