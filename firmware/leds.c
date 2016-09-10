
#include <stdint.h>
#include "leds.h"
#include "global.h"
#include <xc.h>


#define led_red_ON() LATA |= LED_RED
#define led_red_OFF() LATA &= ~LED_RED

#define led_yellow_ON() LATA |= LED_YELLOW
#define led_yellow_OFF() LATA &= ~LED_YELLOW

#define led_green_ON() LATA |= LED_GREEN
#define led_green_OFF() LATA &= ~LED_GREEN

void leds_glitch(uint8_t leds)
{
  LATA |= leds;
  __delay_ms(100);
  LATA &= ~leds;
}
