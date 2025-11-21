#pragma once

#ifndef TYS_I2C_BACK_USI_INCLUDE
#define TYS_I2C_BACK_USI_INCLUDE

#include <stdint.h>
#include <avr/interrupt.h>

//	TODO: Better dir tree
//	Adding a header before this would allow for multiple hardwares supported with the same interface
//	TODO: Keep collision flag
//	Ensures that the collision detection flag is kept in case the user wants it
//	TODO: Wrap status register sets
//	USISR is often changed to the same value, it can wrapped in a utility function
//	TODO: Use stop condition
//	Add some additional behaviour to use the stop flag to avoid issues
	
//
//	COSTANTS
//

enum _state_t : uint8_t
{
	ST_CHECK_ADDRESS,

	//	Write OPeration

	ST_WOP_WRITE_DATA,
	ST_WOP_READ_ACK,
	ST_WOP_CHECK_ACK,

	//	Read OPeration

	ST_ROP_READ_DATA,
	ST_ROP_WRITE_ACK,
};

//	ISR vectors
#define VEC_I2C_START		USI_START_vect
#define VEC_I2C_OVERFLOW	USI_OVF_vect

//	Registers
#define I2C_REG_DIR	DDRB
#define I2C_REG_OUT	PORTB
#define I2C_REG_IN	PINB

//	Pins
constexpr uint8_t I2C_PIN_SDA = PB0;
constexpr uint8_t I2C_PIN_SCL = PB2;

//
//	LOCAL VARIABLES
//

//	stores the state of the machine
volatile _state_t _state;

//
//	PUBLIC FUNCTIONS - BODY
//

