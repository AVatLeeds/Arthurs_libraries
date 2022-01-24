// © Arthur Vie
// June 2020
//
// AV_libBME280.cpp - BME280 sensor implementation
//

#include <stdint.h>
#include "AV_I2C.h"
#include "AV_libBME280.h"

struct BME280_calibration_parameters
{
    uint16_t T1;
    int16_t T2, T3;
    int32_t t_fine;
    uint16_t P1;
    int16_t P2, P3, P4, P5, P6, P7, P8, P9;
    uint8_t H1;
    int16_t H2;
    uint8_t H3;
    int16_t H4, H5;
    int8_t H6;
} cal_params;

uint8_t BME280_get_calibration (uint8_t I2C_address)
{
    // doesn't presently read calibration parameters for humidity
    // TODO - humidity functionality should be added
    uint8_t bytes_array[24];
    if (read_bytes_from_device(I2C_address, 0x88, bytes_array, 24))
    {
        cal_params.T1 = ((uint16_t)bytes_array[1] << 8) | bytes_array[0];
        cal_params.T2 = ((int16_t)bytes_array[3] << 8) | bytes_array[2];
        cal_params.T3 = ((int16_t)bytes_array[5] << 8) | bytes_array[4];
        cal_params.P1 = ((uint16_t)bytes_array[7] << 8) | bytes_array[6];
        cal_params.P2 = ((int16_t)bytes_array[9] << 8) | bytes_array[8];
        cal_params.P3 = ((int16_t)bytes_array[11] << 8) | bytes_array[10];
        cal_params.P4 = ((int16_t)bytes_array[13] << 8) | bytes_array[12];
        cal_params.P5 = ((int16_t)bytes_array[15] << 8) | bytes_array[14];
        cal_params.P6 = ((int16_t)bytes_array[17] << 8) | bytes_array[16];
        cal_params.P7 = ((int16_t)bytes_array[19] << 8) | bytes_array[18];
        cal_params.P8 = ((int16_t)bytes_array[21] << 8) | bytes_array[20];
        cal_params.P9 = ((int16_t)bytes_array[23] << 8) | bytes_array[22];
    }
    else
    {
        return 0;
    }
    return 1;
}

int32_t BME280_temperature_compensate(int32_t raw_temperature)
{
    // algorithm copied from BME280 datasheet
    int32_t var1, var2, T;
    var1 = ((((raw_temperature >> 3) - ((int32_t)(cal_params.T1) << 1))) * ((int32_t)(cal_params.T2))) >> 11;
    var2  =(((((raw_temperature >> 4) - ((int32_t)(cal_params.T1))) * ((raw_temperature >> 4) - ((int32_t)(cal_params.T1)))) >> 12) * ((int32_t)(cal_params.T3))) >> 14;
    cal_params.t_fine = var1 + var2;
    T  = (cal_params.t_fine * 5 + 128) >> 8;
    return T;    
}

uint32_t BME280_pressure_compensate(int32_t raw_pressure)
{
    // algorithm copied from BME280 datasheet
    // returns pressure in Pa as an unsigned 32 bit integer
    // output value of “96386” equals 96386 Pa = 963.86 hPa
    int32_t var1, var2;
    uint32_t p;
    var1 = (((int32_t)(cal_params.t_fine)) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11 ) * ((int32_t)(cal_params.P6));
    var2 = var2 + ((var1 * ((int32_t)(cal_params.P5))) << 1);
    var2 = (var2 >> 2) + (((int32_t)(cal_params.P4)) << 16);
    var1 = ((((cal_params.P3) * (((var1 >> 2) * (var1 >> 2)) >> 13 )) >> 3) + ((((int32_t)(cal_params.P2)) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)(cal_params.P1))) >> 15);
    if(var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = (((uint32_t)(((int32_t) 1048576) - raw_pressure) - (var2 >> 12))) * 3125;
    if(p < 0x80000000)
    {
        p = (p << 1) / ((uint32_t)var1);
    } 
    else
    {
        p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)(cal_params.P9)) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(p >> 2)) * ((int32_t)(cal_params.P8))) >> 13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + cal_params.P7) >> 4));
    return p;
}

// TODO - implement humidity compensation routine
// Temporarily changed type here to void
// TODO - if we have time we should implement proper error status and an error handler 
void BME280_init(uint8_t I2C_address, struct BME280_reading * initial_readings) 
{
  uint32_t raw_temperature;
  uint32_t raw_pressure;
  uint8_t byte_holder[3];
  uint8_t i = 0;

  if (!BME280_get_calibration(I2C_address)) 
  {
    //Error retrieving calibration parameters
    return;
  }
  for (i = 0; i < 10; i ++)
  {
    if (!write_to_device(I2C_address, 0xF4, 0x25))
    {
      //Error requesting measurements
      return;
    }
    if (!read_bytes_from_device(I2C_address, 0xFA, byte_holder, 3))
    {
      //Error reading temperature measurement
      return;
    }
    raw_temperature = (((uint32_t)byte_holder[0] << 16) | ((uint32_t)byte_holder[1] << 8) | (byte_holder[2] & 0xF)) >> 4;
    initial_readings->temperature = (initial_readings->temperature + BME280_temperature_compensate(raw_temperature)) / 2;
    if (!read_bytes_from_device(I2C_address, 0xF7, byte_holder, 3))
    {
      //Error reading pressure measurement
    }
    raw_pressure = (((uint32_t)byte_holder[0] << 16) | ((uint32_t)byte_holder[1] << 8) | (byte_holder[2] & 0xF)) >> 4;
    initial_readings->pressure = (initial_readings->pressure + BME280_pressure_compensate(raw_pressure)) / 2;
  }
  return;
}

void BME280_get_reading(uint8_t I2C_address, struct BME280_reading * reading) 
{
  uint32_t raw_temperature;
  uint32_t raw_pressure;
  uint8_t byte_holder[3];

  if (!write_to_device(I2C_address, 0xF4, 0x25))
  {
    //Error requesting measurements
    return;
  }
  if (!read_bytes_from_device(I2C_address, 0xFA, byte_holder, 3))
  {
    //Error reading temperature measurement
    return;
  }
  raw_temperature = (((uint32_t)byte_holder[0] << 16) | ((uint32_t)byte_holder[1] << 8) | (byte_holder[2] & 0xF)) >> 4;
  reading->temperature = BME280_temperature_compensate(raw_temperature);
  if (!read_bytes_from_device(I2C_address, 0xF7, byte_holder, 3))
  {
    //Error reading pressure measurement
  }
  raw_pressure = (((uint32_t)byte_holder[0] << 16) | ((uint32_t)byte_holder[1] << 8) | (byte_holder[2] & 0xF)) >> 4;
  reading->pressure = BME280_pressure_compensate(raw_pressure);
  return;
}