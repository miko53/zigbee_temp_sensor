#include <xc.h>
#include <stdint.h>
#include "config.h"
#include "leds.h"
#include "status.h"
#include "uart.h"
#include "i2c.h"
#include "global.h"
#include "zb_handle.h"
#include "hyt221.h"

//uint8_t nbCharRecu;
//static STATUS_T i2c_operation();
//static void wait_infinite();
//static void i2c_launch_acq();
//static void do_calculate_temp_humd();
//void i2c_main_loop();

static void main_loop();


int main(void)
{
  OSCTUNE = 0x00;
  //set up internal clock to 1MHz
  OSCCON  = 0x40;

  //configure PORT D in output for led display
  PORTD = 0x0;
  TRISD = 0x0;

  i2c_setup();
  uart_setup();
  
  //TIMER setup
  //1MHz --> 250kHz /2 --> 125kHz by 255  2.04ms
  T0CON = 0xC0; // enable timer, 8bits timer
  INTCONbits.TMR0IE = 1;

  INTCONbits.PEIE = 1; //activate peripherical interrupt
  INTCONbits.GIE = 1; //activate global interrupt

  //uint8_t requestHWversion[] =
  //{ 0x7e,  0x00, 0x04, 0x08 , 0x03, 'H', 'V', 0x56 };
  //uart_write(sizeof(requestHWversion), requestHWversion);

  main_loop();
}

typedef enum
{
  WAIT_JOINED,
  JOINED,
  SLEEP,
  WAIT_HYT221_ACQ
} main_stateT;

static main_stateT main_state;

static void main_loop()
{
  zb_statusT zb_status;
  STATUS_T hyt221_status;
  main_state = WAIT_JOINED;
  
  while (1)
  {
      zb_handle();
      zb_status = zb_handle_getStatus();
      switch (zb_status)
      {
          case ZB_STATUS_NOT_JOINED:
              main_state = WAIT_JOINED;
              __delay_ms(500);
              break;

          case ZB_STATUS_JOINED:
              if (main_state == WAIT_JOINED)
                main_state = JOINED;
              break;

          case ZB_STATUS_IN_ERROR:
          default:
              break;
      }
      
      switch (main_state)
      {
          case WAIT_JOINED:
              break;

          case JOINED:
              hyt221_launch_acq();
              main_state = WAIT_HYT221_ACQ;
              break;

          case SLEEP:
              //sleep 1min
              __delay_ms(500);
              __delay_ms(500);
              main_state = WAIT_JOINED;
              break;
              
          case WAIT_HYT221_ACQ:
              __delay_ms(60);
              hyt221_status = hyt221_operation();
              if (hyt221_status == STATUS_OK)
              {
                  //send data
                  zb_handle_sendData(hyt221_getTemp(), hyt221_getHumidity());

                  main_state = SLEEP;
              }
              else if (hyt221_status == STATUS_ERROR)
              {
                  
              }
              break;

          default:
              break;
      }
  }
}

  


/*

  while (1)
  {
      uint8_t i;
      for(i = 0; i< sizeof(requestHWversion); i++)
      {
          TXREG = requestHWversion[i];
          while (!TXSTAbits.TRMT);
      }
      
      __delay_ms(600);
      if (nbCharRecu >= 11)
          led_red_ON();
      else
          led_red_OFF();
  }*/

  /*
  {
    
    uint8_t i;
      for(i = 0; i< sizeof(requestHWversion); i++)
      {
          while (!PIR1bits.RCIF);
          rxData[i]  = RCREG;
      }

  }
*/

 //i2c_main_loop();

 // wait_infinite();

/*
void i2c_main_loop()
{
  nbStale = 0;
  STATUS_T s;
  BOOL yellow = FALSE;

  while (1)
  {
      i2c_launch_acq();
      //bus free condition must be at min. 1us
      __delay_us(1);
      while (1)
      {
        __delay_ms(60);
        s = i2c_operation();
        if (s == STATUS_ERROR)
        {
            led_green_OFF();
            break;
        }
        if (s == STATUS_OK)
        {
            do_calculate_temp_humd();
            break;
        }
      }
      
      if (yellow == FALSE)
      {
          led_yellow_ON();
          yellow = TRUE;
      }
      else
      {
          led_yellow_OFF();
          yellow = FALSE;
      }
      nbStale = 0;
  }

  wait_infinite();
  return ;
}

*/


/*

static void wait_infinite()
{
  while (1)
  {
    led_yellow_ON();
    __delay_ms(500);
    __delay_ms(500);
    led_yellow_OFF();
    __delay_ms(500);
    __delay_ms(500);
  }
}


*/
