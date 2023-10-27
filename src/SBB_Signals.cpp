#include "SBB_Signals.h"

// You can print Debug Messages uncommenting the line below
// #define PROCESS_SIGNAL_MSG

//=======================================================
//SBB Hauptsignal mit 2 Lampen
//Mode 102
//Type L
//======================================================= 
SBB_Hauptsignal_102::SBB_Hauptsignal_102(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 102, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_) {

	
};

void SBB_Hauptsignal_102::notifyAddress(uint16_t Address_, uint8_t cmd_) {

	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);

	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);

	};
};

void SBB_Hauptsignal_102::process() {
	LED1.process();
	LED2.process();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if (Fahrbegriff != NewFahrbegriff) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();

		if (LED1.isDark() &&
			LED2.isDark()) {
			Fahrbegriff = NewFahrbegriff;

		};
	};

	// Fahrbegriff anzeigen
	if (Fahrbegriff == NewFahrbegriff) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			LED1.off();		// grün
			LED2.on();		// rot
			break;

			// Fahrbegriff 1
		case 1:
			LED1.on();		// grün
			LED2.off();		// rot
			break;

		default:
			LED1.off();		// grün
			LED2.off();		// rot
			break;
		};

	};
};

//=======================================================
//SBB Hauptsignal mit 3 Lampen
//Mode 103
//Type L
//======================================================= 
SBB_Hauptsignal_103::SBB_Hauptsignal_103(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 103, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED3(BaseChannel_ + 2, Brightness1_, fadeUpTime_, fadeDownTime_) {

};

void SBB_Hauptsignal_103::notifyAddress(uint16_t Address_, uint8_t cmd_) {

	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);
		LED3.SetMaxBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);
		LED3.SetMaxBrightness(Brightness2);
	};
};

void SBB_Hauptsignal_103::process() {

	LED1.process();
	LED2.process();
	LED3.process();

	unsigned long now_ = millis();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if (Fahrbegriff != NewFahrbegriff) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();
		LED3.off();

		if (LED1.isDark() &&
			LED2.isDark() &&
			LED3.isDark()) {
			Fahrbegriff = NewFahrbegriff;
		}
	};

	// Fahrbegriff anzeigen
	if (Fahrbegriff == NewFahrbegriff) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			//LED1.off();		// grün
			LED2.on();		// rot
			//LED3.off();		// orange

			break;

			// Fahrbegriff 1
		case 1:
			LED1.on();		// grün
			//LED2.off();		// rot
			//LED3.off();		// orange

			break;

			// Fahrbegriff 2
		case 2:
			LED1.on();		// grün
			//LED2.off();		// rot
			LED3.on();		// orange

			break;

		default:
			LED1.off();		// grün
			LED2.off();		// rot
			LED3.off();		// orange

			break;
		};

	};

};

//=======================================================
//SBB Hauptsignal mit 4 Lampen
//Mode 104
//Type L
//======================================================= 
SBB_Hauptsignal_104::SBB_Hauptsignal_104(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 104, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED3(BaseChannel_ + 2, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED4(BaseChannel_ + 3, Brightness1_, fadeUpTime_, fadeDownTime_) {
};

void SBB_Hauptsignal_104::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};

	// Fahrbegriff 3
	if (Address_ - BaseAddress == 3) {
		NewFahrbegriff = 3;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);
		LED3.SetMaxBrightness(Brightness1);
		LED4.SetMaxBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);
		LED3.SetMaxBrightness(Brightness2);
		LED4.SetMaxBrightness(Brightness2);
	};
};

void SBB_Hauptsignal_104::process() {
	LED1.process();
	LED2.process();
	LED3.process();
	LED4.process();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if (Fahrbegriff != NewFahrbegriff) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();
		LED3.off();
		LED4.off();
		
		if (LED1.isDark() &&
			LED2.isDark() &&
			LED3.isDark() &&
			LED4.isDark()) {
			Fahrbegriff = NewFahrbegriff;

		};
	};

	// Fahrbegriff anzeigen
	if (Fahrbegriff == NewFahrbegriff) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			LED1.off();		// grün
			LED2.on();		// rot
			LED3.off();		// orange
			LED4.off();		// grün

			break;

			// Fahrbegriff 1
		case 1:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.off();		// grün

			break;

			// Fahrbegriff 2
		case 2:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.on();		// orange
			LED4.off();		// grün

			break;

			// Fahrbegriff 3
		case 3:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.on();		// grün

			break;

		default:
			LED1.off();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.off();		// grün

			break;
		};

	};

};

