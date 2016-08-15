/*
 * File:   status.h
 * Author: mickael
 *
 * Created on 11 juillet 2015, 18:20
 */

#ifndef STATUS_H
#define	STATUS_H

#ifdef	__cplusplus
extern "C" {
#endif


typedef enum
{
  STATUS_OK,
  STATUS_ERROR,
  STATUS_EBUSY
} STATUS_T;



#ifdef	__cplusplus
}
#endif

#endif	/* STATUS_H */

