/*
 Name:		zDecoder.ino
 Created:	14.10.2023 10:58:37
 Author:	andy
*/

// #define NOTIFY_DCC_ACCSTATE_MSG
// #define NOTIFY_DCC_MSG
// #define NOTIFY_TURNOUT_MSG
//#define NOTIFY_DCC_CV_WRITE_MSG
//#define NOTIFY_DCC_CV_CHANGE_MSG
//#define DEBUG_MSG


#include "webhandling.h"
#include "Vector.h"  // https://github.com/tomstewart89/Vector/fork
#include <NmraDcc.h>
#include "accessories.h"
#include "Blinkers.h"
#include "SBB_Signals.h"
#include "Turnouts.h"
#include "Watchdog.h"
#include "NMRAhandling.h"
#include "pinmapping.h"
#include "version.h"

char Version[] = VERSION_STR;

// #include "NMRAhandling.h"
#include "Vector.h"

// Liste mit allen Decoderobjekten (Apps)
Vector<accessories*> decoder;

// Task handle (Core 0 on ESP32)
TaskHandle_t TaskHandle;

bool ResetDCCDecoder = false;

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {
#ifdef  NOTIFY_TURNOUT_MSG
	Serial.print("notifyDccAccTurnoutOutput: Turnout: ");
	Serial.print(Addr, DEC);
	Serial.print(" Direction: ");
	Serial.print(Direction ? "Closed" : "Thrown");
	Serial.print(" Output: ");
	Serial.println(OutputPower ? "On" : "Off");
#endif

	for (int _i = 0; _i < decoder.Size(); _i++) {
		decoder[_i]->notifyTurnoutAddress(Addr, Direction, OutputPower);
	}
}

void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps) {
	for (int i = 0; i < decoder.Size(); i++) {

		// is it a Servo?
		if (decoder[i]->ChannelMode() == 230) {
			decoder[i]->notifyDccSpeed(Addr, Speed, ForwardDir, SpeedSteps);
		}
	}
}

void notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State) {
	uint8_t _OutputNum = OutputAddr >> 1;  //shift over the bits so the outputaddr is 0 to 3
	uint8_t _Cmd = OutputAddr & 0b00000001;  //JMRI puts out the state as the right most bit of pDccMsg->Data[1], the state argument doesnt change in JMRI Turnout.

#ifdef NOTIFY_DCC_ACCSTATE_MSG
	Serial.print("AccState - ");
	Serial.print("Raw addr: ");
	Serial.print(Addr);
	Serial.print(" BoardAddr: ");
	Serial.print(BoardAddr, DEC);
	Serial.print(" OutputAddr: ");
	Serial.print(OutputAddr, DEC);
	Serial.print(" Output: ");
	Serial.print(_OutputNum);
	Serial.print(" State: 0b");
	Serial.print(State, BIN);
	Serial.print(" Command: ");
	Serial.print(_Cmd == 1 ? "Closed" : "Thrown");
	Serial.println();
	//Serial.print("    zDecoder: "); Serial.println(BoardAddr + OutputAddr);
#endif 

#ifdef  NOTIFY_TURNOUT_MSG
	Serial.print("zDecoder: Address"); Serial.print(Addr);
	Serial.print(_Cmd == 1 ? " :Closed" : " :Thrown");
	Serial.println();
#endif

	for (int _i = 0; _i < decoder.Size(); _i++) {
		decoder[_i]->notifyAddress(Addr, _Cmd);
	}
}

void notifyDccFunc(uint16_t Addr, uint8_t FuncNum, uint8_t FuncState){

	if (FuncNum != 1)
		return;

	for (int i = 0; i < decoder.Size(); i++) {

		// is it a Servo?
		if (decoder[i]->ChannelMode() == 230) {
			if (FuncState & 0x10)
				decoder[i]->on();
			else
				decoder[i]->off();
		}
	}

	if (FuncNum != 1)
		return;
}

void handleChannel(uint8_t Channel) {
	if (ChannelIsActive(Channel)) {
		if (decoder[Channel]->isOn()) {
			decoder[Channel]->off();
		}
		else {
			decoder[Channel]->on();
		}
	}
}

bool ChannelIsOn(uint8_t Channel) {
	if (ChannelIsActive(Channel)) {
		return decoder[Channel]->isOn();
	}
	else {
		return false;
	}
}

bool ChannelIsActive(uint8_t Channel) {
	if (Channel < decoder.Size() && decoder[Channel] != nullptr) {
		return true;
	}
	else {
		return false;
	}
}

