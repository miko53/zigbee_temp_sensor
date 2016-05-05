
#include <xc.h>
#include "config.h"
#include "global.h"
#include "uart.h"
#include "zb_handle.h"
#include "status.h"
#include "leds.h"
#include "i2c.h"
#include "hyt221.h"
#include "osc.h"

static void main_loop();
static void wait_endOfConversion(void);

int main(void)
{
  OSCTUNE = 0x00;
  //set up internal clock to 1MHz
  OSCCON  = 0x40;

  //configure PORT A in output for led display and zigbee mngt
  PORTA = 0x0;
  TRISA = 0x0;

  PORTBbits.RB1 = 0;
  TRISBbits.RB1 = 0; //set RB1 in OUTPUT
  TRISBbits.RB2 = 1; //set RB2 in INPUT (AN8)
  PIR1bits.ADIF = 0; //set A/D buffer to empty
  PIE1bits.ADIE = 1; //enable A/D interrupt

  __delay_ms(5);
  T1CON = 0x0F;
  //int_rc_cal();
  calibration();
  OSCTUNE = 0;
  __delay_ms(5);
  T1CON = 0;

  i2c_setup();
  uart_setup();

  INTCONbits.PEIE = 1; //activate peripherical interrupt
  INTCONbits.GIE = 1; //activate global interrupt

  XBEE_RESET_ON();
  XBEE_RESET_OFF();

  main_loop();
  
  while (1)
  {
      __delay_ms(500);
      LATA = 0x1C;
      __delay_ms(500);
      LATA = 0x00;
  }

  
}

typedef enum
{
  WAIT_JOINED,
  JOINED,
  SLEEP,
  WAIT_HYT221_ACQ,
} main_stateT;

static main_stateT main_state;
static uint16_t batt_value;
static uint8_t batt_counter;
static uint8_t wait_join_counter;

static void batt_launch_acq();

static void main_loop()
{
  zb_statusT zb_status;
  STATUS_T hyt221_status;

  batt_counter = 5;
  wait_join_counter = 0;
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
              wait_join_counter++;
              if (wait_join_counter >= 5)
              {
                  wait_join_counter = 0;
                  WDTCONbits.SWDTEN = 1; // = activate watchdow
                  SLEEP();
                  WDTCONbits.SWDTEN = 0; //desactivate watchdow
              }
              break;

          case JOINED:
              zb_handle_resetStatus();
              
              batt_counter++;
              if (batt_counter >= 5)
              {
                batt_counter = 0;
                batt_launch_acq();
              }

              hyt221_status = hyt221_launch_acq();
              if (hyt221_status == STATUS_OK)
              {
                main_state = WAIT_HYT221_ACQ;
              }

              break;

          case SLEEP:
              //sleep around 1min

              LATBbits.LATB1 = 1; //XBee sleep request
              WDTCONbits.SWDTEN = 1; // = activate watchdow
              SLEEP();
              WDTCONbits.SWDTEN = 0; //desactivate watchdow
              LATBbits.LATB1 = 0; //XBee end of sleep request

              main_state = WAIT_JOINED;
              break;

          case WAIT_HYT221_ACQ:
              //__delay_ms(60);
              wait_endOfConversion();
              hyt221_status = hyt221_operation();
              if (hyt221_status == STATUS_OK)
              {
                  //send data
                  zb_handle_setTempRaw(hyt221_getTemp());
                  zb_handle_setHumidityRaw(hyt221_getHumidity());
                  zb_handle_sendData();
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


static void batt_launch_acq()
{
  LATBbits.LATB1 = 1; //activate batt_sensor
  ADCON1 = 0x00; //Vss & Vdd src reference, only AN0
  ADCON0 = 0x20; //disable and select AN8
  ADCON2 = 0x8B; //right justifed, 2Tad, Frc
  ADCON0 |= 1; //start A/D movule
  ADCON0 |= 0x2 ; //start conversion

  SLEEP();
  while ((ADCON0 & 0x02) == 0x02)
      ;

  batt_value = (ADRESH<<8) | ADRESL;
  LATBbits.LATB1 = 0; //desactivate batt_sensor
  zb_handle_setbatVolt(batt_value);///*calibr*//*mesrd_instr*/batt_value);
}
