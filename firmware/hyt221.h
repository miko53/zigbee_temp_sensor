/* 
 * File:   hyt221.h
 * Author: mickael
 *
 * Created on 11 juillet 2015, 18:14
 */

#ifndef HYT221_H
#define	HYT221_H

#include "status.h"
#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

extern STATUS_T hyt221_launch_acq();
extern STATUS_T hyt221_operation();
extern uint16_t hyt221_getTemp();
extern uint16_t hyt221_getHumidity();


#ifdef	__cplusplus
}
#endif

#endif	/* HYT221_H */

