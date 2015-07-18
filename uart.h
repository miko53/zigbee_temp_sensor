/* 
 * File:   uart.h
 * Author: mickael
 *
 * Created on 11 juillet 2015, 18:34
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>

extern void uart_setup();
extern void uart_write(uint8_t size, uint8_t* buffer);
extern BOOL uart_read(uint8_t* buffer, uint8_t nbToRead);

#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

