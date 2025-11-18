#pragma once

#ifndef TYS_GPIO_INCLUDE
#define TYS_GPIO_INCLUDE

#if __has_include(<env.h>)
#include <env.h>
#elif __has_include("env.h")
#include "env.h"
#endif

#include <stdint.h>
#include <avr/io.h>

namespace gpio
{

	using num_t = uint8_t;
	using state_t = bool;

	constexpr state_t high = true, low = false;
	constexpr state_t output = true, input = false;
	constexpr state_t pu_on = true, pu_off = false;

	template< num_t Pin >
	static inline void set_dir(state_t Direction);

	template< num_t Pin >
	static inline void set_input();

	template< num_t Pin >
	static inline void set_output();
	
	template< num_t Pin >
	static inline void pull_up(state_t Enabled);

	template< num_t Pin >
	static inline void pull_up_on();

	template< num_t Pin >
	static inline void pull_up_off();

	template< num_t Pin >
	static inline void write(state_t High);

	template< num_t Pin >
	static inline void write_high();

	template< num_t Pin >
	static inline void write_low();

	template< num_t Pin >
	static inline state_t read();

	template< num_t Pin >
	static inline void toggle();

	//	
	//	IMPLEMENTATION
	//
	
	namespace _impl
	{

		template< num_t Pv >
		static inline uint8_t _bit() {
			return Pv;
		}
		template< num_t Pv >
		static inline uint8_t _mask() {
			constexpr uint8_t _mk = (1 << Pv);
			return _mk;
		}

		template< num_t Pv >
		static inline volatile uint8_t& _reg_dir() {
			return DDRB;
		}
		template< num_t Pv >
		static inline volatile uint8_t& _reg_out() {
			return PORTB;
		}
		template< num_t Pv >
		static inline volatile uint8_t& _reg_in() {
			return PINB;
		}

	}

	template< num_t Pin >
	inline void set_dir(state_t Direction) {
		if (Direction)
			set_output<Pin>();
		else
			set_input<Pin>();
	}
	template< num_t Pin >
	inline void set_output() {
		_impl::_reg_dir<Pin>() |=  _impl::_mask<Pin>();
	}
	template< num_t Pin >
	inline void set_input() {
		_impl::_reg_dir<Pin>() &= ~_impl::_mask<Pin>();
	}

	template< num_t Pin >
	inline void write(state_t b) {
		if (b)
			write_high<Pin>();
		else
			write_low<Pin>();
	}
	template< num_t Pin >
	inline void write_high() {
		_impl::_reg_out<Pin>() |=  _impl::_mask<Pin>();
	}
	template< num_t Pin >
	inline void write_low() {
		_impl::_reg_out<Pin>() &= ~_impl::_mask<Pin>();
	}

	template< num_t Pin >
	inline void pull_up(state_t b) {
		write<Pin>(b);
	}
	template< num_t Pin >
	inline void pull_up_on() {
		write_high<Pin>();
	}
	template< num_t Pin >
	inline void pull_up_off() {
		write_low<Pin>();
	}

	template< num_t Pin >
	inline state_t read() {
		return (_impl::_reg_in<Pin>() & _impl::_mask<Pin>());
	}
	template< num_t Pin >
	inline void toggle() {
		_impl::_reg_in<Pin>() |= _impl::_mask<Pin>();
	}

}


#endif	//	Include guard
