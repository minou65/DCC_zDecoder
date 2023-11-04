#include "Turnouts.h"

UnCoupler::UnCoupler(int8_t Port_) :
	accessories(3, 0, 201),
	coil(Port_, 500) {

	Serial.println("UnCoupler::UnCoupler");
	off();
}

UnCoupler::UnCoupler(int8_t Port_, int8_t Address_) :
	accessories(Address_, 0, 201),
	coil(Port_, 500) {

	Serial.println("UnCoupler::UnCoupler");
	off();
}

UnCoupler::UnCoupler(int8_t Port_, int8_t Address_, uint16_t PulsTime_) :
	accessories(Address_, 0, 201),
	coil(Port_, PulsTime_) {

	Serial.println("UnCoupler::UnCoupler");
	off();
}

UnCoupler::~UnCoupler() {
	coil.~CoilPulsed();
}

void UnCoupler::notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_) {
	if ((Address_ >= BaseAddress) && (Address_ < (BaseAddress + 1)) && OutputPower_) {
		if (Direction_) {
			on();
		}
	}
}

void UnCoupler::process(){
	coil.process();
}

void UnCoupler::on(){
	accessories::on();
	Serial.println("UnCoupler::on");
	coil.on();
	off();

}

void UnCoupler::off() {
	accessories::off();
	Serial.println("UnCoupler::off");

}

Turnout::Turnout(int8_t RPort_, int8_t GPort_) : 
	accessories(3, 500, 202),
	coil1(RPort_, 500),
	coil2(GPort_, 500) {

	Serial.println("Turnout::Turnout");
}

Turnout::Turnout(int8_t RPort_, int8_t GPort_, int8_t Address_) :
	accessories(Address_, 0, 202),
	coil1(RPort_, 500),
	coil2(GPort_, 500) {

	Serial.println("Turnout::Turnout");
}

Turnout::Turnout(int8_t RPort_, int8_t GPort_, int8_t Address_, uint16_t PulsTime_) :
	accessories(Address_, 0, 202),
	coil1(RPort_, PulsTime_),
	coil2(GPort_, PulsTime_) {

	Serial.println("Turnout::Turnout");
}

Turnout::~Turnout() {
	coil1.~CoilPulsed();
	coil2.~CoilPulsed();
}

void Turnout::notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_){
	if ((Address_ >= BaseAddress) && (Address_ < (BaseAddress + 1)) && OutputPower_) {
		if (Direction_) {
			on();
		}
		else {
			off();
		}
	}
}

void Turnout::process() {

	coil1.process();
	coil2.process();

}

// Turnout
void Turnout::on(){
	accessories::on();
	Serial.println("Turnout::on");
	coil2.off();
	coil1.on();
}

// Closed
void Turnout::off(){
	accessories::off();
	Serial.println("Turnout::off");
	coil1.off();
	coil2.on();
}

TurnoutServo::TurnoutServo(int8_t ServoPort_, int8_t Address_, int8_t limit1_, int8_t limit2_, int8_t travelTime_) :
	accessories(Address_, ServoPort_, 230),
	// servo(ServoPort_, limit1_, limit2_, travelTime_, SERVO_INITMID)
	servo(ServoPort_, 1, 180, 5, SERVO_INITMID)
{

	Serial.println("TurnoutServo::TurnoutServo");
	Serial.print("    Port: "); Serial.println(ServoPort_);

	servo.setActive(true);

}

TurnoutServo::~TurnoutServo() {
	servo.~ServoControl();	
}

void TurnoutServo::process(){
	servo.loop();
}

void TurnoutServo::notifyTurnoutAddress(uint16_t Address_, uint8_t Direction_, uint8_t OutputPower_){
	if ((Address_ >= BaseAddress) && (Address_ < (BaseAddress + 1)) && OutputPower_) {
		if (Direction_) {
			on();
		}
		else {
			off();
		}
	}
}

void TurnoutServo::notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps){
	if (Addr == BaseAddress) {
		percentage = ((Speed - 1) * 100) / SpeedSteps;
	}
}

void TurnoutServo::on(){
	accessories::on();
	Serial.println("TurnoutServo::on");

	percentage = 10;

	if (servo.isAbsolute()) {
		servo.setPosition(percentage);
	}
	else {
		servo.setSpeed(percentage, true);
	}
}

void TurnoutServo::off(){
	accessories::off();
	Serial.println("TurnoutServo::off");

	percentage = 90;

	if (servo.isAbsolute()) {
		servo.setPosition(percentage);
	}
	else {
		servo.setSpeed(percentage, false);
	}
}
