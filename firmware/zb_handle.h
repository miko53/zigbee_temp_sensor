/*
 * File:   zb_handle.h
 * Author: mickael
 *
 * Created on 12 juillet 2015, 16:11
 */

#ifndef ZB_HANDLE_H
#define	ZB_HANDLE_H

#include <stdint.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define XBEE_RESET_OFF()  (LATAbits.LA1 = 1)
#define XBEE_RESET_ON()   (LATAbits.LA1 = 0)
#define XBEE_SLEEP_RQ()   (LATAbits.LATA0 = 1)
#define XBEE_WAKE_UP()    (LATAbits.LATA0 = 0)

typedef enum
{
  ZB_STATUS_NOT_JOINED,
  ZB_STATUS_JOINED,
  ZB_STATUS_IN_ERROR
} zb_statusT;

extern void zb_handle(void);
extern zb_statusT zb_handle_getStatus(void);
extern void zb_handle_sendData();
extern void zb_handle_setTempRaw(uint16_t tempRaw);
extern void zb_handle_setHumidityRaw(uint16_t humidityRaw);
extern void zb_handle_setbatVolt(uint16_t battVoltage);
extern void zb_handle_resetStatus();


#ifdef	__cplusplus
}
#endif

#endif	/* ZB_HANDLE_H */

