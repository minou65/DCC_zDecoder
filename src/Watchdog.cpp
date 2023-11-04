// 
// 
// 

#include "Watchdog.h"

Watchdog::Watchdog(uint16_t BaseAddress_, byte BaseChannel_, uint16_t Interval_) :
	accessories(BaseAddress_, BaseChannel_, 1),
	Output(BaseChannel_),
	Interval(Interval_) {

	timer.start(Interval);
}

Watchdog::~Watchdog() {
	Output.~Coil();
	timer.~Neotimer();
}

void Watchdog::notifyAddress(uint16_t Address_, uint8_t cmd_) {
	// Watchdog starten
	if (Address_ == BaseAddress) {
		timer.start(Interval);
	}
}

void Watchdog::process(){
	if (timer.done()) {

		// Watchdog hat keinen Befehl innerhalb von Intervall erhalten
		Output.off();
	}
	else {

		// Watchdog hat Befehl innerhalb von Intervall erhalten
		Output.on();

	};

}
