#include "LEDControl.h"

static uint8_t ChannelToGPIOMapping[16] = { GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_23, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_35, GPIO_NUM_34, GPIO_NUM_39, GPIO_NUM_36 };

// ===========================================
// LED
// ===========================================
LED::LED(const uint8_t Channel_) :
	Channel(Channel_) {

	PWMResolution = 8;
	PWMFrequency = 5000;

	GPIO = ChannelToGPIOMapping[Channel];

	ledcSetup(Channel, PWMFrequency, PWMResolution);
	ledcAttachPin(GPIO, Channel);

	off();
}

LED::~LED() {
	ledcDetachPin(GPIO);
}

void LED::process() {};

void LED::on() {
	IsActive = true;
	ledcWrite(Channel, PWM_Set_On);
}

void LED::off() {
	IsActive = false;
	ledcWrite(Channel, PWM_Set_Off);
}

bool LED::isOn(){
	return IsActive;
}

// ===========================================
// LEDFader
// ===========================================
LEDFader::LEDFader(const uint8_t Channel_) :
	fadeUpIntervall(10),
	fadeDownIntervall(10),
	CurrentBrightness(PWM_Set_Off),
	TargetBrightness(PWM_Set_Off),
	LED(Channel_),
	Brightness(PWM_Set_On) {
	
	SetFadeMultiplikator(1);
	SetFadeTime(500, 500, fadeUpIntervall, fadeDownIntervall);

	off();
}

LEDFader::LEDFader(const uint8_t Channel_, uint8_t Brightness_, uint16_t fadeUpTime_, uint16_t fadeDownTime_) :
	fadeUpIntervall(10),
	fadeDownIntervall(10),
	CurrentBrightness(PWM_Set_Off),
	TargetBrightness(PWM_Set_Off),
	LED(Channel_),
	Brightness(Brightness_) {

	SetFadeMultiplikator(1);
	SetFadeTime(fadeUpTime_, fadeDownTime_, fadeUpIntervall, fadeDownIntervall);

	off();
}

// Muss regelmässig aufgerufen werden
void LEDFader::process() {

	LED::process();

	int16_t _direction;

	if (CurrentBrightness != TargetBrightness) {
		_direction = TargetBrightness - CurrentBrightness;

		// Fade up
		if (_direction > 0) {

			if (fadeUpRate == 0) {
				// No fading, set direct
				CurrentBrightness = TargetBrightness;

			} else if (fadeUpTimer.repeat()) {
				// fading
				CurrentBrightness = CurrentBrightness + fadeUpRate;
				if (CurrentBrightness > TargetBrightness) {
					CurrentBrightness = TargetBrightness;
				}
			}
		}
		// Fade down
		else if (_direction < 0) {

			if (fadeDownRate == 0) {
				// Set direct the Brightness
				CurrentBrightness = TargetBrightness;

			} else if (fadeDownTimer.repeat()) {
				// fading
				CurrentBrightness = CurrentBrightness - fadeDownRate;

				if (CurrentBrightness < TargetBrightness) {
					CurrentBrightness = TargetBrightness;
				}
			}
		}
	}

	//Serial.print("    Current:"); Serial.println(CurrentBrightness);
	//Serial.print("    Target:"); Serial.println(TargetBrightness);

	ledcWrite(Channel, CurrentBrightness);
	
}

void LEDFader::on() {
	IsActive = true;
	TargetBrightness = Brightness;
}

void LEDFader::on(uint8_t Brightness_) {
	IsActive = true;
	SetBrightness(Brightness_);
};

void LEDFader::off() {
	IsActive = false;
	TargetBrightness = PWM_Set_Off;
}

// Brightness in Prozent 0 - 100; , wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
void LEDFader::SetPercent(uint8_t Percent_, bool Hardset_) {
	IsActive = true;
	SetBrightness(((uint32_t)Percent_ * 255) / 100, Hardset_);
}

// Brightness von 0 - 255, wenn Hardset == true dann wird der Wert direkt gesetzt ohne fading
void LEDFader::SetBrightness(uint16_t Brightness_, bool Hardset_) {
	if (PWM_Set_On == 0) {
		Brightness_ = 255 - Brightness_;

		if (Brightness_ > PWM_Set_Off) {
			TargetBrightness = PWM_Set_Off;
		}
		else if (Brightness_ < PWM_Set_On) {
			TargetBrightness = PWM_Set_On;
		} else {
			TargetBrightness = Brightness_;
		}
	}
	else {
		if (Brightness_ >= Brightness)
			TargetBrightness = Brightness;
		else
			TargetBrightness = Brightness_;
	}

	//Serial.println("LEDFader::SetBrightness");
	//Serial.print("    new Brightness: "); Serial.println(Brightness_);
	//Serial.print("    Brightness:     "); Serial.println(Brightness);
	//Serial.print("    Target:         "); Serial.println(TargetBrightness);

	//if (TargetBrightness != 0)
	//	TargetBrightness = Brightness_;

	if (IsActive && Hardset_) {
		CurrentBrightness = TargetBrightness;
	}
}

