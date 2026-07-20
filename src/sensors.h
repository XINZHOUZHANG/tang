#pragma once

#include <stdint.h>

void sensors_init();
uint16_t sensor_read_light();
uint16_t sensor_read_temp();
