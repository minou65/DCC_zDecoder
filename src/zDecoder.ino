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
	OutputGroup* _outputgroup = &OutputGroup1;
	while (_outputgroup != nullptr) {
		_outputgroup->DesignationParam.applyDefaultValue();
		_outputgroup->ModeParam.applyDefaultValue();
		_outputgroup->NumberParam.applyDefaultValue();
		_outputgroup->AddressParam.applyDefaultValue();
		_outputgroup->TimeOnParam.applyDefaultValue();
		_outputgroup->TimeOffParam.applyDefaultValue();
		_outputgroup->TimeOnFadeParam.applyDefaultValue();
		_outputgroup->TimeOffFadeParam.applyDefaultValue();
		_outputgroup->setActive(false);

		_outputgroup = (OutputGroup*)_outputgroup->getNext();
	}

	ServoGroup* _servogroup = &ServoGroup1;
	while (_servogroup != nullptr) {
		_servogroup->DesignationParam.applyDefaultValue();
		_servogroup->AddressParam.applyDefaultValue();
		_servogroup->TravelTimeParam.applyDefaultValue();
		_servogroup->Limit1Param.applyDefaultValue();
		_servogroup->Limit2Param.applyDefaultValue();
		_servogroup->setActive(false);

		_servogroup = (ServoGroup*)_servogroup->getNext();
	}
}