// Setzen der maximalen Helligkeit
void LEDFader::SetMaxBrightness(uint16_t Max_) {
	if (PWM_Set_On == 0) {
		Max_ = 255 - Max_;
		Brightness = Max_;
	}
	else {
		Brightness = Max_;
	}

	if (TargetBrightness != 0)
		TargetBrightness = Brightness;
}

// Time in ms
void LEDFader::SetFadeTime(uint16_t fadeUpTime_, uint16_t fadeDownTime_) {
	fadeUpTimer.start(fadeUpTime_);
	fadeDownTimer.start(fadeDownTime_);
}

// Time in ms, zudem kann das Intevall angepasst werden, kleinst möglicher Wert ist 10ms
// maximale Schritte sind 255, je mehr Schritte umso flüssiger ist das fading
// durch erhöhen von fadeIntervall können längere Zeiten erreicht werden
void LEDFader::SetFadeTime(uint16_t fadeUpTime_, uint16_t fadeDownTime_, uint16_t fadeUpIntervall_, uint16_t fadeDownIntervall_) {
	uint16_t  fadeAmount_;
	Serial.println("LEDFader::SetFadeTime");
	Serial.print("    fadeUpTime_       : "); Serial.println(fadeUpTime_);
	Serial.print("    fadeDownTime_     : "); Serial.println(fadeDownTime_);
	Serial.print("    fadeUpIntervall_  : "); Serial.println(fadeUpIntervall_);
	Serial.print("    fadeDownIntervall_: "); Serial.println(fadeDownIntervall_);

	fadeAmount_ = 0;
	if (fadeUpIntervall_ < 10) fadeUpIntervall_ = 10;
	if (fadeUpIntervall_ < 10) fadeUpIntervall_ = 10;

	if (fadeUpTime_ > 0) {
		int _denominator = fadeUpTime_ / fadeUpIntervall_;
		if (_denominator != 0)
			fadeAmount_ = 255 / _denominator;
	}
	fadeUpRate = fadeAmount_;

	fadeAmount_ = 0;
	if (fadeDownTime_ > 0) {
		int _denominator = fadeDownTime_ / fadeDownIntervall_;
		if (_denominator != 0)
			fadeAmount_ = 255 / _denominator;
	}
	fadeDownRate = fadeAmount_;

	SetFadeTime(fadeUpTime_, fadeDownTime_);

}

// Mit diser funktion kann das Intervall zusätzlich erhöht werden. Sollte nicht mehr verwendet werden
void LEDFader::SetFadeMultiplikator(uint8_t Value_) {
	FadeMultiplikator = Value_;
}

// Liefert die aktuelle helligkeit
uint8_t LEDFader::GetCurrentBrightness() {
	uint8_t _result = 255 - CurrentBrightness;

	if (PWM_Set_On == 0) {
		if (_result < PWM_Set_On) {
			_result = PWM_Set_On;
		}
	}
	else {
		_result =  CurrentBrightness;
	}
	return _result;
}

// Prüft ob die LED ganz Dunkel ist
bool LEDFader::isDark() {
	return CurrentBrightness == PWM_Set_Off;
}

// ===========================================
// NatriumLamp
// ===========================================
Natrium::Natrium(const uint8_t Channel_, const bool  IsMalFunction_, uint8_t fadeOnIntervall_, uint8_t fadeOffIntervall_) :
	LEDFader(Channel_, PWM_Set_On, 0, 0),
	IsMalFunction(IsMalFunction_) {

	SetFadeTime(fadeOnIntervall_, fadeOffIntervall_);
	off(true);
};

