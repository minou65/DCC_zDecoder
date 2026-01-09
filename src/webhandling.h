// webhandling.h
#pragma once

#ifndef _WEBHANDLING_h
#define _WEBHANDLING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <IotWebConf.h>
#include <IotWebConfOptionalGroup.h>
#include <IotWebConfParameter.h>
#include "common.h"

#include <ArduinoOTA.h>

#include <WiFi.h>
#include <DNSServer.h>
#include <IotWebConf.h>
#include <IotWebConfAsync.h>
#include <IotWebConfAsyncUpdateServer.h>
#include <IotWebRoot.h>

void websetup();
void webloop();


class MySelectParameter : public iotwebconf::SelectParameter {
public:
    MySelectParameter(
        const char* label,
        const char* id,
        char* valueBuffer,
        int length,
        const char* optionValues,
        const char* optionNames,
        size_t optionCount,
        size_t nameLength,
        const char* defaultValue,
        const char* customHtml
    );
};

class OutputGroup : public iotwebconf::ChainedParameterGroup {
public:
    OutputGroup(const char* id)
        : ChainedParameterGroup(id, "Output"),
            _DesignationParam("Designation", _DesignationId, _DesignationValue, STRING_LEN),
            _ModeParam("Mode", _ModeId, _ModeValue, sizeof(OutputModeValues[0]),
                (char*)OutputModeValues, (char*)OutputModeNames,
                sizeof(OutputModeValues) / sizeof(OutputModeValues[0]), sizeof(OutputModeNames[0]), nullptr, _ModeCustomHTML),
            _NumberParam("Outputs to use", _NumberId, _NumberValue, NUMBER_LEN, "1", "1..MAX_Outputs", "min='1' max='16' step='1'"),
            _AddressParam("DCC Address", _AddressId, _AddressValue, NUMBER_LEN, "3", "1..1024", "min='1' max='1024' step='1'"),
            _TimeOnParam("Time On (ms)", _TimeOnId, _TimeOnValue, NUMBER_LEN, "100", "1..65535", "min='1' max='65535' step='1'"),
            _TimeOffParam("Time Off (ms)", _TimeOffId, _TimeOffValue, NUMBER_LEN, "100", "1..65535", "min='1' max='65535' step='1'"),
            _MultiplierParam("Multiplier", _MultiplierId, _MultiplierValue, NUMBER_LEN, "10", "1..255", "min='0' max='255' step='1'"),
            _TimeOnFadeParam("Time On Fade (ms)", _TimeOnFadeId, _TimeOnFadeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'"),
            _TimeOffFadeParam("Time Off Fade (ms)", _TimeOffFadeId, _TimeOffFadeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'"),
            _BrightnessParam("Brightness", _BrightnessId, _BrightnessValue, NUMBER_LEN, "255", "0..255", "min='0' max='255' step='1'"),
            _activeDurationParam("Active Duration (ms)", _activeDurationId, _activeDurationValue, NUMBER_LEN, "0", "1..60000", "min='0' max='60000' step='1'"),
            _InputPinParam(
                "Input Pin",
                _InputPinId,
                _InputPinValue,
                sizeof(InputPinValues[0]),
                (char*)InputPinValues,
                (char*)InputPinNames,
                sizeof(InputPinValues) / sizeof(InputPinValues[0]),
                sizeof(InputPinNames[0]),
                "0",
                nullptr
            ),
            _EdgeTypeParam(
                "Edge Type",
                _EdgeTypeId,
                _EdgeTypeValue,
                sizeof(InputEdgeValues[0]),
                (char*)InputEdgeValues,
                (char*)InputEdgeNames,
                sizeof(InputEdgeValues) / sizeof(InputEdgeValues[0]),
                sizeof(InputEdgeNames[0]),
                "1", // Default: Falling
                nullptr
            )
    {
        snprintf(_DesignationId, STRING_LEN, "%s-designation", this->getId());
        snprintf(_ModeId, STRING_LEN, "%s-mode", this->getId());
        snprintf(_NumberId, STRING_LEN, "%s-number", this->getId());
        snprintf(_AddressId, STRING_LEN, "%s-address", this->getId());
        snprintf(_TimeOnId, STRING_LEN, "%s-timeon", this->getId());
        snprintf(_TimeOffId, STRING_LEN, "%s-timeoff", this->getId());
        snprintf(_MultiplierId, STRING_LEN, "%s-multiplier", this->getId());
        snprintf(_TimeOnFadeId, STRING_LEN, "%s-onfade", this->getId());
        snprintf(_TimeOffFadeId, STRING_LEN, "%s-offfade", this->getId());
        snprintf(_BrightnessId, STRING_LEN, "%s-brightness", this->getId());
        snprintf(_ModeCustomHTML, STRING_LEN, "onchange=\"hideClass('%s')\"", this->getId());
        snprintf(_activeDurationId, STRING_LEN, "%s-activeduration", this->getId());
        snprintf(_InputPinId, STRING_LEN, "%s-inputpin", this->getId());
        snprintf(_EdgeTypeId, STRING_LEN, "%s-edgetype", this->getId());

        this->addItem(&_DesignationParam);
        this->addItem(&_ModeParam);
        this->addItem(&_NumberParam);
        this->addItem(&_AddressParam);
        this->addItem(&_TimeOnParam);
        this->addItem(&_TimeOffParam);
        this->addItem(&_MultiplierParam);
        this->addItem(&_TimeOnFadeParam);
        this->addItem(&_TimeOffFadeParam);
        this->addItem(&_BrightnessParam);
        this->addItem(&_activeDurationParam);
        this->addItem(&_InputPinParam);
        this->addItem(&_EdgeTypeParam);
    }

    void applyDefaultValues() {
        _DesignationParam.applyDefaultValue();
        _ModeParam.applyDefaultValue();
        _NumberParam.applyDefaultValue();
        _AddressParam.applyDefaultValue();
        _TimeOnParam.applyDefaultValue();
        _TimeOffParam.applyDefaultValue();
        _MultiplierParam.applyDefaultValue();
        _TimeOnFadeParam.applyDefaultValue();
        _TimeOffFadeParam.applyDefaultValue();
        _BrightnessParam.applyDefaultValue();
        _activeDurationParam.applyDefaultValue();
        _InputPinParam.applyDefaultValue();
        _EdgeTypeParam.applyDefaultValue();
    }

    int getAddress() const { return atoi(_AddressValue); }
    const char* getDesignation() const { return _DesignationValue; }
    int getMode() const { return atoi(_ModeValue); }
    int getNumber(int8_t mode = -1) const {
        if (mode == -1) mode = getMode();
        switch (mode) {
        case 0: return 0;
        case 51: case 202: return 2;
        case 81: case 82: return 3;
        case 52: case 53: case 54: case 55: case 60: case 61: case 62:
            return atoi(_NumberValue);
        case 70:
            return 3;
        case 71: case 72:
            return 6;
        case 73:
            return 9;
        default: return 1;
        }
    }
    int getTimeOn() const { return atoi(_TimeOnValue); }
    int getTimeOff() const { return atoi(_TimeOffValue); }
    int getMultiplier() const { return atoi(_MultiplierValue); }
    int getTimeOnFade() const { return atoi(_TimeOnFadeValue); }
    int getTimeOffFade() const { return atoi(_TimeOffFadeValue); }
    int getBrightness() const { return atoi(_BrightnessValue); }
    int getActiveDuration() const { return atoi(_activeDurationValue); }
    int getInputPin() const { return atoi(_InputPinValue); }
    int getEdgeType() const { return atoi(_EdgeTypeValue); }

    // Zugriff auf NumberParam, falls benötigt
    iotwebconf::NumberParameter& numberParam() { return _NumberParam; }
    iotwebconf::NumberParameter& TimeOnFadeParam() { return _TimeOnFadeParam; }
    iotwebconf::NumberParameter& TimeOffFadeParam() { return _TimeOffFadeParam; }
	MySelectParameter& modeParam() { return _ModeParam; }


private:
    char _DesignationId[STRING_LEN];
    char _ModeId[STRING_LEN];
    char _NumberId[STRING_LEN];
    char _AddressId[STRING_LEN];
    char _TimeOnId[STRING_LEN];
    char _TimeOffId[STRING_LEN];
    char _MultiplierId[STRING_LEN];
    char _TimeOnFadeId[STRING_LEN];
    char _TimeOffFadeId[STRING_LEN];
    char _BrightnessId[STRING_LEN];
    char _ModeCustomHTML[STRING_LEN];
    char _activeDurationId[STRING_LEN];
    char _InputPinId[STRING_LEN];
    char _EdgeTypeId[STRING_LEN];

    char _DesignationValue[STRING_LEN]{};
    char _ModeValue[STRING_LEN]{};
    char _NumberValue[NUMBER_LEN]{};
    char _AddressValue[NUMBER_LEN]{};
    char _TimeOnValue[NUMBER_LEN]{};
    char _TimeOffValue[NUMBER_LEN]{};
    char _MultiplierValue[NUMBER_LEN]{};
    char _TimeOnFadeValue[NUMBER_LEN]{};
    char _TimeOffFadeValue[NUMBER_LEN]{};
    char _BrightnessValue[NUMBER_LEN]{"255"};
    char _activeDurationValue[NUMBER_LEN];
    char _InputPinValue[NUMBER_LEN];
    char _EdgeTypeValue[NUMBER_LEN];

    // Parameter-Objekte
    iotwebconf::TextParameter _DesignationParam;
    MySelectParameter _ModeParam;
    iotwebconf::NumberParameter _NumberParam;
    iotwebconf::NumberParameter _AddressParam;
    iotwebconf::NumberParameter _TimeOnParam;
    iotwebconf::NumberParameter _TimeOffParam;
    iotwebconf::NumberParameter _MultiplierParam;
    iotwebconf::NumberParameter _TimeOnFadeParam;
    iotwebconf::NumberParameter _TimeOffFadeParam;
    iotwebconf::NumberParameter _BrightnessParam;
    iotwebconf::NumberParameter _activeDurationParam;
    MySelectParameter _InputPinParam;
    MySelectParameter _EdgeTypeParam;

    String getEndTemplate() override {
        String result = "<script>hideClass('%s')</script>\n";
        result.replace("%s", this->getId());
        result += ChainedParameterGroup::getEndTemplate();
        return result;
    }
};


extern OutputGroup OutputGroup1;
extern OutputGroup OutputGroup2;
extern OutputGroup OutputGroup3;
extern OutputGroup OutputGroup4;
extern OutputGroup OutputGroup5;
extern OutputGroup OutputGroup6;
extern OutputGroup OutputGroup7;
extern OutputGroup OutputGroup8;
extern OutputGroup OutputGroup9;
extern OutputGroup OutputGroup10;
extern OutputGroup OutputGroup11;
extern OutputGroup OutputGroup12;
extern OutputGroup OutputGroup13;
extern OutputGroup OutputGroup14;
extern OutputGroup OutputGroup15;
extern OutputGroup OutputGroup16;


#endif

