#include <xc.h>
#include <stdint.h>
#include "uart.h"
#include "uart_loc.h"

uint8_t UART_rxBufferNb = 0;
uint8_t UART_rxBuffer[USART_RX_BUFFER_SIZE];
UART_errorT UART_errorStatus;
static uint8_t UART_readIndex = 0;

void uart_setup()
{
  //UART setup
  TRISCbits.RC7 = 1;
  TRISCbits.RC6 = 1;

  TXSTA = 0x24; // Transmit enabled, asynch mode, high baud rate,
  RCSTA = 0x90; // serial enabled SPEN set, Asynch mode, disable addr
  BAUDCON = 0x48; ////default & BRG16 set
  SPBRGH = 0;
  SPBRG = 12; //25 for 9600 baud 12 for 19200 bauds

  PIR1bits.RCIF = 0; //set EUSART buffer to empty
  PIE1bits.RCIE = 1; //enable EUSART interrupt
}


void uart_write(uint8_t size, uint8_t* buffer)
{
  uint8_t i;
  uint8_t* b;

  b = buffer;
  for (i = 0; i < size; i++)
  {
    TXREG = buffer[i];
    while (TXSTAbits.TRMT == 0)
    {
      NOP();
    }
    b++;
  }

  NOP(); //add not to correctly send the last byte before to go to sleep
}


BOOL uart_read(uint8_t* buffer, uint8_t nbToRead)
{
  BOOL expectedNbRead;
  uint8_t i;
  uint8_t* b;
  b = buffer;

  if (nbToRead <= UART_rxBufferNb)
  {
    //copy the data into the given buffer
    for (i = 0; i < nbToRead; i++)
    {
      *b = UART_rxBuffer[UART_readIndex];
      UART_readIndex++;
      if (UART_readIndex >= USART_RX_BUFFER_SIZE)
      {
        UART_readIndex = 0;
      }
      b++;
    }
    UART_rxBufferNb -= nbToRead;
    expectedNbRead = TRUE;
  }
  else
  {
    expectedNbRead = FALSE;
  }

  return expectedNbRead;
}


