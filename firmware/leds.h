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

#define     LED_YELLOW      (0x04)
#define     LED_RED         (0x08)
#define     LED_GREEN       (0x10)

extern void leds_glitch(uint8_t leds);

#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */

