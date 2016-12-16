#include <stdint.h>
#include <string.h>
#include "zb_handle.h"
#include "uart.h"
#include "zb.h"
#include "leds.h"
#include "timer.h"
#include "uart_loc.h"

#define MAX_SIZE_FRAME                (50)

#define SENSOR_PROTOCOL_DATA_TYPE     (0x00)
#define SENSOR_PROTOCOL_DBG_TYPE      (0x01)
#define SENSOR_HYT221_TEMP            (0x01)
#define SENSOR_HYT221_HUM             (0x02)
#define SENSOR_VOLTAGE                (0x03)

#define STATUS_NORMAL_DATA            (0x03)
#define STATUS_INVALID                (0x00)

#define OFFSET_SIZE                      (1)
#define OFFSET_FRAMEID                   (4)
#define OFFSET_COUNTER                  (18)
#define OFFSET_TEMPERATURE_STATUS       (21)
#define OFFSET_TEMPERATURE              (22)
#define OFFSET_HUMIDITY_STATUS          (25)
#define OFFSET_HUMIDITY                 (26)
#define OFFSET_VOLTAGE_STATUS           (29)
#define OFFSET_VOLTAGE                  (30)

/*const uint8_t zb_forceDisassociation[] =
{
  ZB_START_DELIMITER, 0x00, 0x04, ZIGBEE_API_AT_CMD, 0x00, 'F', 'R', 0x5f
};*/

static uint8_t zb_frameToSend[] =
{
  ZB_START_DELIMITER,
  0, //size
  4, //size without the checksum
  ZIGBEE_API_TRANSMIT_REQUEST,
  2, //frameID
  0x00, //coordinator @1
  0x00, //coordinator @2
  0x00, //coordinator @3
  0x00, //coordinator @4
  0x00, //coordinator @5
  0x00, //coordinator @6
  0x00, //coordinator @7
  0x00, //coordinator @8
  0xFF, //coord 16bits ZIGBEE_UNKNOWN_16B_ADDR
  0xFE, //coord 16bits ZIGBEE_UNKNOWN_16B_ADDR
  0x00, //broadcast radius
  0x00, //option
  //payload
  SENSOR_PROTOCOL_DATA_TYPE,
  0x00,//counter
  0x03,//number of sensor
  SENSOR_HYT221_TEMP,
  0x00, //status
  0x00,
  0x00,
  SENSOR_HYT221_HUM,
  0x00, //status
  0x00,
  0x00,
  SENSOR_VOLTAGE,
  0x00, //status
  0x00,
  0x00,
  //and checksum
  0x00
};

//#define DBG_FRAME
//#define USE_FRAME_ID

#ifdef DBG_FRAME
static uint8_t zb_dbgFrame[] =
{
  ZB_START_DELIMITER,
  0, //size
  4, //size without the checksum
  ZIGBEE_API_TRANSMIT_REQUEST,
  0, //frameID
  0x00, //coordinator @1
  0x00, //coordinator @2
  0x00, //coordinator @3
  0x00, //coordinator @4
  0x00, //coordinator @5
  0x00, //coordinator @6
  0x00, //coordinator @7
  0x00, //coordinator @8
  0xFF, //coord 16bits ZIGBEE_UNKNOWN_16B_ADDR
  0xFE, //coord 16bits ZIGBEE_UNKNOWN_16B_ADDR
  0x00, //broadcast radius
  0x00, //option
  //payload
  SENSOR_PROTOCOL_DBG_TYPE,
  0x00,//counter
  0x00,
  0x00,
  0x00,
  //and checksum
  0x00
};
#endif

#ifdef USE_FRAME_ID
static uint8_t zb_frameID = 1;
static uint8_t zb_currentFrameID;
static int8_t zb_currentAck;
#endif /* USE_FRAME_ID */

static uint8_t zb_counter;
#ifdef DBG_FRAME
static uint8_t zb_dbg1;
#endif

typedef struct
{
  uint16_t tempRaw;
  uint8_t tempStatus;
  uint16_t humidityRaw;
  uint8_t humidityStatus;
  uint16_t battRaw;
  uint8_t battStatus;
} sensor;

static sensor sensor_data;
static zb_statusT zb_status = ZB_STATUS_NOT_JOINED;
static uint8_t zb_frameReceived[MAX_SIZE_FRAME];

static void zigbee_appendChecksum(uint8_t* buffer, uint8_t* sizeFrame);

zb_statusT zb_handle_getStatus(void)
{
  return zb_status;
}

void zb_handle_setTempRaw(uint16_t tempRaw)
{
  sensor_data.tempRaw = tempRaw;
  sensor_data.tempStatus = STATUS_NORMAL_DATA;
}

void zb_handle_setHumidityRaw(uint16_t humidityRaw)
{
  sensor_data.humidityRaw = humidityRaw;
  sensor_data.humidityStatus = STATUS_NORMAL_DATA;
}

void zb_handle_setbatVolt(uint16_t battVoltage)
{
  sensor_data.battRaw = battVoltage;
  sensor_data.battStatus = STATUS_NORMAL_DATA;
}

void zb_handle_resetStatus()
{
  sensor_data.battStatus = STATUS_INVALID;
  sensor_data.humidityStatus = STATUS_INVALID;
  sensor_data.tempStatus = STATUS_INVALID;
}


