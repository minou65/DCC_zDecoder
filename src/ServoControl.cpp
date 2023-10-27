#include "ServoControl.h"
#include <Servo.h>

ServoControl::ServoControl(int8_t GPIO_, int8_t limit1_, int8_t limit2_, int8_t travelTime_, int8_t flags_ = SERVO_INITMID) :
    GPIO(GPIO_),
    limit1(limit1_),
    limit2(limit2_),
    tlimit1(limit1_ * 10),
    tlimit2(limit2_ * 10),
    angle(tlimit1),
    bouncing(false),
    bounced(false),
    bouncingSteps(0),
    bouncingAngle(0),
    clockwise(true),
    moving(false),
    IsActive(false) {

    interval = travelTime / (tlimit2 - tlimit1);
    TravelTimer.set(interval);
    TravelTimer.start();

    servo.attach(GPIO);

    switch (flags & (SERVO_INITL1 | SERVO_INITL2 | SERVO_INITMID)){
    case SERVO_INITL1:
        servo.write(limit1);
        break;
    case SERVO_INITL2:
        servo.write(limit2);
        break;
    case SERVO_INITMID:
        servo.write(limit1 + (limit2 - limit1) / 2);
        break;
    }
}

void ServoControl::loop() {
    int8_t  _newangle;
    bool _clockwise;

    if (flags & SERVO_ABSOLUTE)
        return;

    if (!TravelTimer.repeat())
        return;

    if (IsActive == false || moving == false) {
        TravelTimer.set(interval);
        return;
    }

    _clockwise = clockwise;
    if (this->flags & SERVO_REVERSE)
        _clockwise = !_clockwise;

    _newangle = angle;
    if (bouncing) {
        if (bclockwise && angle < blimit) {
            _newangle = angle + 1;
        }
        else if (bclockwise == false && angle > bouncepoint) {
            _newangle = angle - 1;
        }
        else if (angle == bouncepoint) {
            bouncingSteps++;
            int bouncingangleel;
            bouncingangleel = 10 * (bouncingAngle >> (bouncingSteps / 2));
            if (bouncingangleel == 0) {
                bouncing = false;
                bounced = true;
                _newangle = tlimit2;
            }
            else {
                if (blimit == tlimit2)
                    bouncepoint = tlimit2 - (10 * bouncingangleel);
                else
                    blimit = tlimit1 + (10 * bouncingangleel);
                bclockwise = !bclockwise;
            }
        }
        else {
            bouncingSteps++;
            int bouncingangleel;
            bouncingangleel = 10 * (bouncingAngle >> (bouncingSteps / 2));
            if (bouncingangleel == 0) {
                bouncing = false;
                bounced = true;
                _newangle = tlimit1;
            }
            else {
                if (blimit == tlimit2)
                    bouncepoint = tlimit2 - (10 * bouncingangleel);
                else
                    blimit = tlimit1 + (10 * bouncingangleel);
                bclockwise = !bclockwise;
            }
        }
    }
    else {
        if (clockwise && angle < tlimit2) {
            _newangle = angle + 1;
            bounced = false;
        }
        else if (clockwise == false && angle > tlimit1) {
            _newangle = angle - 1;
            bounced = false;
        }
        else if ((flags & SERVO_BOUNCE_L1) != 0 && clockwise == false
            && bounced == false && bouncingAngle != 0) {
            bouncingSteps = 1;
            bouncing = true;
            bouncepoint = angle;
            blimit = tlimit1 + (10 * bouncingAngle);
            bclockwise = true;
        }
        else if ((flags & SERVO_BOUNCE_L2) != 0 && clockwise
            && bounced == false && bouncingAngle != 0) {
            bouncingSteps = 1;
            bouncing = true;
            blimit = angle;
            bouncepoint = tlimit2 - (10 * bouncingAngle);
            bclockwise = false;
        }
        else if (flags & SERVO_AUTO_REVERSE)
            flags = flags ^ SERVO_REVERSE;
        else
            moving = false;
    }

    if (_newangle != angle) {
        writeTenths(_newangle);
        angle = _newangle;
    }
    if (angle / 10 != reported) {
        reported = angle / 10;
        if (notifyServoPosition)
            notifyServoPosition(this, reported);
    }

    TravelTimer.set((interval * 100) / percentage);
 
}

void ServoControl::setSpeed(int8_t percentage_, bool clockwise_){
    percentage = percentage_;
    clockwise = clockwise_;
    moving = (percentage > 0);
}

void ServoControl::setActive(bool active_){
    if ((active_ == true) && (IsActive == false)){
        servo.attach(GPIO);
        TravelTimer.set((interval * 100) / percentage);
        TravelTimer.start();
    }
    else if ((active_ == false) && (IsActive == true))
    {
        servo.detach();
    }
    IsActive = active_;
}

void ServoControl::setStart(int8_t start_){
    if (start_ < 0)
        start_ = 0;
    else if (start_ > 180)
        start_ = 180;
    limit1 = start_;
    tlimit1 = start_ * 10;
    interval = travelTime / (tlimit2 - tlimit1);
}

void ServoControl::setEnd(int8_t angle_){
    if (angle_ < 0)
        angle_ = 0;
    else if (angle_ > 180)
        angle_ = 180;
    limit2 = angle_;
    tlimit2 = angle_ * 10;
    interval = travelTime / (tlimit2 - tlimit1);
}

void ServoControl::setFlags(int8_t flags_){
    flags = (int8_t)flags_;
}

void ServoControl::setBounceAngle(int8_t angle_){
    bouncingAngle = angle_;
}

void ServoControl::setPosition(int8_t percentage_) {
    uint16_t _range = tlimit2 - tlimit1;
    uint16_t _tenth = (percentage_ * _range) / 100;

    _tenth += tlimit1;
    if (IsActive){
        angle = _tenth;
        writeTenths(_tenth);
    }
}

void ServoControl::setAngle(int8_t angle_) {
    uint16_t _tenth = angle_ * 10;

    angle = _tenth;
    if (IsActive) {
        writeTenths(_tenth);
    }
}

int8_t ServoControl::getAngle() {
    return angle / 10;
}

void ServoControl::setTravelTime(int8_t time_) {
    travelTime = time_ * 1000;
    interval = travelTime / (tlimit2 - tlimit1);
}

boolean ServoControl::isAbsolute() {
    if (flags & SERVO_ABSOLUTE)
        return true;
    return false;
}

void ServoControl::writeTenths(int8_t tenth_) {
    servo.writeMicroseconds(map(tenth_, 0, 1800, SERVO_MIN(), SERVO_MAX()));
}

