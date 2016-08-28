
#include "filter.h"
#include <stdint.h>

#define     FILTER_NB   (8)

typedef struct
{
  uint16_t data[FILTER_NB];
  uint8_t offset;
  uint8_t nb;
} filter_storage;

static filter_storage filter_data[FILTER_ID_MAX];

uint16_t filter(filter_id id, uint16_t value)
{
  uint16_t filtered;
  uint32_t sum = 0;

  if (filter_data[id].offset >= FILTER_NB)
  {
    filter_data[id].offset = 0;
  }

  filter_data[id].data[filter_data[id].offset] = value;
  filter_data[id].offset++;

  if (filter_data[id].nb < FILTER_NB)
  {
    filter_data[id].nb++;
  }


  for (uint8_t i = 0; i < filter_data[id].nb; i++)
  {
    sum += filter_data[id].data[i];
  }

  filtered = sum / filter_data[id].nb;
  /*
  switch(filter_data[id].nb)
  {
      case 2:
          filtered = sum >> 1;
          break;
      case 4:
          filtered = sum >> 2;
          break;
      case 8:
          filtered = sum >> 3;
          break;

      case 3:
      case 5:
      case 6:
      case 7:
      default:
          filtered = sum / filter_data[id].nb;
          break;

      case 1:
          filtered = value;
          break;
  }
  */
  return filtered;
}

