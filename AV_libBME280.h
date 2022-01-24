#ifndef LIB_BME280_H
#define LIB_BME280_H
#include <stdint.h>

struct BME280_reading
{
  uint32_t temperature;
  uint32_t pressure;
};

void BME280_init(uint8_t I2C_address, struct BME280_reading * initial_readings);

void BME280_get_reading(uint8_t I2C_address, struct BME280_reading * reading);

#endif //LIB_BME280_H