inline void i2c_back::init() {
	auto oldSREG = SREG;
	cli();
		//	sets pull ups
		I2C_REG_OUT |= (1 << I2C_PIN_SCL) | (1 << I2C_PIN_SDA);
		//	sets SCL as output
		I2C_REG_DIR |= (1 << I2C_PIN_SCL);

		//	sets control register
		USICR = 0x0 |
			//	enables the start interrupt
			(1 << USISIE) |
			//	doesnt enable the overflow interrupt
			(0 << USIOIE) |
			//	TwoWire mode without SCL hold on overflow
			(1 << USIWM1) | (0 << USIWM0) |
			//	external source, shift on positive edge
			(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |
			//	no manual clock
			(0 << USITC);
		// clear all interrupt flags and reset overflow counter
		USISR = 0x00 |
			//	clears the start and overflow interrupts
			(1 << USISIF) | (1 << USIOIF) | 
			//	clears the other two flags
			(1 << USIPF) | (0 << USIDC);
	SREG = oldSREG;
}

//	
//	LOCAL FUNCTIONS
//

static inline void _raw_set_idle() {
	USICR = 0x00 |
		//	keeps the start interrupt
		(1 << USISIE) |
		//	disables the overflow interrupt
		(0 << USIOIE) |
		//	disables SCL hold on overflow
		(1 << USIWM1) | (0 << USIWM0) |
		//	external clock, shift on positive edge
		(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |
		//	no manual clock
		(0 << USITC);
		// clear all interrupt flags and reset overflow counter
	USISR = 0x00 |
		//	clears the start and overflow interrupts
		(1 << USISIF) | (1 << USIOIF) | 
		//	clears the other two flags
		(1 << USIPF) | (0 << USIDC);
}

static inline void _raw_write_ack() {
	//	sets data, ACK = 0, NACK = 1
	USIDR = 0;
	//	sets SDA as output
	I2C_REG_DIR |= (1 << I2C_PIN_SDA);
	//	updates state register
	USISR = 0x00 |
		//	keeps start flag
		(0 << USISIF) | 
		//	releases SCL line from overflow
		(1 << USIOIF) |
		//	clears the other flags
		(1 << USIPF) | (0 << USIDC) |
		//	sets counter to 14, overflows after 1 bit (2 clocks)
		(1 << USICNT3) | (1 << USICNT2) | (1 << USICNT1) | (0 << USICNT0);
}
static inline void _raw_read_ack() {
	//	clears data
	USIDR = 0;
	//	sets SDA as input
	I2C_REG_DIR &= ~(1 << I2C_PIN_SDA);
	//	updates state register
	USISR = 0x00 |
		//	keeps start flag
		(0 << USISIF) | 
		//	releases SCL line from overflow
		(1 << USIOIF) |
		//	clears the other flags
		(1 << USIPF) | (0 << USIDC) |
		//	sets counter to 14, overflows after 1 bit (2 clocks)
		(1 << USICNT3) | (1 << USICNT2) | (1 << USICNT1) | (0 << USICNT0);
}

static inline void _raw_write_data(uint8_t b) {
	//	sets the byte to write
	USIDR = b;
	//	sets SDA as output
	I2C_REG_DIR |= (1 << I2C_PIN_SDA);
	//	updates state register
	USISR = 0x00 |
		//	keeps start flag
		(0 << USISIF) | 
		//	releases SCL line from overflow
		(1 << USIOIF) |
		//	clears the other flags
		(1 << USIPF) | (0 << USIDC) |
		//	sets counter to 0, overflows after 8 bit (16 clocks)
		(0 << USICNT3) | (0 << USICNT2) | (0 << USICNT1) | (0 << USICNT0);
}
static inline void _raw_read_data() {
	//	clears buffer
	USIDR = 0;
	//	sets SDA as input
	I2C_REG_DIR &= ~(1 << I2C_PIN_SDA);
	//	updates state register
	USISR = 0x00 |
		//	keeps start flag
		(0 << USISIF) | 
		//	releases SCL line from overflow
		(1 << USIOIF) |
		//	clears the other flags
		(1 << USIPF) | (0 << USIDC) |
		//	sets counter to 0, overflows after 8 bit (16 clocks)
		(0 << USICNT3) | (0 << USICNT2) | (0 << USICNT1) | (0 << USICNT0);
}

//
//	INTERRUPT SERVICE ROUTINES
//

ISR(VEC_I2C_START)
{
	//	TODO: Check SDA line
	//	Make sure that the SDA line doesnt go back up without the SCL going down
	//	TODO: SCL waiting
	//	Make sure the while is actually necessary and if it is, find a better way
	//	This can take a lot of cycles, PCINTs might be a better alternative

	//	waits for SCL line to go low
	while (I2C_REG_IN & (1 << I2C_PIN_SCL)) { }

	//	updates state
  	_state = ST_CHECK_ADDRESS;
	//	set SDA as input
	I2C_REG_DIR &= ~(1 << I2C_PIN_SDA);

	USICR = 0x00 |
		//	keeps start condition on
		(1 << USISIE) |
		//	enables overflow interrupt
		(1 << USIOIE) |
		//	TwoWire mode with overflow SCL hold
		(1 << USIWM1) | (1 << USIWM0) |
		//	external source, positive edge
		(1 << USICS1) | (0 << USICS0) | (0 << USICLK) |
		//	no manual clock
		(0 << USITC);
	USISR = 0x00 |
		//	releases the SCL line
		(1 << USISIF) | (1 << USIOIF) |
		//	clears the other flags
		(1 << USIPF) | (0 << USIDC) |
		//	sets counter to 0, overflows after 8 bit (16 clocks)
		(0 << USICNT3) | (0 << USICNT2) | (0 << USICNT1) | (0 << USICNT0);
}

ISR(VEC_I2C_OVERFLOW)
{
	//	TODO: Add dead time processing
	//	Between reads and writes, there's a lot of dead time
	//	An event can be added to allow for better processing

	//	the state is always set to the NEXT operation
	//	e.g.: writes ACK and sets to read data at the next overflow
	switch (_state)
  	{
	case ST_CHECK_ADDRESS: {
		//	checks the address and either acknowledges it or goes back to idle
		uint8_t b = USIDR;
		//	allows the user to decide what to do with the address
		auto res = i2c_back::ev_check_address(b >> 1, b & 0x01, b);
		if (res & i2c_back::ADDR_IS_MATCH) {
			//	valid address
			_raw_write_ack();
			//	checks if to read or write
			if (res & i2c_back::ADDR_MODE_WRITE) {
				_state = ST_WOP_WRITE_DATA;
			}
			else {
				_state = ST_ROP_READ_DATA;
			}
		}
		else {
			//	NO MATCH, sets to IDLE
			_raw_set_idle();
		}
	break;
	}

	//	WRITE OPERATION

	case ST_WOP_CHECK_ACK:
		//	checks if the master sent an ACK or NACK
		if (USIDR) {
			//	a NACK (1) means that the master doesnt want anymore data
			_raw_set_idle();
			break;
		}
		else {
			//	an ACK (0) means the master wants more data
			//	simply cascades to writing data case
		}

	case ST_WOP_WRITE_DATA: {
		//	sends the next byte to the master
		//	fetches the new byte
		uint8_t d = 0;
		i2c_back::ev_get_byte(d);
		//	writes the byte
		_raw_write_data(d);
		_state = ST_WOP_READ_ACK;
	break;
	}

	case ST_WOP_READ_ACK:
		//	prepares to receive the ACK
		_raw_read_ack();
		_state = ST_WOP_CHECK_ACK;
	break;

	//	READ OPERATION 

    case ST_ROP_READ_DATA:
		//	reads the data from the master
      	_raw_read_data();
		_state = ST_ROP_WRITE_ACK;
    break;

    case ST_ROP_WRITE_ACK: {
		//	sends an ACK after receiving the data
		//	buffers data
		uint8_t d = USIBR;
		//	sends ack
		_raw_write_ack();
		_state = ST_ROP_READ_DATA;
		//	handles the data during the ACK time
		i2c_back::ev_push_byte(d);
    break;
	}
	}
}

#endif	//	Include guard
