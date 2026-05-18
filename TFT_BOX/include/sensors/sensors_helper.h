#pragma once

#include "Arduino.h"
#include <Wire.h>

#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

#define TCAADDR 0x70

typedef enum { 
    SENSORS_STARTED_OK,
    SENSOR_BMP085_INIT_KO,
    SENSOR_MPU6050_INIT_KO,
    SENSOR_UNKNOWN_ERROR,
} sensors_init_t;

typedef struct {

    struct {
        float temp_c;
        float temp_f;
        int32_t pressure;
        int32_t sea_level_pressure;
        float altitude;
    } BMP085;

    struct {


    } MPU6050;

    

} sensors_information_t;








#ifdef __cplusplus
}
#endif /**< _cplusplus */