//=======================================================
//SBB Hauptsignal mit 5 Lampen (Schneider)
//Mode 105
//Type L
//======================================================= 

SBB_Hauptsignal_105::SBB_Hauptsignal_105(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 105, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED3(BaseChannel_ + 2, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED4(BaseChannel_ + 3, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED5(BaseChannel_ + 4, Brightness1_, fadeUpTime_, fadeDownTime_) {
}

void SBB_Hauptsignal_105::notifyAddress(uint16_t Address_, uint8_t cmd_) {

	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};

	// Fahrbegriff 3
	if (Address_ - BaseAddress == 3) {
		NewFahrbegriff = 3;
	};

	// Fahrbegriff 4
	if (Address_ - BaseAddress == 4) {
		NewFahrbegriff = 4;
	};

	// Fahrbegriff 6
	if (Address_ - BaseAddress == 5) {
		NewFahrbegriff = 6;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);
		LED3.SetMaxBrightness(Brightness1);
		LED4.SetMaxBrightness(Brightness1);
		LED5.SetMaxBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);
		LED3.SetMaxBrightness(Brightness2);
		LED4.SetMaxBrightness(Brightness2);
		LED5.SetMaxBrightness(Brightness2);
	};
};

void SBB_Hauptsignal_105::process() {

	LED1.process();
	LED2.process();
	LED3.process();
	LED4.process();
	LED5.process();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if (Fahrbegriff != NewFahrbegriff) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();
		LED3.off();
		LED4.off();
		LED5.off();

		if (LED1.isDark() &&
			LED2.isDark() &&
			LED3.isDark() &&
			LED4.isDark() &&
			LED5.isDark()) {
			Fahrbegriff = NewFahrbegriff;
			
		};
	};

	// Fahrbegriff anzeigen
	if (Fahrbegriff == NewFahrbegriff) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			//LED1.off();		// grün
			//LED2.off();		// orange
			//LED3.off();		// grün
			LED4.on();		// rot
			//LED5.off();		// orange

			break;

			// Fahrbegriff 1
		case 1:
			LED1.on();		// grün
			//LED2.off();		// orange
			//LED3.off();		// grün
			//LED4.off();		// rot
			//LED5.off();		// orange

			break;

			// Fahrbegriff 2
		case 2:
			LED1.on();		// grün
			//LED2.off();		// orange
			//LED3.off();		// grün
			//LED4.off();		// rot
			LED5.on();		// orange

			break;

			// Fahrbegriff 3
		case 3:
			LED1.on();		// grün
			//LED2.off();		// orange
			LED3.on();		// grün
			//LED4.off();		// rot
			//LED5.off();		// orange

			break;

			// Fahrbegriff 4
		case 4:
			LED1.on();		// grün
			//LED2.off();		// orange
			LED3.on();		// grün
			//LED4.off();		// rot
			LED5.on();		// orange

			break;

			// Fahrbegriff 6
		case 6:
			//LED1.off();		// grün
			LED2.on();		// orange
			//LED3.off();		// grün
			//LED4.off();		// rot
			LED5.on();		// orange

			break;
		};

	};

};

//=======================================================
//SBB Hauptsignal mit 5 Lampen (Microscale)
//Mode 106
//Type L
//======================================================= 
SBB_Hauptsignal_106::SBB_Hauptsignal_106(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 106, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED3(BaseChannel_ + 2, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED4(BaseChannel_ + 3, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED5(BaseChannel_ + 4, Brightness1_, fadeUpTime_, fadeDownTime_) {
};

void SBB_Hauptsignal_106::notifyAddress(uint16_t Address_, uint8_t cmd_) {

	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};

	// Fahrbegriff 3
	if (Address_ - BaseAddress == 3) {
		NewFahrbegriff = 3;
	};

	// Fahrbegriff 4
	if (Address_ - BaseAddress == 4) {
		NewFahrbegriff = 4;
	};

	// Fahrbegriff 6
	if (Address_ - BaseAddress == 5) {
		NewFahrbegriff = 6;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);
		LED3.SetMaxBrightness(Brightness1);
		LED4.SetMaxBrightness(Brightness1);
		LED5.SetMaxBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);
		LED3.SetMaxBrightness(Brightness2);
		LED4.SetMaxBrightness(Brightness2);
		LED5.SetMaxBrightness(Brightness2);
	};
};

