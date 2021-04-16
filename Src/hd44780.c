#include "hd44780.h"

#define PIN_ENABLE 1 << 2

uint8_t _i2c_addr = 0;
I2C_HandleTypeDef* _i2c_handle = NULL;

void HD44780_init_I2C(I2C_HandleTypeDef* handle)
{
	_i2c_handle = handle;
	HD44780_set_addr(0X27);

	HD44780_send_I2C(0x38, true, true); // 2 lines, 5x7
	HD44780_send_I2C(0x2, true, true);	// Return home
	HD44780_send_I2C(0xC, true, true);  // Turn on (with cursor off)
	//LCD_command(0x0F, handle, I2C_addr); //Turn on (cursor blinking)
	HD44780_send_I2C(0x1, true, true);	// Clear screen
}

void HD44780_send_I2C(uint8_t data, bool is_instruction, bool backlight)
{
	//1 for DATA REGISTER, 0 for INSTRUCTION REGISTER
	const uint8_t REGISTER_SELECT = !is_instruction;

	uint8_t hi = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4] = {0};
	data_arr[0] = hi | REGISTER_SELECT | PIN_ENABLE;
	data_arr[1] = 0;	//Trigger send by falling edge
	data_arr[2] = lo | REGISTER_SELECT | PIN_ENABLE;
	data_arr[3] |= backlight ? 1 << 3 : 0; //Next send, but control backlight

	while( HAL_I2C_IsDeviceReady(_i2c_handle, _i2c_addr, 1, HAL_MAX_DELAY) != HAL_OK);

	HAL_I2C_Master_Transmit(_i2c_handle, _i2c_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(5);
}

void HD44780_sends_I2C(char* str, bool backlight)
{
	while(*str){
		HD44780_send_I2C((uint8_t) *str, false, backlight);
		str++;
	}
}

void HD44780_set_addr(uint8_t i2c_addr)
{
	// Address depends on screen module. solder pads (usually labeled A0-A1-A2 ) can be used to change address
	// The device 7 bits address value in data sheet must be shifted to the left (says HAL_I2C_Master_Transmit)
	_i2c_addr = i2c_addr << 1;
}
