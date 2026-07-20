#include "sensors.h"
#include "pins.h"

#include <Arduino.h>

void sensors_init()
{
}

uint16_t sensor_read_light()
{
    return analogRead(LIGHT_SENSOR);
}

uint16_t sensor_read_temp()
{
    return analogRead(TEMP_SENSOR);
}
