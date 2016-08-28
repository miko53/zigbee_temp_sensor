
#include "filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct
{
  uint16_t in;
  uint16_t out;
} in_out;


in_out input_output_array[] =
{
  { 1, 1},
  { 5, 3},
  { 6, 4},
  { 8, 5},
  { 10, 6},
  { 18, 8},
  { 22, 10},
  { 25, 11},
  { 35, 16},
  { 45, 21},
  { 50, 26},
  { 35, 30},
  { 30, 32},
  { 30, 34},
  { 45, 36},
  { 50, 40},
};

in_out input_output_array_humd[] =
{
  { 1, 1},
  { 5, 3},
  { 6, 4},
  { 8, 5},
  { 10, 6},
  { 18, 8},
  { 22, 10},
  { 25, 11},
  { 35, 16},
  { 45, 21},
  { 50, 26},
  { 35, 30},
  { 30, 32},
  { 30, 34},
  { 45, 36},
  { 50, 40},
};


int main(int argc, char* argv[])
{
  fprintf(stdout, "filter_test\n");
  uint16_t v;

  for (uint32_t i = 0; i < sizeof(input_output_array) / sizeof(in_out); i++)
  {
    fprintf(stdout, "test %u insertion of %u", i, input_output_array[i].in);
    v = filter(TEMPERATURE_ID, input_output_array[i].in);
    fprintf(stdout, "--> out = %u\n", v);
    assert(v == input_output_array[i].out);
  }


  for (uint32_t i = 0; i < sizeof(input_output_array) / sizeof(in_out); i++)
  {
    fprintf(stdout, "test %u insertion of %u", i, input_output_array[i].in);
    v = filter(HUMIDITY_ID, input_output_array[i].in);
    fprintf(stdout, "--> out = %u\n", v);
    assert(v == input_output_array[i].out);
  }

  return EXIT_SUCCESS;




}