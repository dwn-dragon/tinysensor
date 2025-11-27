#pragma once

#ifndef TYS_ADC_INCLUDE
#define TYS_ADC_INCLUDE

#include "tys/core.h"

#include <avr/io.h>

namespace adc
{

	enum class prescaler : uint8_t
	{
		F002 = 0x00 | (0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0),
		F004 = 0x00 | (0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0),
		F008 = 0x00 | (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0),
		F016 = 0x00 | (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0),
		F032 = 0x00 | (1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0),
		F064 = 0x00 | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0),
		F128 = 0x00 | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	};

	enum class vref : uint8_t
	{
		VCC		= 0x00 | (0 << REFS2) | (0 << REFS1) | (0 << REFS0),
		AREF	= 0x00 | (0 << REFS2) | (0 << REFS1) | (1 << REFS0),
		V110	= 0x00 | (0 << REFS2) | (1 << REFS1) | (0 << REFS0),
		V110CAP	= 0x00 | (0 << REFS2) | (1 << REFS1) | (1 << REFS0),
		V256	= 0x00 | (1 << REFS2) | (1 << REFS1) | (0 << REFS0),
		V256CAP	= 0x00 | (1 << REFS2) | (1 << REFS1) | (1 << REFS0),
	};

	enum class input : uint8_t
	{
		SINGLE_P5	= 0x00 | (0 << MUX3) |(0 << MUX2) | (0 << MUX1) | (0 << MUX0),
		SINGLE_P2	= 0x00 | (0 << MUX3) |(0 << MUX2) | (0 << MUX1) | (1 << MUX0),
		SINGLE_P4	= 0x00 | (0 << MUX3) |(0 << MUX2) | (1 << MUX1) | (0 << MUX0),
		SINGLE_P3	= 0x00 | (0 << MUX3) |(0 << MUX2) | (1 << MUX1) | (1 << MUX0),

		DIFF_P33_01	= 0x00 | (0 << MUX3) |(1 << MUX2) | (0 << MUX1) | (0 << MUX0),
		DIFF_P33_20	= 0x00 | (0 << MUX3) |(1 << MUX2) | (0 << MUX1) | (1 << MUX0),
		DIFF_P34_01	= 0x00 | (0 << MUX3) |(1 << MUX2) | (1 << MUX1) | (0 << MUX0),
		DIFF_P34_20	= 0x00 | (0 << MUX3) |(1 << MUX2) | (1 << MUX1) | (1 << MUX0),
		
		DIFF_P55_01	= 0x00 | (1 << MUX3) |(0 << MUX2) | (0 << MUX1) | (0 << MUX0),
		DIFF_P55_20	= 0x00 | (1 << MUX3) |(0 << MUX2) | (0 << MUX1) | (1 << MUX0),
		DIFF_P52_01	= 0x00 | (1 << MUX3) |(0 << MUX2) | (1 << MUX1) | (0 << MUX0),
		DIFF_P52_20	= 0x00 | (1 << MUX3) |(0 << MUX2) | (1 << MUX1) | (1 << MUX0),
	};

	static inline void enable();
	static inline void disable();

	static inline void interrupt_enable();
	static inline void interrupt_disable();

	static inline void set_prescaler(prescaler Value);

	static inline void set_vref(vref Value);

	static inline void set_input(input Value);

}

inline void adc::enable() {
	ADCSRA |=  (1 << ADEN);
}
inline void adc::disable() {
	ADCSRA &= ~(1 << ADEN);
}

inline void adc::interrupt_enable() {
	ADCSRA |=  (1 << ADIE);
}
inline void adc::interrupt_disable() {
	ADCSRA &= ~(1 << ADIE);
}

inline void adc::set_prescaler(prescaler Value) {
	uint8_t _tmp = ADCSRA;
	_tmp &= ~((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0));
	_tmp |= (uint8_t)Value;
	ADCSRA = _tmp;
}

inline void adc::set_vref(vref Value) {
	uint8_t _tmp = ADMUX;
	_tmp &= ~((1 << REFS2) | (1 << REFS1) | (1 << REFS0));
	_tmp |= (uint8_t)Value;
	ADMUX = _tmp;
}

inline void adc::set_input(input Value) {
	uint8_t _tmp = ADMUX;
	_tmp &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
	_tmp |= (uint8_t)Value;
	ADMUX = _tmp;
}

#endif	//	Include guard
