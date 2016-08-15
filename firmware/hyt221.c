#include "global.h"
#include <xc.h>
#include <stdint.h>
#include "hyt221.h"
#include "leds.h"
#include "i2c.h"


// I2C addr is 0x28 which give the following addr with R/W bit.
#define HYT221_I2C_ADDR_W        (0x50)
#define HYT221_I2C_ADDR_R        (0x51)

//static uint16_t humidity_raw;
//static uint16_t temp_raw;

static int8_t gData[4];
//static int32_t nbStale;


STATUS_T hyt221_launch_acq()
{
  i2c_trig_start();

  //send I2C device Addr on I2C bus
  SSPBUF = HYT221_I2C_ADDR_W;

  i2c_wait_ready();

  //read I2C Ack
  //if the device doesn't respond, indicate error with RED led
  if (SSPCON2bits.ACKSTAT == 1)
  {
    leds_red_and_yellow_glitch();
    return STATUS_ERROR;
  }

  i2c_wait_idle();
  i2c_trig_stop();
  NOP();
  return STATUS_OK;
}


STATUS_T hyt221_operation()
{
  int8_t index;
  BOOL bDeviceDataAreStale;
  bDeviceDataAreStale = FALSE;

  //generate start condition
  i2c_trig_start();

  //send I2c device address in reading
  SSPBUF = HYT221_I2C_ADDR_R;
  i2c_wait_ready();

  //read ACK
  if (SSPCON2bits.ACKSTAT == 1)
  {
    leds_red_and_yellow_glitch();
    i2c_trig_stop();
    return STATUS_ERROR;
  }

  index = 0;
  while (index < 4)
  {
    i2c_wait_idle();
    SSPCON2bits.RCEN = 1; // start I2C reception mode
    //    while (SSPCON2bits.RCEN == 1);
    i2c_wait_ready();

    gData[index] = SSPBUF;
    if (index == 0)
    {
      //check stale bit
      if ((gData[0] & 0x40) == 0x40)
      {
        //nbStale++;
        SSPCON2bits.ACKDT = 1;//1;  //set NACK
        index = 3;
        bDeviceDataAreStale = TRUE;
      }
      else
      {
        SSPCON2bits.ACKDT = 0; //set ACK
      }
    }
    else if (index == 3)
    {
      SSPCON2bits.ACKDT = 1;//1;  //set NACK
    }
    else
    {
      SSPCON2bits.ACKDT = 0;  //set ACK
    }

    i2c_wait_idle();

    //send the ACK or NACK bit on I2C bus
    SSPCON2bits.ACKEN = 1;
    i2c_wait_ready();
    index++;
  }

  i2c_wait_idle();
  i2c_trig_stop();

  if (bDeviceDataAreStale == TRUE)
  {
    return STATUS_EBUSY;
  }

  return STATUS_OK;
}


uint16_t hyt221_getTemp()
{
  uint16_t temp_raw;
  temp_raw = gData[2];
  temp_raw = temp_raw << 8;
  temp_raw += gData[3];
  temp_raw = temp_raw >> 2;
  return temp_raw;
}

uint16_t hyt221_getHumidity()
{
  uint16_t humidity_raw;
  humidity_raw = (gData[0] & 0x3F);
  humidity_raw = humidity_raw << 8;
  humidity_raw += gData[1];

  return humidity_raw;
}



#if 0
uint32_t humidity;
uint32_t temp;


static void do_calculate_temp_humd()
{
  humidity_raw = (gData[0] & 0x3F);
  humidity_raw = humidity_raw << 8;
  humidity_raw += gData[1];

  temp_raw = gData[2];
  temp_raw = temp_raw << 8;
  temp_raw += gData[3];
  temp_raw = temp_raw >> 2;

  humidity = ((uint32_t) humidity_raw) * 1000;
  humidity = humidity / 16383;

  temp = (uint32_t) temp_raw * 1650;
  temp = temp - 6553200;
  temp = temp / 16383;
}
#endif