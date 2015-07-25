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

  ///configure PORD B in output for zigbee mgt
  //set analog input into digital input
  ADCON1 = 0x05; //AN10(RB1)/AN11(RB4)/AN12(RB0)

  //put RB0 and RB1 in output
  //RB0 = _RESET
  //RB1 = SLEEP_RQ
  LATB = 0x01;
  TRISB = 0xFC;

  i2c_setup();
  uart_setup();
  
  //TIMER setup
  //1MHz --> 250kHz /2 --> 125kHz by 255  2.04ms
  //T0CON = 0xC0; // enable timer, 8bits timer
  //INTCONbits.TMR0IE = 1;

  INTCONbits.PEIE = 1; //activate peripherical interrupt
  INTCONbits.GIE = 1; //activate global interrupt

  //uint8_t requestHWversion[] =
  //{ 0x7e,  0x00, 0x04, 0x08 , 0x03, 'H', 'V', 0x56 };
  //uart_write(sizeof(requestHWversion), requestHWversion);

  //generate a reset of XBee module cycle time to 1MHZ give 4µs, the pulse must be at least 200ns
  LATBbits.LATB0 = 0;
  LATBbits.LATB0 = 1;

  main_loop();
}

typedef enum
{
  WAIT_JOINED,
  JOINED,
  SLEEP,
  WAIT_HYT221_ACQ,
} main_stateT;

static main_stateT main_state;
//static uint8_t wakeTick;
static void wait_endOfConversion(void);


static void main_loop()
{
  zb_statusT zb_status;
  STATUS_T hyt221_status;
  main_state = WAIT_JOINED;

  leds_green_glitch();

  while (1)
  {
      zb_handle();
      zb_status = zb_handle_getStatus();
      switch (zb_status)
      {
          case ZB_STATUS_NOT_JOINED:
              main_state = WAIT_JOINED;
              leds_yellow_glitch();
              __delay_ms(400); //400+100 in glitch func
              break;

          case ZB_STATUS_JOINED:
              if (main_state == WAIT_JOINED)
                main_state = JOINED;
              break;

          case ZB_STATUS_IN_ERROR:
              leds_red_glitch();
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
              LATBbits.LATB1 = 1; //sleep request
              WDTCONbits.SWDTEN = 1;
              SLEEP();
              WDTCONbits.SWDTEN = 0;
              LATBbits.LATB1 = 0; //end of sleep
              main_state = WAIT_JOINED;
              break;

          case WAIT_HYT221_ACQ:
              //__delay_ms(60);
              wait_endOfConversion();
              hyt221_status = hyt221_operation();
              if (hyt221_status == STATUS_OK)
              {
                  //send data
                  zb_handle_sendData(hyt221_getTemp(), hyt221_getHumidity());
                  //leds_green_glitch();
                  __delay_ms(500); //400  + 100 in glitch
                  main_state = SLEEP;
              }
              else if (hyt221_status == STATUS_ERROR)
              {
                  leds_red_glitch();
              }
              break;

          default:
              break;
      }
  }
}


static void wait_endOfConversion(void)
{
  T0CON = 0xC5; // enable timer, 8bits timer,prescaler to 64 for around 65ms
  INTCONbits.TMR0IE = 1; //active TIMER0 interupt.
  OSCCONbits.IDLEN = 1; //set Idle mode on Sleep instruction
  SLEEP(); //TODO check that end of idle mode on IT timer
  OSCCONbits.IDLEN = 0; //reset idle mode on sleep instruction
  T0CONbits.TMR0ON = 0; //stop timer
}
