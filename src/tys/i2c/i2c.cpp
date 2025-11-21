#include "tys/i2c.h"
#include "back/back.hpp"

//	TODO: Move the defines to i2c.h and allow custom lengths

#ifndef TYS_ENV_I2C_BUFFER_SIZE_IN
#define TYS_ENV_I2C_BUFFER_SIZE_IN 8
#endif

#ifndef TYS_ENV_I2C_BUFFER_SIZE_OUT
#define TYS_ENV_I2C_BUFFER_SIZE_OUT 8
#endif

//
//	CONSTANTS
//

constexpr uint8_t
	_in_len = TYS_ENV_I2C_BUFFER_SIZE_IN,
	_in_mask = _in_len - 1,
	_out_len = TYS_ENV_I2C_BUFFER_SIZE_OUT,
	_out_mask = _out_len - 1;

//
//	LOCAL VARIABLES
//

uint8_t _addr;

uint8_t _in_buff[_in_len];
volatile uint8_t _in_head, _in_tail;

uint8_t _out_buff[_out_len];
volatile uint8_t _out_head, _out_tail;

//
//	EVENTS
//

inline i2c_back::addr_res_t i2c_back::ev_check_address(uint8_t Address, bool Write, uint8_t Raw) {
	i2c_back::addr_res_t res = 0;
	if (Address == _addr)
		res = i2c_back::ADDR_IS_MATCH | (Write ? i2c_back::ADDR_MODE_WRITE : i2c_back::ADDR_MODE_READ);

	return res;
}

inline void i2c_back::ev_get_byte(uint8_t& Byte) {
	//	buffers the index
	//	avoids a second load when its incremented
	uint8_t _t = _out_tail;

	//	buffer is currently empty
	if (_t == _out_head)
		Byte = 0x0;
	else {
		//	gets the byte, updates index and returns value
		uint8_t d = _out_buff[_t];
		_out_tail = (_t + 1) & _out_mask;
		Byte = d;
	}
}
inline void i2c_back::ev_push_byte(uint8_t& Data) {
	//	buffers the index
	//	avoids a second load when its incremented
	uint8_t _h = _in_head;
	//	no checks in input
	//	it would need a counter, but it slows down
	_in_buff[_h] = Data;
	_in_head = (_h + 1) & _in_mask;
}

//
//
//

void i2c::init(uint8_t addr) {
	_addr = addr;
	i2c_back::init();
}

uint8_t i2c::available() {
	return (_in_head - _in_tail) & _in_mask;
}
bool i2c::empty() {
	return _in_head == _in_tail;
}

void i2c::write(uint8_t byte) {
	auto _h = _out_head;

	_out_buff[_h] = byte;
	_out_head = (_h + 1) & _out_mask;
}
uint8_t i2c::read() {
	auto _t = _in_tail;

	auto b = _in_buff[_t];
	_in_tail = (_t + 1) & _in_mask;

	return b;
}

void i2c::flush(bool Input, bool Output) {
	if (Input)
		_in_head = 0, _in_tail = 0;
	if (Output)
		_out_head = 0, _out_tail = 0;
}
