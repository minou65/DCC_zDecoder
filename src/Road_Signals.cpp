
#include "Road_Signals.h"
#include "neotimer.h"

Road_Signal_70::Road_Signal_70(uint16_t BaseChannel_, byte BasePort_, uint16_t DayLightAddress_, byte Brightness1_, byte Brightness2_, byte fadeUpTime_, byte fadeDownTime_, byte Delay_):
	Signal(BaseChannel_, BaseChannel_, DayLightAddress_, 102, Brightness1_, Brightness2_),
	// Ausgänge einrichten
	Red(BaseChannel_, Brightness1_, fadeUpTime_, fadeDownTime_),
	Orange(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_),
	Green(BaseChannel_ + 1, Brightness1_, fadeUpTime_, fadeDownTime_) {

	OperationTimer.set(Delay_);
};

void Road_Signal_70::notifyAddress(uint16_t Address_, uint8_t cmd_) {

	// Fahrbegriff 0 (Rot)
	if (Address_ - BaseAddress == 0) {
		NewFahrbegriff = 0;
	};

	// Fahrbegriff 1 (Grün)
	if (Address_ - BaseAddress == 1) {
		NewFahrbegriff = 1;
	};

	// Fahrbegriff 2 (blinken)
	if (Address_ - BaseAddress == 2) {
		NewFahrbegriff = 2;
	};


	// Helligkeit des Signals setzen
	if (Address_ == DayLightAddress) {
		Red.SetBrightness(Brightness1);
		Orange.SetBrightness(Brightness1);
		Green.SetBrightness(Brightness1);
	};

	if (Address_ == DayLightAddress + 1) {
		Red.SetBrightness(Brightness2);
		Orange.SetBrightness(Brightness2);
		Green.SetBrightness(Brightness2);

	};
};

void Road_Signal_70::process() {
};
