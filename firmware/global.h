/*
 * File:   global.h
 * Author: mickael
 *
 * Created on 11 juillet 2015, 18:32
 */
#ifndef GLOBAL_H
#define	GLOBAL_H

#ifdef	__cplusplus
extern "C" {
#endif


     //set Frequency to 1MHz
#define _XTAL_FREQ   (1000000)

#define START_WATCHDOG()  (WDTCONbits.SWDTEN = 1)
#define STOP_WATCHDOG()   (WDTCONbits.SWDTEN = 0)

#define DEEP_SLEEP()   do { START_WATCHDOG();SLEEP();STOP_WATCHDOG(); } while (0);

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBAL_H */

