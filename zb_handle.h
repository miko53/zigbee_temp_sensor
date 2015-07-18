/* 
 * File:   zb_handle.h
 * Author: mickael
 *
 * Created on 12 juillet 2015, 16:11
 */

#ifndef ZB_HANDLE_H
#define	ZB_HANDLE_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
  ZB_STATUS_NOT_JOINED,
  ZB_STATUS_JOINED,
  ZB_STATUS_IN_ERROR
} zb_statusT;

extern void zb_handle(void);
extern zb_statusT zb_handle_getStatus(void);
extern void zb_handle_sendData(uint16_t tempRaw, uint16_t humidityRaw);

#ifdef	__cplusplus
}
#endif

#endif	/* ZB_HANDLE_H */

