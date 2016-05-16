
#include <xc.h>
#include "global.h"
#include "i2c.h"
#include "i2c_loc.h"

BOOL i2c_interupt_done;

void i2c_setup()
{
  //Errata on PIC4520
  //put I2C pins to output
  //and set output to 0
  //after put I2C pins to input
  TRISC &= ~0x18;
  LATCbits.LATC3 = 0;
  LATCbits.LATC4 = 0;
  __delay_us(100);
  //configure PORT C RC3/RC4 for I2C in input
  TRISC |= 0x18;

  //TRISE = 0x0; //PORT E en sortie

  //setup I2C bus
  SSPSTAT = 0x80; //slew rate disable --> for speed to 125kHz
  SSPADD  = 0x01;// SSPADD =  0x01; //I2C speed set to 125kHz
  SSPCON1 = 0x28; //RC3 and RC4 in I2C master mode
  SSPCON2 = 0x00; //I2C in idle

  //clear interrupt register I2C interupt and bus collision interupt
  i2c_interupt_done = FALSE;
  PIR1bits.SSPIF = 0;
  PIR2bits.BCLIF = 0;
  PIE1bits.SSPIE = 1;  //ADDD
}

void i2c_wait_ready()
{
    OSCCONbits.IDLEN = 1;
    SLEEP();
    OSCCONbits.IDLEN = 0;
    while (i2c_interupt_done == FALSE);
    //while (PIR1bits.SSPIF == 0);
    //PIR1bits.SSPIF = 0;
    i2c_interupt_done = FALSE;
}

void i2c_wait_idle()
{
    while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_W));
}


void i2c_trig_start()
{
  i2c_wait_idle();
  //generate start condition
  SSPCON2bits.SEN = 1;
  i2c_wait_ready();
}

void i2c_trig_stop()
{
  //set Stop sequence
  SSPCON2bits.PEN = 1;
  i2c_wait_ready();
}
