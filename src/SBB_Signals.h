// SBB_Signal.h

#ifndef _SBB_SIGNAL_h
#define _SBB_SIGNAL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "accessories.h"
#include "LEDControl.h"

//=======================================================
//SBB Hauptsignal mit 2 Lampen
//Mode 102
//Type L
//=======================================================
class SBB_Hauptsignal_102 : public Signal {
private:
	LEDFader LED1;		// grün
	LEDFader LED2;		// rot

public:
	SBB_Hauptsignal_102(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();

};

/* ====================================================
SBB Hauptsignal mit 3 Lampen
Mode 103
Type L
======================================================= */
class SBB_Hauptsignal_103 : public Signal {
private:
	LEDFader LED1;		// grün
	LEDFader LED2;		// rot
	LEDFader LED3;		// orange

public:
	SBB_Hauptsignal_103(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();

};

/* ====================================================
SBB Hauptsignal mit 4 Lampen
Mode 104
Type L
======================================================= */
class SBB_Hauptsignal_104 : public Signal {
private:
	LEDFader LED1;		// grün
	LEDFader LED2;		// rot
	LEDFader LED3;		// orange
	LEDFader LED4;		// grün

public:
	SBB_Hauptsignal_104() = default;
	SBB_Hauptsignal_104(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();



};

/* ====================================================
SBB Hauptsignal mit 5 Lampen (Schneider)
Mode 105
Type L
======================================================= */
class SBB_Hauptsignal_105 : public Signal {
private:
	LEDFader LED1;		// grün
	LEDFader LED2;		// orange
	LEDFader LED3;		// grün
	LEDFader LED4;		// rot
	LEDFader LED5;		// orange

public:
	SBB_Hauptsignal_105(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();

};

/* ====================================================
SBB Hauptsignal mit 5 Lampen (Microscale)
Mode 106
Type L
======================================================= */
class SBB_Hauptsignal_106 : public Signal {
private:
	LEDFader LED1;		// grün
	LEDFader LED2;		// orange
	LEDFader LED3;		// grün
	LEDFader LED4;		// rot
	LEDFader LED5;		// orange

public:
	SBB_Hauptsignal_106(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();

};

/* ====================================================
SBB Zwergsignal
Mode 110
Type
======================================================= */
class SBB_Zwergsignal_110 : public Signal {
private:
	LEDFader LED1;
	LEDFader LED2;
	LEDFader LED3;

public:
	SBB_Zwergsignal_110(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();

};

/* ====================================================
SBB Vorsignal mit 4 Lampen
Mode 201
Type L
======================================================= */
class SBB_Vorsignal_201 : public Signal {
private:
	LEDFader LED1;		// orange
	LEDFader LED2;		// orange
	LEDFader LED3;		// grün
	LEDFader LED4;		// grün

	uint16_t HauptsignalAddress;	// Adresse des Hauptsignals am selben Masten
	bool dunkel;					// True wenn Vorsignal dunkel geschaltet ist

public:
	SBB_Vorsignal_201(uint16_t BaseAddress_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_, uint16_t HauptsignalAddress_ = 0);
	void notifyAddress(uint16_t Address_, uint8_t cmd_);
	void process();

};


#endif
