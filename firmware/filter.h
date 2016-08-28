/*
 * File:   filter.h
 * Author: mickael
 *
 * Created on 27 août 2016, 13:31
 */

#ifndef FILTER_H
#define	FILTER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum
{
  TEMPERATURE_ID,
  HUMIDITY_ID,
  FILTER_ID_MAX
} filter_id;

extern uint16_t filter(filter_id id, uint16_t value);


#ifdef	__cplusplus
}
#endif

#endif	/* FILTER_H */

