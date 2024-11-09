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

void websetup();
void webloop();

extern void handleChannel(uint8_t Channel_);
extern bool ChannelIsOn(uint8_t Channel_);

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

class ServoGroup : public iotwebconf::ChainedParameterGroup {
public:
    ServoGroup(const char* id) : ChainedParameterGroup(id, "Servo") {
        // -- Update parameter Ids to have unique ID for all parameters within the application.
        snprintf(DesignationId, STRING_LEN, "%s-designation", this->getId());
        snprintf(AddressId, STRING_LEN, "%s-address", this->getId());
        snprintf(TravelTimeId, STRING_LEN, "%s-traveltime", this->getId());
        snprintf(MultiplierId, STRING_LEN, "%s-multiplier", this->getId());
        snprintf(Limit1Id, STRING_LEN, "%s-limit1", this->getId());
        snprintf(Limit2Id, STRING_LEN, "%s-limit2", this->getId());
        snprintf(ModeCustomHTML, STRING_LEN, "onchange=\"hideClass('%s')\"", this->getId());

        // -- Add parameters to this group.
        this->addItem(&this->DesignationParam);
        this->addItem(&this->AddressParam);
        this->addItem(&this->TravelTimeParam);
        this->addItem(&this->MultiplierParam);
        this->addItem(&this->Limit1Param);
        this->addItem(&this->Limit2Param);
    }

    char DesignationValue[STRING_LEN];
    char AddressValue[NUMBER_LEN];
    char TravelTimeValue[NUMBER_LEN];
    char MultiplierValue[NUMBER_LEN];
    char Limit1Value[NUMBER_LEN];
    char Limit2Value[NUMBER_LEN];

    iotwebconf::TextParameter DesignationParam =
        iotwebconf::TextParameter("Designation", DesignationId, DesignationValue, STRING_LEN);

    iotwebconf::NumberParameter AddressParam =
        iotwebconf::NumberParameter("DCC Address", AddressId, AddressValue, NUMBER_LEN, "3", "1..1024", "min='1' max='1024' step='1'");

