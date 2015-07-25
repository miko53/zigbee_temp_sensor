
#include <stdint.h>
#include "leds.h"
#include "global.h"
#include <xc.h>

void leds_red_glitch(void)
{
    led_red_ON();
    __delay_ms(100);
    led_red_OFF();
}

void leds_yellow_glitch(void)
{
    led_yellow_ON();
    __delay_ms(100);
    led_yellow_OFF();
}

void leds_green_glitch(void)
{
    led_green_ON();
    __delay_ms(100);
    led_green_OFF();
}
