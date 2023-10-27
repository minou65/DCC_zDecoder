// Servos.h

#ifndef _SERVOS_h
#define _SERVOS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "accessories.h"
#include "neotimer.h"
#include <Servo.h>

#define SERVO_INITL1		0x01
#define SERVO_INITL2		0x02
#define SERVO_INITMID		0x03

#define SERVO_ABSOLUTE		0x04
#define SERVO_BOUNCE_L1		0x08
#define SERVO_BOUNCE_L2		0x10
#define SERVO_AUTO_REVERSE	0x20
#define SERVO_REVERSE		0x80

#define SERVO_MIN() (544)  // minimum value in uS for this servo
#define SERVO_MAX() (2400)  // maximum value in uS for this servo 

class ServoControl {
private:
    Servo servo;
    Neotimer TravelTimer;

    int8_t GPIO;

    bool IsActive;

    int8_t limit1;     // Limit of travel in one direction
    int8_t limit2;     // Limit of travel in the other direction
    int8_t tlimit1;    // Limit of travel in one directionin tenths
    int8_t tlimit2;    // Limit of travel in the other direction in tenths
    uint16_t travelTime; // Total travel time between two limits in milliseconds
    uint16_t interval;   // Time between steps of 1 degree in milliseconds

    bool clockwise;  // Current direction is clockwise
    int8_t percentage; // Current speed percentage
    bool moving;     // Servo is moving

    int8_t angle;

    int8_t flags;	 // Configuration flags

    int8_t blimit;
    bool bclockwise;
    bool bounced;
    bool bouncing;
    int8_t bouncingSteps;
    int8_t bouncingAngle;
    int8_t bouncepoint;

    int8_t reported;

public:
    ServoControl() = default;
    ServoControl(int8_t GPIO_, int8_t limit1_, int8_t limit2_, int8_t travelTime_, int8_t flags_);
    void loop();
    void setSpeed(int8_t percentage_, bool clockwise_);
    void setActive(bool active_);
    void setStart(int8_t start_);
    void setEnd(int8_t angle_);
    void setFlags(int8_t flags_);
    void setBounceAngle(int8_t angle_);
    void setPosition(int8_t percentage_);
    void setAngle(int8_t angle_);
    int8_t getAngle();
    void setTravelTime(int8_t time_);
    boolean isAbsolute();
    void writeTenths(int8_t tenth_);

};

extern void notifyServoPosition(ServoControl*, int) __attribute__((weak));

#endif