void SBB_Hauptsignal_106::process() {

	LED1.process();
	LED2.process();
	LED3.process();
	LED4.process();
	LED5.process();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if (Fahrbegriff != NewFahrbegriff) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();
		LED3.off();
		LED4.off();
		LED5.off();

		if (LED1.isDark() &&
			LED2.isDark() &&
			LED3.isDark() &&
			LED4.isDark() &&
			LED5.isDark()) {
			Fahrbegriff = NewFahrbegriff;

		};
	};

	// Fahrbegriff anzeigen
	if (Fahrbegriff == NewFahrbegriff) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			LED1.off();		// grün
			LED2.on();		// rot
			LED3.off();		// orange
			LED4.off();		// grün
			LED5.off();		// orange

			break;

			// Fahrbegriff 1
		case 1:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.off();		// grün
			LED5.off();		// orange

			break;

			// Fahrbegriff 2
		case 2:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.on();		// orange
			LED4.off();		// grün
			LED5.off();		// orange

			break;

			// Fahrbegriff 3
		case 3:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.on();		// grün
			LED5.off();		// orange

			break;

			// Fahrbegriff 4
		case 4:
			LED1.on();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.on();		// grün
			LED5.on();		// orange

			break;

			// Fahrbegriff 6
		case 6:
			LED1.off();		// grün
			LED2.off();		// rot
			LED3.on();		// orange
			LED4.off();		// grün
			LED5.on();		// orange

			break;

		default:
			LED1.off();		// grün
			LED2.off();		// rot
			LED3.off();		// orange
			LED4.off();		// grün
			LED5.off();		// orange

			break;
		};

	};

};

//=======================================================
//SBB Vorsignal mit 4 Lampen
//Mode 201
//Type L
//======================================================= 
SBB_Vorsignal_201::SBB_Vorsignal_201(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_, uint16_t HauptsignalAddress_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 201, Brightness1_, Brightness2_),
	HauptsignalAddress(HauptsignalAddress_),
	dunkel(false),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED3(BaseChannel_ + 2, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED4(BaseChannel_ + 3, Brightness1_, fadeUpTime_, fadeDownTime_) {

	if (HauptsignalAddress > 0) {
		dunkel = true;
		Fahrbegriff = 255;
	}
};

void SBB_Vorsignal_201::notifyAddress(uint16_t Address_, uint8_t cmd_) {


	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};

	// Fahrbegriff 3
	if (Address_ - BaseAddress == 3) {
		NewFahrbegriff = 3;
	};

	// Fahrbegriff 4
	if (Address_ - BaseAddress == 4) {
		NewFahrbegriff = 4;
	};

	// Fahrbegriff 6
	if (Address_ - BaseAddress == 5) {
		NewFahrbegriff = 6;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);
		LED3.SetMaxBrightness(Brightness1);
		LED4.SetMaxBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);
		LED3.SetMaxBrightness(Brightness2);
		LED4.SetMaxBrightness(Brightness2);
	};

	// Hauptsignal am selben Mast zeigt rot => Vorsignal dunkel schalten
	if (Address_ - HauptsignalAddress == 0) {
		dunkel = true;
		Fahrbegriff = 255;
	};

	// Hauptsignal am selben Mast <> rot => Vorsignal einschalten
	if ((Address_ - HauptsignalAddress > 0) && (Address_ - HauptsignalAddress <= 6)) {
		dunkel = false;
	};
};