void zDecoderInit(void) {

	uint8_t _Channel = 0;

	// Decoderobjekte in decoder l�schen
	decoder.Erase(0, decoder.Size());
	decoder.Clear();
	Vector<accessories*>().Swap(decoder);

	OutputGroup* _outputgroup = &OutputGroup1;
	while (_outputgroup != nullptr) {
		if (_Channel > 15) {
			Serial.println("no more free channels!");
			break;
		}

		if (_outputgroup->isActive()) {
			uint8_t _Mode = atoi(_outputgroup->ModeValue);
			uint8_t _Count = atoi(_outputgroup->NumberValue);
			uint16_t _Address = atoi(_outputgroup->AddressValue);
			uint8_t _TimeOn = atoi(_outputgroup->TimeOnValue);
			uint8_t _TimeOff = atoi(_outputgroup->TimeOffValue);
			uint8_t _Multiplier = atoi(_outputgroup->MultiplierValue); // Multiplikator
			uint8_t _TimeOnFade = atoi(_outputgroup->TimeOnFadeValue);
			uint8_t _TimeOffFade = atoi(_outputgroup->TimeOffFadeValue);

			uint16_t _DayLightAddress = 0;
			uint8_t _DayBrightness = 255;
			uint8_t _NightBrightness = 150;

			Serial.print(F("Values for channel ")); Serial.print(_Channel); Serial.println(F(" preserved"));
			Serial.print(F("    Channels used: ")); Serial.println(_Count);
			Serial.print(F("    Address: ")); Serial.println(_Address);

			// Einrichten des Ports
			switch (_Mode) {
			case 0:
				decoder.PushBack(new accessories(_Address, _Channel));
				_Channel += _Count;
				break;
			case 1:			// Watchdog
				decoder.PushBack(new Watchdog(_Address, _Channel, _Multiplier * _TimeOn));
				_Channel += 1;
				break;
			case 40:			// einfacher Ausgang
				decoder.PushBack(new Ausgang(_Address, _Channel));
				_Channel += 1;
				break;

			case 50:			// Blinker
				decoder.PushBack(new Blinker(_Address, _Channel, _Multiplier * _TimeOff, _Multiplier * _TimeOn, _TimeOnFade, _TimeOffFade, _Mode));
				// decoder.PushBack(new Blinker(_Address, _Channel, _Multiplier * _TimeOff, _Multiplier * _TimeOn, 150, 150, _Mode));

				_Channel += 1;
				break;

			case 51:			// Wechselblinker
				decoder.PushBack(new Wechselblinker(_Address, _Channel, _Multiplier * _TimeOff, _Multiplier * _TimeOn, _TimeOnFade, _TimeOffFade));
				_Channel += 2;
				break;

			case 52: case 53: case 54: case 55:			// diverse Lauflichter
				decoder.PushBack(new Lauflicht(_Address, _Channel, _Count, _Multiplier * _TimeOff, _Multiplier * _TimeOn, _Mode));
				_Channel += _Count;
				break;

			case 60:			// Hausbeleuchtung
				decoder.PushBack(new Hausbeleuchtung(_Address, _Channel, _Count, _Multiplier * _TimeOff, _Multiplier * _TimeOn));
				_Channel += _Count;
				break;

			case 61:			// Neonr�hren
				decoder.PushBack(new NeonLampen(_Address, _Channel, _Count, _Multiplier));
				_Channel += _Count;
				break;

			case 62:			// Natriumlampen
				decoder.PushBack(new NatriumLampen(_Address, _Channel, _Count, _Multiplier, _TimeOnFade, _TimeOffFade));
				_Channel += _Count;
				break;

			case 80:            // TV
				decoder.PushBack(new Fernseher(_Address, _Channel));
				_Channel += 1;
				break;

			case 81:            // Schweisslicht
				decoder.PushBack(new Schweissen(_Address, _Channel, _Multiplier * _TimeOff, _Multiplier * _TimeOn));
				_Channel += 3;
				break;

			case 82:            // Feuer
				decoder.PushBack(new Feuer(_Address, _Channel));
				_Channel += 3;
				break;

			case 83:            // Blitzlicht
				decoder.PushBack(new Blitzlicht(_Address, _Channel, _TimeOnFade * 1000, _TimeOffFade * 1000, _TimeOn * _Multiplier, _TimeOff * _Multiplier));
				_Channel += 1;
				break;

			case 102:			// SBB_Hauptsignal_102
				decoder.PushBack(new SBB_Hauptsignal_102(_Address, _Channel, _DayLightAddress, _DayBrightness, _NightBrightness, _TimeOnFade, _TimeOffFade));
				_Channel += 2;
				break;


			case 103:			// SBB_Hauptsignal_103
				decoder.PushBack(new SBB_Hauptsignal_103(_Address, _Channel, _DayLightAddress, _DayBrightness, _NightBrightness, _TimeOnFade, _TimeOffFade));
				_Channel += 3;
				break;

			case 104:			// SBB_Hauptsignal_104
				decoder.PushBack(new SBB_Hauptsignal_104(_Address, _Channel, _DayLightAddress, _DayBrightness, _NightBrightness, _TimeOnFade, _TimeOffFade));
				_Channel += 4;
				break;

			case 105:
				decoder.PushBack(new SBB_Hauptsignal_105(_Address, _Channel, _DayLightAddress, _DayBrightness, _NightBrightness, _TimeOnFade, _TimeOffFade));
				_Channel += 5;
				break;

			case 106:
				decoder.PushBack(new SBB_Hauptsignal_106(_Address, _Channel, _DayLightAddress, _DayBrightness, _NightBrightness, _TimeOnFade, _TimeOffFade));
				_Channel += 6;
				break;

			case 110:
				decoder.PushBack(new SBB_Zwergsignal_110(_Address, _Channel, _DayLightAddress, _DayBrightness, _NightBrightness, _TimeOnFade, _TimeOffFade));
				_Channel += 2;
				break;

			case 201: // Spule, Entkuppler
				decoder.PushBack(new UnCoupler(_Channel, _Address, _Multiplier * _TimeOn));
				_Channel += 1;
				break;

			case 202: // Weiche
				decoder.PushBack(new Turnout(_Channel, _Channel + 1, _Address, _Multiplier * _TimeOn));
				_Channel += 2;
				break;
			}
			
		}
		_outputgroup = (OutputGroup*)_outputgroup->getNext();
	}

	ServoGroup* _servogroup = &ServoGroup1;
	while (_servogroup != nullptr) {
		if (_Channel > 15) {
			Serial.println("no more free channels!");
			break;
		}

		if (_servogroup->isActive()) {

			uint8_t _Address = atoi(_servogroup->AddressValue);
			uint8_t _TravelTime = atoi(_servogroup->TravelTimeValue);
			uint8_t _Multiplier = atoi(_servogroup->MultiplierValue); // Multiplikator
			uint8_t _Limit1 = atoi(_servogroup->Limit1Value);
			uint8_t _Limit2 = atoi(_servogroup->Limit2Value);
			uint8_t _Count = 1;

			Serial.print(F("Values for channel ")); Serial.print(_Channel); Serial.println(F(" preserved"));
			Serial.print(F("    Channels used: ")); Serial.println(_Count);

			decoder.PushBack(new TurnoutServo(_Channel, _Address, _Limit1, _Limit2, _Multiplier * _TravelTime));
			_Channel += 1;

			Serial.print(F("next channel is ")); Serial.println(_Channel);
		}
		_servogroup = (ServoGroup*)_servogroup->getNext();

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