    iotwebconf::NumberParameter TravelTimeParam =
        iotwebconf::NumberParameter("Travel time (ms)", TravelTimeId, TravelTimeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter MultiplierParam =
        iotwebconf::NumberParameter("Multiplier", MultiplierId, MultiplierValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter Limit1Param =
        iotwebconf::NumberParameter("Limit 1", Limit1Id, Limit1Value, NUMBER_LEN, "0", "0..180", "min='0' max='180' step='1'");

    iotwebconf::NumberParameter Limit2Param =
        iotwebconf::NumberParameter("Limit 2", Limit2Id, Limit2Value, NUMBER_LEN, "180", "1..180", "min='0' max='180' step='1'");

private:
    char DesignationId[STRING_LEN];
    char AddressId[STRING_LEN];
    char TravelTimeId[STRING_LEN];
    char MultiplierId[STRING_LEN];
    char Limit1Id[STRING_LEN];
    char Limit2Id[STRING_LEN];
    char ModeCustomHTML[STRING_LEN];

    String getEndTemplate() override {

        String result = "<script>hideClass('%s')</script>\n";
        result.replace("%s", this->getId());

        result += ChainedParameterGroup::getEndTemplate();

        return result;

    };
};

class OutputGroup : public iotwebconf::ChainedParameterGroup {
public:
    OutputGroup(const char* id) : ChainedParameterGroup(id, "Output") {
        // -- Update parameter Ids to have unique ID for all parameters within the application.
        snprintf(DesignationId, STRING_LEN, "%s-designation", this->getId());
        snprintf(ModeId, STRING_LEN, "%s-mode", this->getId());
        snprintf(NumberId, STRING_LEN, "%s-number", this->getId());
        snprintf(AddressId, STRING_LEN, "%s-address", this->getId());
        snprintf(TimeOnId, STRING_LEN, "%s-timeon", this->getId());
        snprintf(TimeOffId, STRING_LEN, "%s-timeoff", this->getId());
        snprintf(MultiplierId, STRING_LEN, "%s-multiplier", this->getId());
        snprintf(TimeOnFadeId, STRING_LEN, "%s-onfade", this->getId());
        snprintf(TimeOffFadeId, STRING_LEN, "%s-offfade", this->getId());
        snprintf(BrightnessId, STRING_LEN, "%s-brightness", this->getId());
        snprintf(ModeCustomHTML, STRING_LEN, "onchange=\"hideClass('%s')\"", this->getId());

        // -- Add parameters to this group.
        this->addItem(&this->DesignationParam);
        this->addItem(&this->ModeParam);
        this->addItem(&this->NumberParam);
        this->addItem(&this->AddressParam);
        this->addItem(&this->TimeOnParam);
        this->addItem(&this->TimeOffParam);
        this->addItem(&this->MultiplierParam);
        this->addItem(&this->TimeOnFadeParam);
        this->addItem(&this->TimeOffFadeParam);
        this->addItem(&this->BrightnessParam);
    }

    char DesignationValue[STRING_LEN];
    char ModeValue[STRING_LEN];
    char NumberValue[NUMBER_LEN];
    char AddressValue[NUMBER_LEN];
    char TimeOnValue[NUMBER_LEN];
    char TimeOffValue[NUMBER_LEN];
    char MultiplierValue[NUMBER_LEN];
    char TimeOnFadeValue[NUMBER_LEN];
    char TimeOffFadeValue[NUMBER_LEN];
    char BrightnessValue[NUMBER_LEN] = "255"; // Default value for brightness

    iotwebconf::TextParameter DesignationParam =
        iotwebconf::TextParameter("Designation", DesignationId, DesignationValue, STRING_LEN);

    MySelectParameter ModeParam =
        MySelectParameter(
            "Mode",
            ModeId,
            ModeValue,
            STRING_LEN,
            (char*)DecoderModeValues,
            (char*)DecoderModeNames,
            sizeof(DecoderModeValues) / STRING_LEN,
            STRING_LEN,
            nullptr,
            ModeCustomHTML
        );

    iotwebconf::NumberParameter NumberParam =
        iotwebconf::NumberParameter("Number of outputs", NumberId, NumberValue, NUMBER_LEN, "1", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter AddressParam =
        iotwebconf::NumberParameter("DCC Address", AddressId, AddressValue, NUMBER_LEN, "3", "1..1024", "min='1' max='1024' step='1'");

    iotwebconf::NumberParameter TimeOnParam =
        iotwebconf::NumberParameter("Time On (ms)", TimeOnId, TimeOnValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeOffParam =
        iotwebconf::NumberParameter("Time Off (ms)", TimeOffId, TimeOffValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter MultiplierParam =
        iotwebconf::NumberParameter("Multiplier", MultiplierId, MultiplierValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeOnFadeParam =
        iotwebconf::NumberParameter("Time On Fade (ms)", TimeOnFadeId, TimeOnFadeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeOffFadeParam =
        iotwebconf::NumberParameter("Time Off Fade (ms)", TimeOffFadeId, TimeOffFadeValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter BrightnessParam =
        iotwebconf::NumberParameter("Brightness", BrightnessId, BrightnessValue, NUMBER_LEN, "255", "0..255", "min='0' max='255' step='1'");

private:
    char DesignationId[STRING_LEN];
    char ModeId[STRING_LEN];
    char NumberId[STRING_LEN];
    char AddressId[STRING_LEN];
    char TimeOnId[STRING_LEN];
    char TimeOffId[STRING_LEN];
    char MultiplierId[STRING_LEN];
    char TimeOnFadeId[STRING_LEN];
    char TimeOffFadeId[STRING_LEN];
    char BrightnessId[STRING_LEN];
    char ModeCustomHTML[STRING_LEN];

    String getEndTemplate() override {
        String result = "<script>hideClass('%s')</script>\n";
        result.replace("%s", this->getId());
        result += ChainedParameterGroup::getEndTemplate();
        return result;
    };
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

extern ServoGroup ServoGroup1;
extern ServoGroup ServoGroup2;
extern ServoGroup ServoGroup3;
extern ServoGroup ServoGroup4;
extern ServoGroup ServoGroup5;
extern ServoGroup ServoGroup6;

#endif

