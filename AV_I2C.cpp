// © Arthur Vie
// June 2020
//
// AV_I2C.cpp - I2C implementation
//

#define __AVR_ATmega2560__
#include <avr/io.h>
#include <stdint.h>

/*************************************/
/**** Arthur's simple I2C library ****/
/*************************************/
//I2C state defintions
#define BUS_ERROR 0x00
#define START 0x8
#define REP_START 0x10
#define SLA_W_ACK 0x18
#define SLA_W_NOACK 0x20
#define MT_DATA_ACK 0x28
#define ARB_LOST 0x38
#define SLA_R_ACK 0x40
#define SLA_R_NOACK 0x48
#define MR_DATA_ACK 0x50
#define MR_DATA_NOACK 0x58
#define NO_INFO 0xF8

//Setting up SCL
void I2C_init(void) {
    //set SCL to 400kHz
    TWSR = 0x00;
    TWBR = 0x0C;
    //enable TWI
    TWCR = 0b00000100;
}

//generate start condition
void start_condition(void) {
    TWCR = 0b10100100;
    while ((TWCR & 0b10000000) == 0);
}

//generate stop condition
void stop_condition(void) {
    TWCR = 0b10010100;
    while (TWCR & 0b00010000);
}

//write a byte on the I2C bus
void write_byte(uint8_t data_byte) {
    TWDR = data_byte;
    TWCR = 0b10000100;
    while ((TWCR & 0b10000000) == 0);
}

//read a byte on I2C with acknowledge
uint8_t read_byte_ACK(void) {
    TWCR = 0b11000100; 
    while ((TWCR & 0b10000000) == 0);
    return TWDR;
}

//read a byte on I2C with not acknowledge
uint8_t read_byte_NACK(void) {
    TWCR = 0b10000100;
    while ((TWCR & 0b10000000) == 0);
    return TWDR;
}

//get the status of the I2C hardware
uint8_t get_status(void) {
    return TWSR & 0b11111000;
}

//write a data byte to a specific device at a specific address
uint8_t write_to_device(uint8_t bus_address, uint8_t mem_address, uint8_t data_byte) {
  start_condition();
  //Serial.println(get_status());
  if (get_status() != START) {
    return 0;
  }
  write_byte((bus_address << 1) & 0b11111110);
  //Serial.println(get_status());
  if (get_status() != SLA_W_ACK) {
    return 0;
  }
  write_byte(mem_address);
  //Serial.println(get_status());
  if (get_status() != MT_DATA_ACK) {
    return 0;
  }
  write_byte(data_byte);
  //Serial.println(get_status());
  if (get_status() != MT_DATA_ACK) {
    return 0;
  }
  stop_condition();
  return 1;
}

//read a byte of data from a specific device at a specific address
uint8_t read_from_device(uint8_t bus_address, uint8_t mem_address) {
  uint8_t byte_read;
  start_condition();
  if (get_status() != START) {
    return 0;
  }
  write_byte((bus_address << 1) & 0b11111110);
  if (get_status() != SLA_W_ACK) {
    return 0;
  }
  write_byte(mem_address);
  if (get_status() != MT_DATA_ACK) {
    return 0;
  }
  start_condition();
  if (get_status() != REP_START) {
    return 0;
  }
  write_byte((bus_address << 1) | 0b00000001);
  if (get_status() != SLA_R_ACK) {
    return 0;
  }
  byte_read = read_byte_NACK();
  if (get_status() != MR_DATA_NOACK) {
    return 0;
  }
  stop_condition();
  return byte_read;
}

//read a string of consecutive bytes from an I2C device
uint8_t read_bytes_from_device(uint8_t bus_address, uint8_t mem_address, uint8_t * bytes_array, uint16_t num_bytes) {
  uint16_t i;
  start_condition();
  //Serial.println(get_status());
  if (get_status() != START) {
    return 0;
  }
  write_byte((bus_address << 1) & 0b11111110);
  //Serial.println(get_status());
  if (get_status() != SLA_W_ACK) {
    return 0;
  }
  write_byte(mem_address);
  //Serial.println(get_status());
  if (get_status() != MT_DATA_ACK) {
    return 0;
  }
  start_condition();
  //Serial.println(get_status());
  if (get_status() != REP_START) {
    return 0;
  }
  write_byte((bus_address << 1) | 0b00000001);
  //Serial.println(get_status());
  if (get_status() != SLA_R_ACK) {
    return 0;
  }
  for (i = 0; i < num_bytes; i ++) {
    if (i == (num_bytes - 1))  {
      bytes_array[i] = read_byte_NACK();
      //Serial.println(get_status());
      if (get_status() != MR_DATA_NOACK) {
        return 0;
      }
    }
    else {
      bytes_array[i] = read_byte_ACK();
      //Serial.println(get_status());
      if (get_status() != MR_DATA_ACK) {
        return 0;
      }
    }
  }
  stop_condition();
  return 1;
}

/************************************/
/************************************/
/************************************/