void Natrium::process() {

	LEDFader::process();

	if (Operationtimer.repeat()) {

		// Lampe glüht und baut nun Betriebsdruck auf
		if (CurrentStatus == status::GlowingOn) {
			CurrentStatus = status::OperatingPressure;
			SetPercent(random(randomMin, randomMax), true);

			// Wenn defekt, dann Ausfallzeit berechnen
			if (IsMalFunction) {
				Malfunctiontimer.start(random(MalFunctionIntervalMin, MalFunctionIntervalMax));
			}

		}

		// Lampe baut Betriebsdruck auf
		else if ((CurrentStatus == status::OperatingPressure) && (GetCurrentBrightness() != Brightness)) {
			SetBrightness(GetCurrentBrightness() + 5);
		}

		// Lampe hat Betriebsdruck erreicht
		else if ((CurrentStatus == status::OperatingPressure) && (GetCurrentBrightness() == Brightness)) {
			CurrentStatus = status::On;
			Serial.println("Natrium::process, Lampe hat Betriebsdruck erreicht");
		}

		// Lampe Glüht aus
		else if ((CurrentStatus == status::GlowingOff) && (GetCurrentBrightness() != PWM_Set_Off) && IsActive) {
			SetBrightness(GetCurrentBrightness() - 3);
		}

		// Lampe ist Ausgeglüht
		else if ((CurrentStatus == status::GlowingOff) && (GetCurrentBrightness() == PWM_Set_Off) && IsActive) {
			CurrentStatus = status::Off;
		}

		// Lampe ist defekt und fällt nun aus
		else if ((CurrentStatus == status::OperatingPressure || CurrentStatus == status::On) && (IsActive) && (IsMalFunction) && (Malfunctiontimer.repeat())) {
			// Ausschalten, Lampe glüht noch
			CurrentStatus = status::GlowingOff;
			SetPercent(random(randomMin, randomMax), true);

			// Einschaltzeit berechnen
			Malfunctiontimer.start(random(MalFunctionIntervalMin, MalFunctionIntervalMax));
		}

		// Lampe ist defekt und schaltet nun wieder ein
		else if ((CurrentStatus == status::GlowingOff || CurrentStatus == status::Off) && IsActive && IsMalFunction && (Malfunctiontimer.repeat())) {
			// Vorglühen 
			CurrentStatus = status::GlowingOn;
			SetPercent(random(randomMin, randomMax), true);
		}


	}
};

void Natrium::on() {
	Serial.println("Natrium::on");
	IsActive = true;

	// Vorglühen 
	CurrentStatus = status::GlowingOn;


	Operationtimer.start(OperationTime);
};

void Natrium::off() {
	Serial.println("Natrium::off");
	IsActive = false;

	// Ausschalten, Lampe glüht noch
	CurrentStatus = status::GlowingOff;
	SetPercent(random(randomMin, randomMax), true);
};

void Natrium::off(bool force_) {
	Serial.println("Natrium::off(bool force_)");
	if (force_) {
		LEDFader::off();
		CurrentStatus = status::Off;
	}
	else {
		off();
	}
};

// ===========================================
// NeonLamp
// ===========================================
Neon::Neon(const uint8_t Channel_, const bool MalFunction_) :
	LED(Channel_),
	IsMalFunction(MalFunction_) {
};

void Neon::process() {

	LED::process();

	// do nothing if not active and not is running
	if (!IsActive)
		return;

	// if time to do something, do it
	if (Operationtimer.repeat()) {

		// Wir müssen blinken
		if (FlashCounter > 0) {

			if (CurrentStatus) {
				// Einsachltverzögerung
				Operationtimer.set(random(300, 500));
				// Lampe aus
				ledcWrite(Channel, PWM_Set_Off);
			}
			else {
				// Ausschaltverzögerung
				Operationtimer.set(random(200, 400));
				// Lampe ein
				ledcWrite(Channel, PWM_Set_On);
			}

			// Defekte Lampe, Einschaltverzögern darf grösser sein
			if (CurrentStatus && IsMalFunction)
				Operationtimer.set(random(200, 2000));

			// Blinker runterzählen wenn Lampe nicht defekt ist
			if (!IsMalFunction)
				FlashCounter--;

			CurrentStatus = !CurrentStatus;

		}
		else {
			// Wir müssen nicht mehr blinken, Lampe ein
			ledcWrite(Channel, PWM_Set_On);
		}
	}
}

void Neon::on() {
	IsActive = true;
	CurrentStatus = false;

	// Blinker festlegen
	FlashCounter = random(1, 10);

	// Defekte Lampe dann muss m_Flashcount auf jedenfall grösser als 0 sein
	if (IsMalFunction)
		FlashCounter = 1;

	// Wie lange soll gewartet werden bis zum ersten Einschalten
	Operationtimer.start(random(400, 600));
}

void Neon::off() {
	LED::off();
}
