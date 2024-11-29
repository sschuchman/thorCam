#ifndef SENSOR_H
#define SENSOR_H

#include "movement.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "ads1115.h"
#include "l293d.h"

// External variable declarations
extern ads1115_t adc1, adc2;
extern int16_t diff_0_1_adc1, diff_2_3_adc1, diff_0_1_adc2, diff_2_3_adc2;
extern int16_t sensor_values[4];

extern volatile bool mutex;
extern volatile bool stop_flag;

void read_ads111_task();
void calibrate();

#endif // SENSOR_H