void zDecoderReset() {
	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		outputgroup_->DesignationParam.applyDefaultValue();
		outputgroup_->ModeParam.applyDefaultValue();
		outputgroup_->NumberParam.applyDefaultValue();
		outputgroup_->AddressParam.applyDefaultValue();
		outputgroup_->TimeOnParam.applyDefaultValue();
		outputgroup_->TimeOffParam.applyDefaultValue();
		outputgroup_->TimeOnFadeParam.applyDefaultValue();
		outputgroup_->TimeOffFadeParam.applyDefaultValue();
		outputgroup_->setActive(false);

		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}

	ServoGroup* servogroup_ = &ServoGroup1;
	while (servogroup_ != nullptr) {
		servogroup_->DesignationParam.applyDefaultValue();
		servogroup_->AddressParam.applyDefaultValue();
		servogroup_->TravelTimeParam.applyDefaultValue();
		servogroup_->Limit1Param.applyDefaultValue();
		servogroup_->Limit2Param.applyDefaultValue();
		servogroup_->setActive(false);

		servogroup_ = (ServoGroup*)servogroup_->getNext();
	}
}

void zDecoderInit(void) {

	uint8_t channel_ = 0;

	// Decoderobjekte in decoder löschen
	decoder.Erase(0, decoder.Size());
	decoder.Clear();
	Vector<accessories*>().Swap(decoder);

	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		if (channel_ > 15) {
			Serial.println("no more free channels!");
			break;
		}

		if (outputgroup_->isActive()) {
			uint8_t Mode_ = atoi(outputgroup_->ModeValue);
			uint8_t Count_ = atoi(outputgroup_->NumberValue);
			uint16_t Address_ = atoi(outputgroup_->AddressValue);
			uint8_t TimeOn_ = atoi(outputgroup_->TimeOnValue);
			uint8_t TimeOff_ = atoi(outputgroup_->TimeOffValue);
			uint8_t Multiplier_ = atoi(outputgroup_->MultiplierValue); // Multiplikator
			uint8_t TimeOnFade_ = atoi(outputgroup_->TimeOnFadeValue);
			uint8_t TimeOffFade_ = atoi(outputgroup_->TimeOffFadeValue);

			uint16_t DayLightAddress_ = 0;
			uint8_t DayBrightness_ = 255;
			uint8_t NightBrightness_ = 150;

			Serial.print(F("Values for channel ")); Serial.print(channel_); Serial.println(F(" preserved"));
			Serial.print(F("    Channels used: ")); Serial.println(Count_);
			Serial.print(F("    Address: ")); Serial.println(Address_);

			// Einrichten des Ports
			switch (Mode_) {
			case 0:
				decoder.PushBack(new accessories(Address_, channel_));
				channel_ += Count_;
				break;
			case 1:			// Watchdog
				decoder.PushBack(new Watchdog(Address_, channel_, Multiplier_ * TimeOn_));
				channel_ += 1;
				break;
			case 40:			// einfacher Ausgang
				decoder.PushBack(new Ausgang(Address_, channel_));
				channel_ += 1;
				break;

			case 50:			// Blinker
				decoder.PushBack(new Blinker(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_, TimeOnFade_, TimeOffFade_, Mode_));
				// decoder.PushBack(new Blinker(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_, 150, 150, Mode_));

				channel_ += 1;
				break;

			case 51:			// Wechselblinker
				decoder.PushBack(new Wechselblinker(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_, TimeOnFade_, TimeOffFade_));
				channel_ += 2;
				break;

			case 52: case 53: case 54: case 55:			// diverse Lauflichter
				decoder.PushBack(new Lauflicht(Address_, channel_, Count_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_, Mode_));
				channel_ += Count_;
				break;

			case 60:			// Hausbeleuchtung
				decoder.PushBack(new Hausbeleuchtung(Address_, channel_, Count_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_));
				channel_ += Count_;
				break;

			case 61:			// Neonröhren
				decoder.PushBack(new NeonLampen(Address_, channel_, Count_, Multiplier_));
				channel_ += Count_;
				break;

			case 62:			// Natriumlampen
				decoder.PushBack(new NatriumLampen(Address_, channel_, Count_, Multiplier_, TimeOnFade_, TimeOffFade_));
				channel_ += Count_;
				break;

			case 80:            // TV
				decoder.PushBack(new Fernseher(Address_, channel_));
				channel_ += 1;
				break;

			case 81:            // Schweisslicht
				decoder.PushBack(new Schweissen(Address_, channel_, Multiplier_ * TimeOff_, Multiplier_ * TimeOn_));
				channel_ += 3;
				break;

			case 82:            // Feuer
				decoder.PushBack(new Feuer(Address_, channel_));
				channel_ += 3;
				break;

			case 83:            // Blitzlicht
				decoder.PushBack(new Blitzlicht(Address_, channel_, TimeOnFade_ * 1000, TimeOffFade_ * 1000, TimeOn_ * Multiplier_, TimeOff_ * Multiplier_));
				channel_ += 1;
				break;

			case 102:			// SBB_Hauptsignal_102
				decoder.PushBack(new SBB_Hauptsignal_102(Address_, channel_, DayLightAddress_, DayBrightness_, NightBrightness_, TimeOnFade_, TimeOffFade_));
				channel_ += 2;
				break;


			case 103:			// SBB_Hauptsignal_103
				decoder.PushBack(new SBB_Hauptsignal_103(Address_, channel_, DayLightAddress_, DayBrightness_, NightBrightness_, TimeOnFade_, TimeOffFade_));
				channel_ += 3;
				break;

			case 104:			// SBB_Hauptsignal_104
				decoder.PushBack(new SBB_Hauptsignal_104(Address_, channel_, DayLightAddress_, DayBrightness_, NightBrightness_, TimeOnFade_, TimeOffFade_));
				channel_ += 4;
				break;

			case 105:
				decoder.PushBack(new SBB_Hauptsignal_105(Address_, channel_, DayLightAddress_, DayBrightness_, NightBrightness_, TimeOnFade_, TimeOffFade_));
				channel_ += 5;
				break;

			case 106:
				decoder.PushBack(new SBB_Hauptsignal_106(Address_, channel_, DayLightAddress_, DayBrightness_, NightBrightness_, TimeOnFade_, TimeOffFade_));
				channel_ += 6;
				break;

			case 110:
				decoder.PushBack(new SBB_Zwergsignal_110(Address_, channel_, DayLightAddress_, DayBrightness_, NightBrightness_, TimeOnFade_, TimeOffFade_));
				channel_ += 2;
				break;

			case 201: // Spule, Entkuppler
				decoder.PushBack(new UnCoupler(channel_, Address_, Multiplier_ * TimeOn_));
				channel_ += 1;
				break;

			case 202: // Weiche
				decoder.PushBack(new Turnout(channel_, channel_ + 1, Address_, Multiplier_ * TimeOn_));
				channel_ += 2;
				break;
			}
			
		}
		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}

	ServoGroup* servogroup_ = &ServoGroup1;
	while (servogroup_ != nullptr) {
		if (channel_ > 15) {
			Serial.println("no more free channels!");
			break;
		}

		if (servogroup_->isActive()) {

			uint8_t Address_ = atoi(servogroup_->AddressValue);
			uint8_t _TravelTime = atoi(servogroup_->TravelTimeValue);
			uint8_t Multiplier_ = atoi(servogroup_->MultiplierValue); // Multiplikator
			uint8_t _Limit1 = atoi(servogroup_->Limit1Value);
			uint8_t _Limit2 = atoi(servogroup_->Limit2Value);
			uint8_t Count_ = 1;

			Serial.print(F("Values for channel ")); Serial.print(channel_); Serial.println(F(" preserved"));
			Serial.print(F("    Channels used: ")); Serial.println(Count_);

			decoder.PushBack(new TurnoutServo(channel_, Address_, _Limit1, _Limit2, Multiplier_ * _TravelTime));
			channel_ += 1;

			Serial.print(F("next channel is ")); Serial.println(channel_);
		}
		servogroup_ = (ServoGroup*)servogroup_->getNext();

	}

}

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}

	Serial.println("Starting with Firmware " + String(VERSION));

	for (uint8_t _i = 0; _i < sizeof(ChannelToGPIOMapping) - 1; _i++) {
		pinMode(ChannelToGPIOMapping[_i], OUTPUT);
		digitalWrite(ChannelToGPIOMapping[_i], LOW);
	}

	NMRAsetup();
	Serial.println("NMRA initalized");

	websetup();
	Serial.println("web server initalized");

	if (CONFIG_PIN >= 0) {
		pinMode(CONFIG_PIN, INPUT_PULLUP);
		if (digitalRead(CONFIG_PIN) == LOW) {
			Serial.println("config pin was pressed, reset decoder");
			zDecoderReset();
		}
	}

    zDecoderInit();
	Serial.println("Decoder initalized");

	xTaskCreatePinnedToCore(
		loop2, /* Function to implement the task */
		"TaskHandle", /* Name of the task */
		10000,  /* Stack size in words */
		NULL,  /* Task input parameter */
		0,  /* Priority of the task */
		&TaskHandle,  /* Task handle. */
		0 /* Core where the task should run */
	);

	Serial.println("Everything has been initialized");
}

void loop() {
	NMRAloop();
	webloop();
}

void loop2(void* parameter) {

	for (;;) {   // Endless loop
		if (ResetDCCDecoder) {
			zDecoderInit();
			ResetDCCDecoder = false;
		}

		for (int _i = 0; _i < decoder.Size(); _i++) {
			decoder[_i]->process();
		}
	}
}