void SBB_Vorsignal_201::process() {

	LED1.process();
	LED2.process();
	LED3.process();
	LED4.process();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if ((Fahrbegriff != NewFahrbegriff) && !dunkel) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();
		LED3.off();
		LED4.off();
		
		if (LED1.isDark() &&
			LED2.isDark() &&
			LED3.isDark() &&
			LED4.isDark()) {
			Fahrbegriff = NewFahrbegriff;

		};
	};

	// Fahrbegriff anzeigen
	if ((Fahrbegriff == NewFahrbegriff) || dunkel) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			LED1.on();	LED2.on();		// 2 orange
			LED4.off();		// grün
			LED3.off();						// grün

			break;

			// Fahrbegriff 1
		case 1:
			LED1.off();	LED2.off();		// 2 orange
			LED4.on();		// grün
			LED3.on();						// grün
			break;

			// Fahrbegriff 2
		case 2:
			LED1.on();	LED2.off();		// 2 orange
			LED4.on();		// grün
			LED3.off();						// grün

			break;

			// Fahrbegriff 3
		case 3:
			LED1.on();	LED2.off();		// 2 orange
			LED4.on();		// grün
			LED3.on();						// grün

			break;

			// Fahrbegriff 4
		case 4:
			LED1.on();	LED2.off();		// 2 orange
			LED4.on();		// grün
			LED3.off();						// grün

			break;

		default:
			LED1.off();	LED2.off();		// 2 orange
			LED4.off();		// grün
			LED3.off();						// grün

			break;
		};

	};

}

//=======================================================
//SBB Zwergsignal
//Mode 110
//Type
//======================================================= 
SBB_Zwergsignal_110::SBB_Zwergsignal_110(uint16_t BaseAddress_, byte BaseChannel_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_) :
	Signal(BaseAddress_, BaseChannel_, DayLightAddress_, 110, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	LED1(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED2(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	LED3(BaseChannel_ + 2, Brightness1_, fadeUpTime_, fadeDownTime_) {
};

void SBB_Zwergsignal_110::notifyAddress(uint16_t Address_, uint8_t cmd_) {

#ifdef PROCESS_SIGNAL_MSG
	Serial.println(F("========================================="));
	Serial.print(F("notifyAddress, Mode ")); Serial.println(Mode, DEC);
	Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
	Serial.print(F("   Address:           ")); Serial.println(Address_, DEC);

	Serial.println(F(" "));
#endif
	// Fahrbegriff 0 (Halt)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Freie Fahrt)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};

	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		LED1.SetMaxBrightness(Brightness1);
		LED2.SetMaxBrightness(Brightness1);
		LED3.SetMaxBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		LED1.SetMaxBrightness(Brightness2);
		LED2.SetMaxBrightness(Brightness2);
		LED3.SetMaxBrightness(Brightness2);
	};
};

void SBB_Zwergsignal_110::process() {

	LED1.process();
	LED2.process();
	LED3.process();

	// Wenn Fahrbegriff ändert zuerst alles dunkel schalten
	if (Fahrbegriff != NewFahrbegriff) {

#ifdef PROCESS_SIGNAL_MSG
		Serial.println(F("========================================="));
		Serial.print(F("process Signal, Mode ")); Serial.println(Mode, DEC);
		Serial.print(F("   BaseAdresse:       ")); Serial.println(BaseAddress, DEC);
		Serial.print(F("   BasePin:           ")); Serial.println(BaseChannel, DEC);
		Serial.print(F("   Fahrbegriff:       ")); Serial.println(Fahrbegriff, DEC);
		Serial.print(F("   Neuer Fahrbegriff: ")); Serial.println(NewFahrbegriff, DEC);
		Serial.print(F("   Helligkeit1:       ")); Serial.println(Brightness1, DEC);
		Serial.print(F("   Helligkeit2:       ")); Serial.println(Brightness1, DEC);
		Serial.println(F(" "));
#endif

		LED1.off();
		LED2.off();
		LED3.off();
		
		if (LED1.isDark() &&
			LED2.isDark() &&
			LED3.isDark()) {
			Fahrbegriff = NewFahrbegriff;

		};
	};

	// Fahrbegriff anzeigen
	if (Fahrbegriff == NewFahrbegriff) {
		switch (Fahrbegriff)
		{
			// Fahrbegriff 0
		case 0:
			LED1.off();
			LED2.on();	LED3.on();

			break;

			// Fahrbegriff 1
		case 1:
			LED1.on();
			LED2.on();	LED3.off();

			break;

			// Fahrbegriff 2
		case 2:
			LED1.on();
			LED2.off();	LED3.on();

			break;

		default:
			LED1.off();
			LED2.off();	LED3.off();

			break;
		};

	};

};
