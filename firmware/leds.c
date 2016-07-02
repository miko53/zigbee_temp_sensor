
#include <stdint.h>
#include "leds.h"
#include "global.h"
#include <xc.h>

#define     LED_YELLOW      (0x04)
#define     LED_RED         (0x08)
#define     LED_GREEN       (0x10)

#define led_red_ON() LATA |= LED_RED
#define led_red_OFF() LATA &= ~LED_RED

#define led_yellow_ON() LATA |= LED_YELLOW
#define led_yellow_OFF() LATA &= ~LED_YELLOW

#define led_green_ON() LATA |= LED_GREEN
#define led_green_OFF() LATA &= ~LED_GREEN


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


void leds_red_and_yellow_glitch()
{
    led_red_ON();
    led_yellow_ON();
    __delay_ms(100);
    led_red_OFF();
    led_yellow_OFF();
}

void leds_green_and_yellow_glitch()
{
    led_green_ON();
    led_yellow_ON();
    __delay_ms(100);
    led_green_OFF();
    led_yellow_OFF();
}
