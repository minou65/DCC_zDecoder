// pinmapping.h

#ifndef _PINMAPPING_h
#define _PINMAPPING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// Recommend only the following pins 2,4,12-19,21-23,25-27,32-33
// Ensure no pin assignments in your Sketch or Libraries are using 
// the IO19 or IO20 to ensure the USB Debugging works correctly.

// All boards support the vMicro Software Debugger which only requires 
// the Serial interface to work

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

