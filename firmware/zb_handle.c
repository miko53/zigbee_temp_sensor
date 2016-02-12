#include <stdint.h>
#include <string.h>
#include "zb_handle.h"
#include "uart.h"
#include "zb.h"
#include "leds.h"

#define         MAX_SIZE_FRAME  (50)

static zb_statusT zb_status = ZB_STATUS_NOT_JOINED;

static uint8_t zb_frameReceived[MAX_SIZE_FRAME];

static void zigbee_appendChecksum(uint8_t* buffer, uint8_t* sizeFrame);

zb_statusT zb_handle_getStatus(void)
{
    return zb_status;
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
            if (sizeOfNextData <= (MAX_SIZE_FRAME-ZB_HEADER_SIZE))
                bSuccess = uart_read(zb_frameReceived+ZB_HEADER_SIZE, sizeOfNextData+1); //+1 for the checksum
            else
                bSuccess = FALSE;
        }
        else
            bSuccess = FALSE;
    }

    if (bSuccess)
    {
        bSuccess = zb_decodage(zb_frameReceived+ZB_HEADER_SIZE, sizeOfNextData + 1, &decodedFrame);
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
                  zb_status = ZB_STATUS_JOINED;
                else
                  zb_status = ZB_STATUS_NOT_JOINED;
                break;

            case ZIGBEE_TRANSMIT_STATUS:
                if (decodedFrame.status != 0)
                {
                  led_red_ON();
                  //led_green_OFF();
                }
                else
                {
                  //led_green_ON();
                }
                break;

            case ZIGBEE_RECEIVE_PACKET:
                break;
                
            default:
                break;
        }
    }
/*
    switch (zb_status)
    {
        case ZB_STATUS_NOT_JOINED:
            led_red_ON();
            break;

        case ZB_STATUS_JOINED:
            led_red_OFF();
            led_yellow_ON();
            break;

        case ZB_STATUS_IN_ERROR:
        default:
            
            break;
    }
*/
}

#define SENSOR_PROTOCOL_DATA_TYPE     (0x00)
#define SENSOR_PROTOCOL_DBG_TYPE      (0x01)
#define SENSOR_HYT221_TEMP            (0x01)
#define SENSOR_HYT221_HUM             (0x02)
#define SENSOR_VOLTAGE                (0x03)


#define OFFSET_SIZE         (1)
#define OFFSET_FRAMEID      (4)
#define OFFSET_COUNTER      (18)
#define OFFSET_TEMPERATURE (21)
#define OFFSET_HUMIDITY    (24)
#define OFFSET_VOLTAGE     (27)


const uint8_t frameData[] = 
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
  0x03,
  SENSOR_HYT221_TEMP,
  0x00,
  0x00,
  SENSOR_HYT221_HUM,
  0x00,
  0x00,
  SENSOR_VOLTAGE,
  0x00,
  0x00  
  //and checksum
};

static uint8_t zb_frameToSend[35];
static uint8_t zb_frameID = 2;
static uint8_t zb_counter;

void zb_handle_sendData(uint16_t tempRaw, uint16_t humidityRaw)
{
    memcpy(zb_frameToSend, frameData,  sizeof(frameData));
    zb_frameToSend[OFFSET_FRAMEID] = zb_frameID++;
    zb_frameToSend[OFFSET_COUNTER] = zb_counter++;
    zb_frameToSend[OFFSET_TEMPERATURE]   = tempRaw>>8;
    zb_frameToSend[OFFSET_TEMPERATURE+1] = tempRaw;
    zb_frameToSend[OFFSET_HUMIDITY]   = humidityRaw>>8;
    zb_frameToSend[OFFSET_HUMIDITY+1] = humidityRaw;
    zb_frameToSend[OFFSET_VOLTAGE]   = 0xFF;
    zb_frameToSend[OFFSET_VOLTAGE+1] = 0xFF;
    zb_frameToSend[OFFSET_SIZE] = ((sizeof(frameData)-ZB_HEADER_SIZE) & 0xFF00)>>8;
    zb_frameToSend[OFFSET_SIZE+1] = ((sizeof(frameData)-ZB_HEADER_SIZE) & 0x00FF);

    uint8_t frameSize = sizeof(frameData);
    zigbee_appendChecksum(zb_frameToSend, &frameSize);
    uart_write(frameSize, zb_frameToSend);
}


static void zigbee_appendChecksum(uint8_t* buffer, uint8_t* sizeFrame)
{
  buffer[*sizeFrame] = zb_doChecksum(&buffer[ZB_HEADER_SIZE], &buffer[*sizeFrame] - &buffer[ZB_HEADER_SIZE]);
  (*sizeFrame)++;
}

