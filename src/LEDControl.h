// LEDControl.h

#ifndef _LEDCONTROL_h
#define _LEDCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "neotimer.h"

// Values for On and Off
// Werden die LED's direkt am ESP angeschlossen sind die 
// Werte zu invertieren. O = on, 255 = off
#define PWM_Set_On 255
#define PWM_Set_Off 0

// ===========================================
// LED
// ===========================================
class LED {
protected:
	uint8_t Channel;						 // the number of the LED pin
	uint8_t GPIO;
	bool IsActive;

	uint16_t PWMFrequency;
	uint16_t PWMResolution;

public:
	LED(const uint8_t Channel_);
	~LED();

	// Muss regelmässig aufgerufen werden
	virtual void process();

	void on();
	void off();
	bool isOn();
};

// ===========================================
// LEDFader
// ===========================================
class LEDFader : public LED {
protected:
	Neotimer fadeUpTimer;
	Neotimer fadeDownTimer;

	int16_t Brightness;					// On Wert
	int16_t CurrentBrightness;			// Aktuelle Wert für LED beim faden
	int16_t TargetBrightness;			// Wert, welcher eingestellt werden soll

	// Time in ms
	uint16_t fadeUpIntervall;
	uint16_t fadeDownIntervall;

	// Anzahl Steps um die die Helligkeit der LED pro Intervall erhöht wird 1-255
	// Ist der Wert 0 wird direkt auf Helligkeit direkt auf Target gesetzt
	uint8_t fadeUpRate;
	uint8_t fadeDownRate;

	// Um besonders hohe Zeiten zu erreichen wird Intervall mit diesem Wert multipliziert. Default ist 1
	uint8_t FadeMultiplikator;

public:

	LEDFader(const uint8_t Channel_);
	LEDFader(const uint8_t Channel_, uint8_t Brightness_, uint16_t fadeUpTime_ = 1000, uint16_t fadeDownTime_ = 1000);
	~LEDFader();

	// Muss regelmässig aufgerufen werden
	void process();

	void on();
	void on(uint8_t Brightness_);
	void off();

	// Brightness in Prozent 0 - 100; , wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
	void SetPercent(uint8_t Percent_, bool Hardset_ = false);
	// Brightness von 0 - 255, wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
	void SetBrightness(uint16_t Brightness_, bool Hardset_ = false);
	// Setzen der maximalen Helligkeit
	void SetMaxBrightness(uint16_t Max_);

	// Time in ms
	void SetFadeTime(uint16_t fadeUpTime_, uint16_t fadeDownTime_);
	// Time in ms, zudem kann das Intevall angepasst werden, kleinst möglicher Wert ist 10ms
	void SetFadeTime(uint16_t fadeUpTime_, uint16_t fadeDownTime_, uint16_t fadeUpIntervall_, uint16_t fadeDownIntervall_);
	// Mit diser funktion kann das Intervall zusätzlich erhöht werden. Sollte nicht mehr verwendet werden
	void SetFadeMultiplikator(uint8_t Value_);
	// Liefert die aktuelle helligkeit
	uint8_t GetCurrentBrightness();
	// Prüft ob die LED ganz Dunkel ist
	bool isDark();
};

// ===========================================
// Simuliert eine Natriumdampflampe
// ===========================================
class Natrium : protected LEDFader {
protected:
	enum status {
		GlowingOn = 1,
		OperatingPressure = 2,
		On = 3,
		GlowingOff = 4,
		Off = 5
	};

	status CurrentStatus = status::Off;

	const uint16_t OperationTime = 500;

	uint16_t MalFunctionInterval;
	const uint16_t MalFunctionIntervalMin = 5000;
	const uint16_t MalFunctionIntervalMax = 60000;
	bool IsMalFunction;

	const uint8_t randomMax = 15;		// Definition in Prozent wie stark die Lampe beim
	const uint8_t randomMin = 5;		// Ein- und Ausschalten noch leuchten soll
	// Danach dimmt die Lampe zu 100% oder zu 0%

	Neotimer Malfunctiontimer;
	Neotimer Operationtimer;

public:
	Natrium(const uint8_t Channel_, const bool MalFunction_, uint8_t fadeOnIntervall_, uint8_t fadeOffIntervall_);
	~Natrium();
	void process();
	void on();
	void off();
	void off(bool force_);
};

// ===========================================
// Simuliert eine Neon Röhre
// ===========================================
class Neon : protected LED {
protected:
	bool CurrentStatus = false;

	const bool IsMalFunction;

	uint8_t FlashCounter;			// Wie oft soll beim einschalten geblinkt werden

	Neotimer Operationtimer;

public:
	Neon(const uint8_t Channel_, const bool MalFunction_);
	~Neon();
	void process();
	void on();
	void off();
};

#endif

