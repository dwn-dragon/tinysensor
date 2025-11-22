#pragma once

#ifndef TYS_I2C_BACK_BACK_INCLUDE
#define TYS_I2C_BACK_BACK_INCLUDE

#include "tys/core.h"

#include <stdint.h>

namespace i2c_back
{

	using addr_res_t = uint8_t;

	enum : addr_res_t
	{
		//	Flag for match/no match
		ADDR_NO_MATCH	= 0b00,
		ADDR_IS_MATCH	= 0b10,

		//	Flag for write/read
		ADDR_MODE_READ	= 0b00,
		ADDR_MODE_WRITE	= 0b01
	};

	//	INTERFACE
	//	Methods that that the BACK layer adds for manipulation

	static inline void init();

	//	MASK
	//	Event methods that the BACK layer calls to let the FRONT layer act

	//	Checks the input address for a match. Should report if it's READ or WRITE
	//	addr_res_t is a flag variable, both mode and match should be set
	static inline addr_res_t ev_check_address(uint8_t Address, bool Write, uint8_t Raw);

	//	Moves a byte from the back layer to the front layer
	static inline void ev_push_byte(uint8_t Byte);
	//	Moves a byte from the front layer to the back layer
	static inline uint8_t ev_get_byte();

}

#include "usi.hpp"

#endif	//	Include guard
