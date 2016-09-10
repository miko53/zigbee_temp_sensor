
#include "timer.h"
#include "timer_loc.h"
#include "global.h"

BOOL timer0_interupt_done;

static void timer0_wait_ready();

//wait 65ms
void timer0_wait_65ms(void)
{
  TMR0 = 0;
  T0CON = 0x45; // 8bits timer,prescaler to 64 for around 65ms
  INTCONbits.TMR0IF = 0;
  INTCONbits.TMR0IE = 1; //active TIMER0 interupt.
  T0CONbits.TMR0ON = 1;// enable timer

  IDLE_SLEEP();
  timer0_wait_ready();

  T0CONbits.TMR0ON = 0; //stop timer
  INTCONbits.TMR0IE = 0;
}

void timer0_wait_262ms(void)
{
  //  LATA |= (0x08);//RED ON
  TMR0 = 0;
  T0CON = 0x47; // set-up timer, 8bits timer,prescaler to 256 for around 262ms
  INTCONbits.TMR0IF = 0;
  INTCONbits.TMR0IE = 1; //active TIMER0 interrupt.
  T0CONbits.TMR0ON = 1;
  IDLE_SLEEP();
  timer0_wait_ready();
  T0CONbits.TMR0ON = 0; //stop timer
  INTCONbits.TMR0IE = 0;
  //  __delay_ms(100);
  //  LATA &= ~(0x08); //RED OFF
}

static void timer0_wait_ready()
{
  IDLE_SLEEP();

  while (timer0_interupt_done == FALSE)
    ;
  timer0_interupt_done = FALSE;
}
