
#include "osc.h"
#include "global.h"
#include <xc.h>
#include <stdint.h>
#include "leds.h"

#define CALIBRATION_NB_INSTS_EXPECTED   ((int8_t)(0x30))
#define CALIBRATION_ACCEPTABLE_DELTA    ((int8_t) (2))
volatile int8_t calibr;

void calibration(void)
{
  int8_t w;
  int8_t tryCounter;

  T1CON = 0x0F;
  tryCounter = 0;
  while (tryCounter < 32)
  {
    calibr = 0;
    __delay_ms(250);
    T1CON = 0x0F;
    //TMR1H = 0x80;
    //TMR1L = 0x00;
    TMR1H = 0xFF;
    TMR1L = 0xE7;
    PIR1bits.TMR1IF = 0;
    //  T1CON |= 0x01;

    while (PIR1bits.TMR1IF == 0)
    {
      calibr++;
    }

    w =  CALIBRATION_NB_INSTS_EXPECTED - calibr;
    if (w > 0)
    {
      //too slow increase it
      w -= CALIBRATION_ACCEPTABLE_DELTA;
      if (w > 0)
      {
        //do calibration again
        OSCTUNE = (OSCTUNE + 1) & 0x1F;
      }
      else
      {
        break;
      }
    }
    else
    {
      //too speed, downcrease it
      w = CALIBRATION_ACCEPTABLE_DELTA - w;
      if (w > 0)
      {
        OSCTUNE = (OSCTUNE - 1) & 0x1F;
      }
      else
      {
        break;
      }
    }
    __delay_us(300);
    tryCounter++;
  }

  if (tryCounter >= 32)
  {
    OSCTUNE = 0;
    __delay_us(300);
    leds_glitch(LED_RED);
    __delay_ms(250);
    leds_glitch(LED_RED);
    __delay_ms(250);
    leds_glitch(LED_RED);
  }

  //OSCCON  = 0x40;
  T1CON = 0;

}
