// 
// 
// 

//#define NOTIFY_DCC_ACCSTATE_MSG
//#define NOTIFY_DCC_MSG
//#define NOTIFY_TURNOUT_MSG
//#define NOTIFY_DCC_CV_WRITE_MSG
//#define NOTIFY_DCC_CV_CHANGE_MSG
//#define DEBUG_MSG

#include "NMRAhandling.h"
#include "common.h"

uint8_t FactoryDefaultCVIndex = 0;

NmraDcc  Dcc;
DCC_MSG  Packet;

struct CVPair {
	uint16_t  CV;
	uint8_t   Value;
};

CVPair FactoryDefaultCVs[] = {
	{ CV_ACCESSORY_DECODER_ADDRESS_LSB,        DCC_Default_Addr },   // CV 1 Board Address (lower 6 bits) 
	{ CV_ACCESSORY_DECODER_ADDRESS_MSB,        0 },   // CV 9 Board Address (Upper 3 bits)

	{ CV_VERSION_ID, DCC_DECODER_VERSION_NUM },		// Version
	{ CV_MANUFACTURER_ID,	13 },					// Öffentliche und Selbstbaudekoder

	{ CV_DECODER_UNLOCK ,	0 },
	{ CV_DECODER_CARRIER ,	0 },

	{ CV_29_CONFIG, 192 },

	{ CV_DCC_POM_PROG, DCC_Default_Addr },
	{ CV_DCC_POM_PROG + 1, 0 },

};

// This functions should be eriodically called
extern void notifyDccAccTurnoutOutput(uint16_t Addr, uint8_t Direction, uint8_t OutputPower);
extern void notifyDccAccState(uint16_t Addr, uint16_t BoardAddr, uint8_t OutputAddr, uint8_t State);
extern void notifyDccSpeed(uint16_t Addr, uint8_t Speed, uint8_t ForwardDir, uint8_t SpeedSteps);
extern void notifyDccFunc(uint16_t Addr, uint8_t FuncNum, uint8_t FuncState);
extern void notifyDccMsg(DCC_MSG* Msg);

extern uint8_t notifyCVWrite(uint16_t CV, uint8_t Value) {

#ifdef NOTIFY_DCC_CV_WRITE_MSG

	byte CV16_ = Dcc.getCV(CV_DECODER_CARRIER);
	byte CV15_ = Dcc.getCV(CV_DECODER_UNLOCK);

	Serial.println("notifyCVWrite MSG");
	Serial.print(F("  CV16 (Carrier): ")); Serial.println(CV16_);
	Serial.print(F("  CV15 (Unlock): ")); Serial.println(CV15_);
	Serial.println(CV15_ == CV16_ ? "Decoder is unlocked" : "Decoder is locked!");
	Serial.println(" ");
#endif // NOTIFY_DCC_CV_WRITE_MSG
	return 0;
};

extern void notifyCVChange(uint16_t CV, uint8_t Value) {

#ifdef NOTIFY_DCC_CV_CHANGE_MSG
	Serial.print(F("notifyCVChange: CV: "));
	Serial.print(CV, DEC);
	Serial.print(F(" Value: "));
	Serial.println(Value, DEC);
#endif  
};

extern void notifyCVAck(void) {

#ifdef NOTIFY_DCC_CV_ACK_MSG
	Serial.println("notifyCVAck");
#endif
};

extern void notifyCVResetFactoryDefault() {
	// Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset 
	// to flag to the loop() function that a reset to Factory Defaults needs to be done
	FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs) / sizeof(CVPair);
};

void NMRAsetup() {
	// Configure the DCC CV Programing ACK pin for an output
	pinMode(DccAckPin, OUTPUT);
	digitalWrite(DccAckPin, 0);

#if !defined(FORCE_RESET_FACTORY_DEFAULT_CV)
	//if eeprom has 0xFF then assume it needs to be programmed
	if (Dcc.getCV(CV_MULTIFUNCTION_PRIMARY_ADDRESS) == 0xFF) {
		Serial.println("CV Defaulting due to blank eeprom");
		notifyCVResetFactoryDefault();

	}
	else {
		Serial.println("CV Not Defaulting");
	}
#else
	Serial.println("CV Defaulting Always On Powerup");
	notifyCVResetFactoryDefault();
#endif 

	// Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up 
	Dcc.pin(0, 2, 1);

	// Call the main DCC Init function to enable the DCC Receiver
	Dcc.init(MAN_ID_DIY, DCC_DECODER_VERSION_NUM, FLAGS_DCC_ACCESSORY_DECODER, CV_DCC_POM_PROG);

	// DCC-Adresse setzen wenn keine Adresse gespeichert ist
	if (Dcc.getCV(CV_ACCESSORY_DECODER_ADDRESS_LSB) == 255) {
		Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_LSB, DCC_Default_Addr % 256);
		Dcc.setCV(CV_ACCESSORY_DECODER_ADDRESS_MSB, DCC_Default_Addr / 256);
	}

	// POM Adresse setzen wenn keine Adresse gespeichert ist
	// Default ist die selbe Adesse wie unter CV_ACCESSORY_DECODER_ADDRESS_LSB
	// gespeichert wird
	if (Dcc.getCV(CV_DCC_POM_PROG) == 255) {
		Dcc.setCV(CV_DCC_POM_PROG, DCC_Default_Addr);
		Dcc.setCV(CV_DCC_POM_PROG + 1, 0);
	}

}

void NMRAloop() {
	Dcc.process();

	if (FactoryDefaultCVIndex && Dcc.isSetCVReady()) {

		FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array
		uint16_t cv = FactoryDefaultCVs[FactoryDefaultCVIndex].CV;
		uint8_t val = FactoryDefaultCVs[FactoryDefaultCVIndex].Value;

#ifdef DEBUG_MSG
		Serial.print(F("  FactoryDefaultCV: ")); Serial.print(cv, DEC); Serial.print(F(" Value: ")); Serial.println(val, DEC);
#endif     

		Dcc.setCV(cv, val);

		// Is this the last Default CV to set? if so re-init zDecoder
		if (FactoryDefaultCVIndex == 0)
			ResetDCCDecoder = true;

	}
}
