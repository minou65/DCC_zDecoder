// pinmapping.h

#ifndef _PINMAPPING_h
#define _PINMAPPING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

static uint8_t ChannelToGPIOMapping[16] = {
	GPIO_NUM_16,
	GPIO_NUM_17,
	GPIO_NUM_18,
	GPIO_NUM_19,
	GPIO_NUM_21,
	GPIO_NUM_22,
	GPIO_NUM_23,
	GPIO_NUM_26,

	GPIO_NUM_12,
	GPIO_NUM_13,
	GPIO_NUM_14,
	GPIO_NUM_4,
	GPIO_NUM_25,
	GPIO_NUM_27,
	GPIO_NUM_32,
	GPIO_NUM_33

};

#endif

