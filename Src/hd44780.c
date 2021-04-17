#include "hd44780.h"
#include "stdlib.h"

#define PIN_ENABLE 1 << 2

uint8_t _i2c_addr = 0;
I2C_HandleTypeDef* _i2c_handle = NULL;

void HD44780_I2C_init(I2C_HandleTypeDef* handle)
{
	_i2c_handle = handle;
	HD44780_I2C_set_addr(0X27);

	// First instruction selects the 'function set': the amount of display lines, character font.
	// For now just support 2 lines which can only use 5x7 dots per character. 1 line also can handle 5x10 dots character font
	const uint8_t function_set = 0x38;
	HD44780_I2C_send(function_set, true, true);

	HD44780_I2C_send(HD44780_HOME, true, true);
	//HD44780_send_I2C(HD44780_DISPLAY_ON_CRS_OFF, true, true);
	HD44780_I2C_send(HD44780_DISPLAY_ON_CRS_BLNK, true, true);
	HD44780_I2C_send(HD44780_CLEAR, true, true);

	//HD44780_I2C_send(HD44780_INC_CRS, true, true);
}

void HD44780_I2C_send(uint8_t data, bool is_instruction, bool backlight)
{
	// 1 for DATA REGISTER, 0 for INSTRUCTION REGISTER
	const uint8_t REGISTER_SELECT = !is_instruction;

	uint8_t hi = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;
	const uint8_t backlight_mask = backlight ? 1 << 3 : 0;

	uint8_t data_arr[4] = {0};
	// When the 4 bit length is selected, data must be sent or received twice.
	data_arr[0] = hi | REGISTER_SELECT | PIN_ENABLE | backlight_mask;
	data_arr[1] = hi | REGISTER_SELECT | backlight_mask;
	data_arr[2] = lo | REGISTER_SELECT | PIN_ENABLE | backlight_mask;
	data_arr[3] = lo | REGISTER_SELECT | backlight_mask;

	while( HAL_I2C_IsDeviceReady(_i2c_handle, _i2c_addr, 1, HAL_MAX_DELAY) != HAL_OK);

	HAL_I2C_Master_Transmit(_i2c_handle, _i2c_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(5);
}

void HD44780_I2C_chunk_send(uint8_t* data, size_t data_size, bool is_instruction, bool backlight)
{
	const uint8_t REGISTER_SELECT = !is_instruction;
	const uint8_t backlight_mask = backlight ? 1 << 3 : 0;

	size_t data_arr_len = 4 * data_size;
	uint8_t* data_arr = malloc(data_arr_len);
	uint8_t* data_ptr = data;

	for (int i = 0; i < data_arr_len; i+=4){
		uint8_t hi = *data_ptr & 0xF0;
		uint8_t lo = (*data_ptr << 4) & 0xF0;

		data_arr[i+0] = hi | REGISTER_SELECT | PIN_ENABLE | backlight_mask;
		data_arr[i+1] = hi | REGISTER_SELECT | backlight_mask;
		data_arr[i+2] = lo | REGISTER_SELECT | PIN_ENABLE | backlight_mask;
		data_arr[i+3] = lo | REGISTER_SELECT | backlight_mask;

		data_ptr++;
	}


	while( HAL_I2C_IsDeviceReady(_i2c_handle, _i2c_addr, 1, HAL_MAX_DELAY) != HAL_OK);

	HAL_I2C_Master_Transmit(_i2c_handle, _i2c_addr, data_arr, data_arr_len, HAL_MAX_DELAY);
	HAL_Delay(5);
}

void HD44780_I2C_sends(char* str, bool backlight)
{
	while(*str){
		HD44780_I2C_send((uint8_t) *str, false, backlight);
		str++;
	}
}

void HD44780_I2C_cursor_pos(uint8_t pos, bool backlight)
{
	// Made for 40 character 4 line display
	// Line one shows first 16, second one shows 64-79
	HD44780_I2C_send(0x80 + pos, true, backlight);
}

void HD44780_I2C_cursor_pos_second_line(uint8_t pos, bool backlight)
{
	HD44780_I2C_cursor_pos(pos + 64, backlight);
}

void HD44780_I2C_set_addr(uint8_t i2c_addr)
{
	// Address depends on screen module. solder pads (usually labeled A0-A1-A2 ) can be used to change address
	// The device 7 bits address value in data sheet must be shifted to the left (says HAL_I2C_Master_Transmit)
	_i2c_addr = i2c_addr << 1;
}
