#ifndef INC_HD44780_H_
#define INC_HD44780_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bool.h"

void HD44780_init_I2C(I2C_HandleTypeDef* handle);
void HD44780_send_I2C(uint8_t data, bool is_instruction, bool backlight);
void HD44780_sends_I2C(char* str, bool backlight);
void HD44780_set_addr(uint8_t i2c_addr);

#endif /* INC_HD44780_H_ */