void zb_handle_sendData()
{
#ifdef USE_FRAME_ID
  zb_currentFrameID = zb_frameID;
  zb_frameID++;
  if (zb_frameID == 0)
  {
    zb_frameID = 1;
  }
  zb_currentAck = -1;
  zb_frameToSend[OFFSET_FRAMEID] = zb_currentFrameID;
#else
  zb_frameToSend[OFFSET_FRAMEID] = 0;
#endif /* USE_FRAME_ID */

  zb_frameToSend[OFFSET_COUNTER] = zb_counter++;
  zb_frameToSend[OFFSET_TEMPERATURE_STATUS]   = sensor_data.tempStatus;
  zb_frameToSend[OFFSET_TEMPERATURE]   = sensor_data.tempRaw >> 8;
  zb_frameToSend[OFFSET_TEMPERATURE + 1] = sensor_data.tempRaw;
  zb_frameToSend[OFFSET_HUMIDITY_STATUS]   = sensor_data.humidityStatus;
  zb_frameToSend[OFFSET_HUMIDITY]   = sensor_data.humidityRaw >> 8;
  zb_frameToSend[OFFSET_HUMIDITY + 1] = sensor_data.humidityRaw;
  zb_frameToSend[OFFSET_VOLTAGE_STATUS]   = sensor_data.battStatus;
  zb_frameToSend[OFFSET_VOLTAGE]   = sensor_data.battRaw >> 8;
  zb_frameToSend[OFFSET_VOLTAGE + 1] = sensor_data.battRaw;
  zb_frameToSend[OFFSET_SIZE] = ((sizeof(zb_frameToSend) - 1 - ZB_HEADER_SIZE) & 0xFF00) >> 8;
  zb_frameToSend[OFFSET_SIZE + 1] = ((sizeof(zb_frameToSend) - 1 - ZB_HEADER_SIZE) & 0x00FF);

  uint8_t frameSize = sizeof(zb_frameToSend) - 1;
  zigbee_appendChecksum(zb_frameToSend, &frameSize);

  uart_write(frameSize, zb_frameToSend);
}

/*void zb_handle_force_disassociation()
{
  uart_write(sizeof(zb_forceDisassociation), zb_forceDisassociation);
}*/

#ifdef DBG_FRAME
void zb_handle_sendDbgData()
{
  zb_dbgFrame[19] = zb_currentAck;
  zb_dbgFrame[20] = zb_dbg1;
  zb_dbgFrame[21] = zb_currentFrameID;
  uint8_t frameSize = sizeof(zb_dbgFrame) - 1;
  zb_dbgFrame[OFFSET_SIZE] = ((frameSize - ZB_HEADER_SIZE) & 0xFF00) >> 8;
  zb_dbgFrame[OFFSET_SIZE + 1] = ((frameSize - ZB_HEADER_SIZE) & 0x00FF);
  zigbee_appendChecksum(zb_dbgFrame, &frameSize);
  uart_write(frameSize, zb_dbgFrame);
  zb_dbg1 = 0;
}
#endif

static void zigbee_appendChecksum(uint8_t* buffer, uint8_t* sizeFrame)
{
  buffer[*sizeFrame] = zb_doChecksum(&buffer[ZB_HEADER_SIZE], &buffer[*sizeFrame] - &buffer[ZB_HEADER_SIZE]);
  (*sizeFrame)++;
}


void zb_handle(void)
{
  BOOL bSuccess;
  uint16_t sizeOfNextData;
  zigbee_decodedFrame decodedFrame;

  bSuccess = uart_read(zb_frameReceived, ZB_HEADER_SIZE);
  if (bSuccess)
  {
    if (zb_frameReceived[0] == ZB_START_DELIMITER)
    {
      sizeOfNextData = (((uint16_t) zb_frameReceived[1]) << 8) | (zb_frameReceived[2]);
      if (sizeOfNextData <= (MAX_SIZE_FRAME - ZB_HEADER_SIZE))
      {
        bSuccess = uart_read(zb_frameReceived + ZB_HEADER_SIZE, sizeOfNextData + 1);  //+1 for the checksum
      }
      else
      {
        bSuccess = FALSE;
      }
    }
    else
    {
      bSuccess = FALSE;
    }
  }

  if (bSuccess)
  {
    bSuccess = zb_decodage(zb_frameReceived + ZB_HEADER_SIZE, sizeOfNextData + 1, &decodedFrame);
  }

  if (bSuccess)
  {
    switch (decodedFrame.type)
    {
      case ZIGBEE_API_AT_CMD:
        break;

      case ZIGBEE_API_TRANSMIT_REQUEST:
        break;

      case ZIGBEE_AT_COMMAND_RESPONSE:
        break;

      case ZIGBEE_MODEM_STATUS:
        if (decodedFrame.status == 0x02)
        {
          zb_status = ZB_STATUS_JOINED;
          leds_glitch(LED_GREEN);
        }
        else if (decodedFrame.status == 0x03)
        {
          zb_status = ZB_STATUS_NOT_JOINED;
          leds_glitch(LED_RED);
        }
        break;

      case ZIGBEE_TRANSMIT_STATUS:
#ifdef USE_FRAME_ID
        if (zb_currentFrameID == decodedFrame.frameID)
        {
          zb_currentAck = decodedFrame.status;
        }
        else
        {
          ;
        }
#endif /* USE_FRAME_ID */
        break;

      case ZIGBEE_RECEIVE_PACKET:
        break;

      default:
        break;
    }
  }
}

BOOL zb_handle_waitAck(void)
{
#ifdef USE_FRAME_ID
  uint8_t retryCounter;
  BOOL bAckReceived;

  bAckReceived = FALSE;
  retryCounter = 0;
  //5
  //19 max retry = 3 --> 4.8s
  timer0_wait_262ms();

  while ((retryCounter < 18) && (bAckReceived == FALSE))
  {
    zb_handle();
    if (zb_currentAck == 0)
    {
      bAckReceived = TRUE;
    }

    timer0_wait_262ms(); //wait end of transmission
    retryCounter++;
  }

  return bAckReceived;
#else
  timer0_wait_262ms();
  timer0_wait_262ms();
  return TRUE;
#endif
}
