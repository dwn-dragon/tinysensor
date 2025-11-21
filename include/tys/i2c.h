#pragma once

#ifndef TYS_I2C_INCLUDE
#define TYS_I2C_INCLUDE

#include "tys/core.h"
#include <stdint.h>

namespace i2c
{

	void init(uint8_t Address);
	
	uint8_t available();
	bool empty();

	void write(uint8_t byte);
	uint8_t read();

	void flush(bool Input, bool Output);

}

#endif	//	Include guard
