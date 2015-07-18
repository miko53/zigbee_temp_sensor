/*
 * File:   leds.h
 * Author: mickael
 *
 * Created on 6 avril 2015, 12:30
 */

#ifndef LEDS_H
#define	LEDS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define     LED_YELLOW      (0x01)
#define     LED_RED         (0x02)
#define     LED_GREEN       (0x04)

#define led_red_ON() LATD |= LED_RED
#define led_red_OFF() LATD &= ~LED_RED

#define led_yellow_ON() LATD |= LED_YELLOW
#define led_yellow_OFF() LATD &= ~LED_YELLOW

#define led_green_ON() LATD |= LED_GREEN
#define led_green_OFF() LATD &= ~LED_GREEN


#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */

