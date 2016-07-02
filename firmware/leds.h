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

extern void leds_red_glitch(void);
extern void leds_yellow_glitch(void);
extern void leds_green_glitch(void);
extern void leds_red_and_yellow_glitch();
extern void leds_green_and_yellow_glitch();

#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */

