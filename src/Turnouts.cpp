#include "Turnouts.h"

UnCoupler::UnCoupler(int8_t Port) :
	accessories(3, 0, 201),
	_coil(Port, 500),
	_direction(TurnoutDirection::Closed) {

	Serial.println("UnCoupler::UnCoupler");
	off();
}

UnCoupler::UnCoupler(int8_t Port, int8_t Address) :
	accessories(Address, 0, 201),
	_coil(Port, 500),
	_direction(TurnoutDirection::Closed) {

	Serial.println("UnCoupler::UnCoupler");
	off();
}

UnCoupler::UnCoupler(int8_t Port, int8_t Address, uint16_t PulsTime) :
	accessories(Address, 0, 201),
	_coil(Port, PulsTime) {

	Serial.println("UnCoupler::UnCoupler");
	off();
}

UnCoupler::~UnCoupler() {
	_coil.~CoilPulsed();
}

AccessoryType UnCoupler::getType() const {
	Serial.print("UnCoupler::getType");
	return AccessoryType::Coil;
}

void UnCoupler::notifyTurnoutAddress(uint16_t Address, uint8_t Direction, uint8_t OutputPower) {
	
	if ((Address == BaseAddress) && static_cast<bool>(OutputPower)) {
		Serial.println("UnCoupler::notifyTurnoutAddress");
		Serial.print("    Address:     "); Serial.println(Address, DEC);
		Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);
		Serial.print("    Direction: "); Serial.println(Direction ? "Closed" : "Thrown");
		Serial.print("    Output: "); Serial.println(OutputPower ? "On" : "Off");

		if (static_cast<bool>(Direction)) {
			on();
		}
	}
}

void UnCoupler::process(){
	_coil.process();
	if ((_direction == TurnoutDirection::Thrown) && !_coil.isOn()) {
		off();
	}
}

void UnCoupler::on() {
	Thrown();
}

void UnCoupler::Thrown(){
	accessories::on();
	Serial.println("UnCoupler::Thrown");
	_coil.on();
	_direction = TurnoutDirection::Thrown;
}

void UnCoupler::off() {
	Closed();
}

void UnCoupler::Closed() {
	accessories::off();
	Serial.println("UnCoupler::Closed");
	_direction = TurnoutDirection::Closed;
}

Turnout::Turnout(int8_t ThrownPort, int8_t ClosedPort) : 
	accessories(3, 500, 202),
	_coil1(ThrownPort, 500),
	_coil2(ClosedPort, 500) {

	Serial.println("Turnout::Turnout");
}

Turnout::Turnout(int8_t ThrownPort, int8_t ClosedPort, uint16_t Address) :
	accessories(Address, 0, 202),
	_coil1(ThrownPort, 500),
	_coil2(ClosedPort, 500) {

	Serial.println("Turnout::Turnout");
}

Turnout::Turnout(int8_t ThrownPort, int8_t ClosedPort, uint16_t Address, uint16_t PulsTime_) :
	accessories(Address, 0, 202),
	_coil1(ThrownPort, PulsTime_),
	_coil2(ClosedPort, PulsTime_),
	_direction(TurnoutDirection::Closed) {

	Serial.println("Turnout::Turnout");
}

Turnout::~Turnout() {
	_coil1.~CoilPulsed();
	_coil2.~CoilPulsed();
}

AccessoryType Turnout::getType() const {
	Serial.print("UnCoupler::getType");
	return AccessoryType::Coil;
}

void Turnout::notifyTurnoutAddress(uint16_t Address, uint8_t Direction, uint8_t OutputPower) {

	if ((Address == BaseAddress) && static_cast<bool>(OutputPower)) {
		Serial.println("Turnout::notifyTurnoutAddress");
		Serial.print("    Address:     "); Serial.println(Address, DEC);
		Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);
		Serial.print("    Direction: "); Serial.println(Direction ? "Closed" : "Thrown");
		Serial.print("    Output: "); Serial.println(OutputPower ? "On" : "Off");

		if (static_cast<bool>(Direction)) {
			on();
		}
		else {
			off();
		}
	}
}

void Turnout::process() {
	_coil1.process();
	_coil2.process();
}

void Turnout::on() {
	Thrown();
}

void Turnout::Thrown(){
	accessories::on();
	Serial.println("Turnout::on");
	_coil2.off();
	_coil1.on();
	_direction = TurnoutDirection::Thrown;
}

void Turnout::off() {
	Closed();
}

void Turnout::Closed(){
	accessories::off();
	Serial.println("Turnout::Closed");
	_coil1.off();
	_coil2.on();
	_direction = TurnoutDirection::Closed;
}


TurnoutServo::TurnoutServo(int8_t ServoPort_, uint16_t Address, int16_t limit1_, int16_t limit2_, int16_t travelTime_) :
	accessories(Address, ServoPort_, 230),
	_servo(ServoPort_, limit1_, limit2_, travelTime_, SERVO_INITL1),
	_direction(TurnoutDirection::Closed) {

	Serial.println("TurnoutServo::TurnoutServo");
	_servo.setActive(true);
	off();
}

TurnoutServo::~TurnoutServo() {
	_servo.~ServoControl();	
}

void TurnoutServo::notifyTurnoutAddress(uint16_t Address, uint8_t Direction, uint8_t OutputPower) {

	if ((Address == BaseAddress) && static_cast<bool>(OutputPower)) {
		Serial.println("TurnoutServo::notifyTurnoutAddress");
		Serial.print("    Address:     "); Serial.println(Address, DEC);
		Serial.print("    BaseAddress: "); Serial.println(BaseAddress, DEC);
		Serial.print("    Direction: "); Serial.println(Direction ? "Closed" : "Thrown");
		Serial.print("    Output: "); Serial.println(OutputPower ? "On" : "Off");

		if (static_cast<bool>(Direction)) {
			on();
		}
		else {
			off();
		}
	}
}

void TurnoutServo::notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps) {
	if (Addr == BaseAddress) {
		//percentage = ((Speed - 1) * 100) / SpeedSteps;
	}
}

void TurnoutServo::process(){
	_servo.process();
}


void TurnoutServo::on(){
	accessories::on();
	Serial.println("TurnoutServo::on");

	MoveServo(100, true);
}

void TurnoutServo::off(){
	accessories::off();
	Serial.println("TurnoutServo::off");

	MoveServo(1, false);
}

void TurnoutServo::MoveServo(uint16_t percentage, bool clockwise) {
	if (_servo.isAbsolute()) {
		_servo.setPosition(percentage);
	}
	else {
		_servo.setPosition(percentage, clockwise);
	}
}
