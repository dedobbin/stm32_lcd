#ifndef INC_HD44780_H_
#define INC_HD44780_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bool.h"

/** Instructions (not complete set) **/
#define HD44780_CLEAR 0x01
#define HD44780_HOME 0x02

/* Instructions to control behavior after each character is entered */
#define HD44780_INC_CRS 0b110
#define HD44780_INC_CRS_SHIFT 0b111 // Shift moves entire display depending on cursor inc/dec. Makes it look like display is moving
#define HD44780_DEC_CRS 0b100
#define HD44780_DEC_CRS_SHIFT 0b101

/* Display on/off */
#define HD44780_DISPLAY_OFF 0x08
#define HD44780_DISPLAY_ON_CRS_OFF 0x0C
#define HD44780_DISPLAY_ON_CRS_ON 0x0E
#define HD44780_DISPLAY_ON_CRS_BLNK 0X0F // Blinking cursor

/* Shift cursor and screen */
#define HD44780_CRS_LEFT 0x08
#define HD44780_CRS_RIGHT 0x14
#define HD44780_DISPLAY_LEFT 0x18
#define HD44780_DISPLAY_RIGHT 0x1C


void HD44780_init_I2C(I2C_HandleTypeDef* handle);
void HD44780_send_I2C(uint8_t data, bool is_instruction, bool backlight);
void HD44780_sends_I2C(char* str, bool backlight);
void HD44780_set_addr(uint8_t i2c_addr);

#endif /* INC_HD44780_H_ */
