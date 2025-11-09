/*
 Name:		zDecoder.ino
 Created:	14.10.2023 10:58:37
 Author:	andy
*/

#include <Arduino.h>

#include "version.h"
#include "pinmapping.h"

#include "webhandling.h"

#include "ESP32_DecoderCore.h"

char Version[] = VERSION_STR;

// Task handle (Core 0 on ESP32)
TaskHandle_t TaskHandle;

bool ResetDCCDecoder = false;


void zDecoderReset() {
	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		outputgroup_->applyDefaultValues();
		outputgroup_->setActive(false);

		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}
}

void zDecoderInit(void) {
	decoder.reset(); // Clear the decoder list	

	Serial.println("======== Setting up outputs... =========");
	uint8_t OutputIndex_ = 0;
	uint8_t totalAssignedPins_ = 0;
	OutputGroup* outputgroup_ = &OutputGroup1;
	while (outputgroup_ != nullptr) {
		if (ESP32PWM::PWMCount > NUM_PWM) {
			Serial.print("No more pwm channels available for servos! max: "); Serial.println(NUM_PWM);
			break;
		}

		if (OutputIndex_ >= MAX_OUTPUT_PINS) {
			Serial.println("No more output pins available!");
			break;
		}

		if (outputgroup_->isActive()) {
			// Read parameters
			uint8_t Mode_ = outputgroup_->getMode();
			uint8_t assignedPins_ = outputgroup_->getNumber();
			uint16_t Address_ = outputgroup_->getAddress();
			uint16_t TimeOn_ = outputgroup_->getTimeOn();
			uint16_t TimeOff_ = outputgroup_->getTimeOff();
			uint16_t Multiplier_ = outputgroup_->getMultiplier();
			uint16_t TimeOnFade_ = outputgroup_->getTimeOnFade();
			uint16_t TimeOffFade_ = outputgroup_->getTimeOffFade();
			uint8_t Brightness_ = outputgroup_->getBrightness();

			Serial.printf("Values for output index %u loaded\n", OutputIndex_);
			Serial.printf("    Pins assigned to this output group:: %u\n", assignedPins_);
			Serial.printf("    Address: %u\n", Address_);
			Serial.print(F("    Mode: ")); Serial.println(Mode_);

			Accessory* newAccessory_ = nullptr;

			// Prüfen auf Servomode
			if (Mode_ == 190 || Mode_ == 191 || Mode_ == 192 || Mode_ == 193) {
				// Beispiel: ServoAccessory-Konstruktor anpassen, falls andere Parameter benötigt werden
				// Accessory* createServoAccessory(uint16_t Mode, uint16_t Address, uint8_t ServoPort, uint16_t limit1, uint16_t limit2, uint16_t travelTime, uint16_t timeOn, uint16_t timeOff)
				newAccessory_ = createServoAccessory(Mode_, Address_, OutputIndex_, TimeOnFade_, TimeOffFade_, Multiplier_, TimeOn_, TimeOff_);
			}
			else {
				// Standard-Accessory
				newAccessory_ = createOutputAccessory(Mode_, Address_, OutputIndex_, assignedPins_, TimeOn_, TimeOff_, Multiplier_, TimeOnFade_, TimeOffFade_);
			}

			if (newAccessory_ != nullptr) {
				OutputIndex_ += newAccessory_->getUsedPins(); // Increase OutputIndex_ by the number of used pins

				// Set brightness if it's a LED accessory (before wrapping in SoundAccessory)
				if (newAccessory_->getType() == AccessoryType::LED) {
					Ausgang* ledAccessory = static_cast<Ausgang*>(newAccessory_);
					ledAccessory->SetMaxBrightness(Brightness_);
				}

				// add a input accessory if the output group has an input pin
				if (outputgroup_->getInputPin() > 0) {
					Serial.print("Adding input accessory for input pin: "); Serial.println(outputgroup_->getInputPin() - 1);
					Serial.print("    with active duration: "); Serial.println(outputgroup_->getActiveDuration());
					Serial.print("    and edge type: "); Serial.println(outputgroup_->getEdgeType());	
					InputAccessory* inputAccessory_ = createInputAccessory(
						outputgroup_->getInputPin() - 1,
						newAccessory_,
						outputgroup_->getActiveDuration(),
						static_cast<InputAccessory::EdgeType>(outputgroup_->getEdgeType())
					);
					decoder.add(inputAccessory_);
				}

				decoder.add(newAccessory_);
			}
			totalAssignedPins_ += assignedPins_;

			Serial.println(F("*****************************"));
		}

		outputgroup_ = (OutputGroup*)outputgroup_->getNext();
	}

	Serial.print("Decoder size: "); Serial.println(decoder.getSize());
	Serial.print("Total assigned outputs: "); Serial.println(totalAssignedPins_);
	Serial.print("Total assigned PWM channels: "); Serial.println(ESP32PWM::PWMCount);


	Serial.println(F("======== zdecoder initialization complete ========="));

}

void setup() {
	Serial.begin(115200);
	while (!Serial) {
		delay(10);
	}

	Serial.println("Starting with Firmware " + String(VERSION));

	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);

	NMRAsetup();
	websetup();

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

		decoder.process(); // Process all decoder accessories
		vTaskDelay(10 / portTICK_PERIOD_MS); // Delay to prevent watchdog timeout
	}
}
