#ifndef I2C_HARD_H_
#define I2C_HARD_H_

#include <stdint.h>

#define I2C_OK			    0
#define I2C_ERROR_RW		1
#define I2C_ERROR_ADDR		2
#define I2C_TIME_OUT		3


void i2c_init(void);
uint8_t I2C_ByteWrite(uint8_t WriteAddr, uint8_t val);
uint8_t I2C_ByteRead( uint8_t ReadAddr, uint8_t *val);


#endif