// 
// 
// 

#include "CoilControl.h"
#include "pinmapping.h"

Coil::Coil() :
	GPIO(ChannelToGPIOMapping[1]) {

	pinMode(GPIO, OUTPUT);
	digitalWrite(GPIO, LOW);
}
    
Coil::Coil(const int8_t GPIO_) :
	GPIO(ChannelToGPIOMapping[GPIO_]) {

	Serial.println("Coil::Coil");
	pinMode(GPIO, OUTPUT);
	digitalWrite(GPIO, LOW);
}

Coil::~Coil(){
}

void Coil::process(){
}

void Coil::on(){
	IsActive = true;
	digitalWrite(GPIO, HIGH);

}

void Coil::off(){
	IsActive = false;
	digitalWrite(GPIO, LOW);
}

bool Coil::isOn(){
	return IsActive;

}


CoilPulsed::CoilPulsed(const int8_t GPIO_) :
	Coil(GPIO_) {
	Serial.println("CoilPulsed::CoilPulsed");
	Puls.set(500);
}

CoilPulsed::CoilPulsed(const int8_t GPIO_, const uint16_t PulsLength_) :
	Coil(GPIO_) {
	Serial.println("CoilPulsed::CoilPulsed");
	Serial.print("    PulsLength: "); Serial.println(PulsLength_);
	Puls.set(PulsLength_);
}

CoilPulsed::~CoilPulsed(){
	Puls.~Neotimer();
}

void CoilPulsed::process(){

	Coil::process();

	if (Puls.done()) {
		Coil::off();
		Puls.stop();
	}
}

void CoilPulsed::on(){
	Serial.println("CoilPulsed::on");
	Coil::on();
	Puls.start();
}

void CoilPulsed::off() {
	Serial.println("CoilPulsed::off");
	Coil::off();
	Puls.stop();
}

