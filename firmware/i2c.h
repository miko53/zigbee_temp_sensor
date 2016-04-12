/*
 * File:   i2c.h
 * Author: mickael
 *
 * Created on 11 juillet 2015, 18:15
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

extern void i2c_wait_ready();
extern void i2c_wait_idle();
extern void i2c_trig_start();
extern void i2c_trig_stop();
extern void i2c_setup();

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

