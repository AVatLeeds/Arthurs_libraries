#ifndef AV_I2C_H
#define AV_I2C_H
#include <stdint.h>

void I2C_init(void);

uint8_t read_bytes_from_device(uint8_t bus_address, uint8_t mem_address, uint8_t * bytes_array, uint16_t num_bytes);

uint8_t read_from_device(uint8_t bus_address, uint8_t mem_address);

uint8_t write_to_device(uint8_t bus_address, uint8_t mem_address, uint8_t data_byte);

#endif // AV_I2C_H
