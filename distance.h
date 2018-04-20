/*
 * Header file for distance.c file
*/

#ifndef DISTANCE_H_
#define DISTANCE_H_

#include "Timer.h"
#include "lcd.h"

void adc_init(void);

void adc_receive(void);

unsigned ADC_read(char channel);

double convert_distance(int quantization);

#endif /* DISTANCE_H_ */
