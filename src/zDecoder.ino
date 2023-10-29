/*
 Name:		zDecoder.ino
 Created:	14.10.2023 10:58:37
 Author:	andy
*/


#include "webhandling.h"
#include "Vector.h"  // https://github.com/tomstewart89/Vector/fork
#include <NmraDcc.h>
#include "accessories.h"
#include "Blinkers.h"
#include "SBB_Signals.h"
#include "Turnouts.h"
#include "NMRAhandling.h"

char Version[] = "0.0.0.1 (2023-11-01)";

// #include "NMRAhandling.h"
#include "Vector.h"

// Liste mit allen Decoderobjekten (Apps)
Vector<accessories*> decoder;

// Task handle (Core 0 on ESP32)
TaskHandle_t TaskHandle;

bool ResetDCCDecoder = false;

void notifyDccMsg(DCC_MSG* Msg) {
#ifdef  NOTIFY_DCC_MSG
	int i;
	if ((Msg->Data[0] == 0) && (Msg->Data[1] == 0)) return;  //reset packlet
	if ((Msg->Data[0] == 0b11111111) && (Msg->Data[1] == 0)) return;  //idle packet

	if (Msg->Data[0] == 100 && Msg->Data[1] == 63) return;

	Serial.println(F(" "));
	Serial.println(F("========================================= "));
	Serial.print(F(" Free Memory: "));  Serial.println(freeMemory());
	Serial.println(F(" "));

	Serial.print(F("notifyDccMsg - ")); Serial.println(Msg->Size);
	for (i = 0; i < Msg->Size; i++) {
		Serial.print(Msg->Data[i], BIN);
		Serial.print(F(" "));
	}

	Serial.println(F(" ")); Serial.println(F(" "));
#endif
}

void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower) {
#ifdef  NOTIFY_TURNOUT_MSG
	Serial.print("notifyDccAccTurnoutOutput: Turnout: ");
	Serial.print(Addr, DEC);
	Serial.print(" Direction: ");
	Serial.print(Direction ? "Closed" : "Thrown");
	Serial.print(" Output: ");
	Serial.print(OutputPower ? "On" : "Off");
#endif

	for (int i = 0; i < decoder.Size(); i++) {
		decoder[i]->notifyTurnoutAddress(Addr, Direction, OutputPower);
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
	uint8_t OutputNum = OutputAddr >> 1;  //shift over the bits so the outputaddr is 0 to 3
	uint8_t StateProper = OutputAddr & 0b00000001;  //JMRI puts out the state as the right most bit of pDccMsg->Data[1], the state argument doesnt change in JMRI Turnout.

#ifdef NOTIFY_DCC_ACCSTATE_MSG
	Serial.print("AccState - ");
	Serial.print("Raw addr: ");
	Serial.print(Addr);
	Serial.print(" BoardAddr: ");
	Serial.print(BoardAddr, DEC);
	Serial.print(" OutputAddr: ");
	Serial.print(OutputAddr, DEC);
	Serial.print(" Output: ");
	Serial.print(OutputNum);
	Serial.print(" State: 0b");
	Serial.print(State, BIN);
	Serial.print(" Status: ");
	Serial.print(StateProper == 1 ? "Closed" : "Thrown");
	Serial.println();
	//Serial.print("    zDecoder: "); Serial.println(BoardAddr + OutputAddr);
#endif 

	for (int i = 0; i < decoder.Size(); i++) {
		decoder[i]->notifyAddress(BoardAddr + OutputAddr, 1);
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

void handleChannel(uint8_t Channel_) {
	if (decoder[Channel_]->isOn()) {
		decoder[Channel_]->off();
	}
	else {
		decoder[Channel_]->on();
	}
}

bool ChannelIsOn(uint8_t Channel_) {
	return decoder[Channel_]->isOn();
}

void zDecoderInit(void) {
	ActionGroup* _group = &OutputGroup1;
	uint8_t _Channel = 0;

	// Decoderobjekte in decoder löschen
	decoder.Erase(0, decoder.Size());
	decoder.Clear();
	Vector<accessories*>().Swap(decoder);

    while (_group != nullptr) {
		if (_Channel > 15)
			break;

        if (_group->isActive()) {
            uint8_t _Mode = atoi(_group->ModeValue);
            uint8_t _Count = atoi(_group->NumberValue);
            uint8_t _Address = atoi(_group->AddressValue);
            uint8_t _TimeOn = atoi(_group->TimeOnValue);
            uint8_t _TimeOff = atoi(_group->TimeOffValue);
            uint8_t _Multiplier = atoi(_group->MultiplierValue); // Multiplikator
            uint8_t _TimeOnFade = atoi(_group->TimeOnFadeValue);
            uint8_t _TimeOffFade = atoi(_group->TimeOffFadeValue);

			uint16_t _DayLightAddress = 0;
			uint8_t _DayBrightness = 255;
			uint8_t _NightBrightness = 150;
            
			Serial.print(F("Values for channel ")); Serial.print(_Channel); Serial.println(F(" preserved"));
			Serial.print(F("    Channels used: ")); Serial.println(_Count);

            // Einrichten des Ports
			switch (_Mode) {
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

			case 61:			// Neonröhren
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

			case 230: // Servo
				decoder.PushBack(new TurnoutServo(_Channel, _Address, 0, 180, _TimeOn));
				_Channel += 1;
				break;
			}

			Serial.print(F("next free channel is ")); Serial.println(_Channel);
        }
        _group = (ActionGroup*)_group->getNext();
    }
};

void setup() {
	NMRAsetup();
	websetup();

    zDecoderInit();

	xTaskCreatePinnedToCore(
		loop2, /* Function to implement the task */
		"TaskHandle", /* Name of the task */
		10000,  /* Stack size in words */
		NULL,  /* Task input parameter */
		0,  /* Priority of the task */
		&TaskHandle,  /* Task handle. */
		0 /* Core where the task should run */
	);
}

void loop() {
	NMRAloop();
	webloop();
}

void loop2(void* parameter) {
	int _i = 0;
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
