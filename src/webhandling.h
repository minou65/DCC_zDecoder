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
#include "common.h"

void websetup();
void webloop();

extern void handleChannel(uint8_t Channel_);
extern bool ChannelIsOn(uint8_t Channel_);

class ActionGroup : public iotwebconf::ChainedParameterGroup
{
public:
    ActionGroup(const char* id) : ChainedParameterGroup(id, "Ausgang")
    {
        // -- Update parameter Ids to have unique ID for all parameters within the application.
        snprintf(DescriptionId, STRING_LEN, "%s-desc", this->getId());
        snprintf(ModusId, STRING_LEN, "%s-mode", this->getId());
        snprintf(CountId, STRING_LEN, "%s-Count", this->getId());
        snprintf(AddressId, STRING_LEN, "%s-address", this->getId());
        snprintf(TimeOnId, STRING_LEN, "%s-timeon", this->getId());
        snprintf(TimeOffId, STRING_LEN, "%s-timeoff", this->getId());
        snprintf(TimeEinheitId, STRING_LEN, "%s-einheit", this->getId());
        snprintf(TimeOnFadeId, STRING_LEN, "%s-onfade", this->getId());
        snprintf(TimeOffFadeId, STRING_LEN, "%s-offfade", this->getId());

        // -- Add parameters to this group.
        this->addItem(&this->DescriptionParam);
        this->addItem(&this->ModusParam);
        this->addItem(&this->CountParam);
        this->addItem(&this->AddressParam);
        this->addItem(&this->TimeOnParam);
        this->addItem(&this->TimeOffParam);
        this->addItem(&this->TimeEinheitParam);
        this->addItem(&this->TimeOnFadeParam);
        this->addItem(&this->TimeOffFadeParam);
    }

    char DescriptionValue[STRING_LEN];
    char ModusValue[STRING_LEN];
    char CountValue[NUMBER_LEN];
    char AddressValue[NUMBER_LEN];
    char TimeOnValue[NUMBER_LEN];
    char TimeOffValue[NUMBER_LEN];
    char TimeEinheitValue[NUMBER_LEN];
    char TimeOnFadeValue[NUMBER_LEN];
    char TimeOffFadeValue[NUMBER_LEN];

    iotwebconf::TextParameter DescriptionParam =
        iotwebconf::TextParameter("Description", DescriptionId, DescriptionValue, STRING_LEN);
    iotwebconf::SelectParameter ModusParam =
        iotwebconf::SelectParameter("Modus", ModusId, ModusValue, STRING_LEN, (char*)DecoderModusValues, (char*)DecoderModusNames, sizeof(DecoderModusValues) / STRING_LEN, STRING_LEN);

    iotwebconf::NumberParameter CountParam =
        iotwebconf::NumberParameter("Anzahl Ports", CountId, CountValue, NUMBER_LEN, "1", "1..15", "min='1' max='15' step='1'");

    iotwebconf::NumberParameter AddressParam =
        iotwebconf::NumberParameter("Address", AddressId, AddressValue, NUMBER_LEN, "3", "1..1200", "min='1' max='1200' step='1'");

    iotwebconf::NumberParameter TimeOnParam =
        iotwebconf::NumberParameter("Blinker time on", TimeOnId, TimeOnValue, NUMBER_LEN, "1", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeOffParam =
        iotwebconf::NumberParameter("Blinker time off", TimeOffId, TimeOffValue, NUMBER_LEN, "1", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeEinheitParam =
        iotwebconf::NumberParameter("Einheit", TimeEinheitId, TimeEinheitValue, NUMBER_LEN, "10", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeOnFadeParam =
        iotwebconf::NumberParameter("Fade time on", TimeOnFadeId, TimeOnFadeValue, NUMBER_LEN, "1", "1..255", "min='1' max='255' step='1'");

    iotwebconf::NumberParameter TimeOffFadeParam =
        iotwebconf::NumberParameter("Fade time off", TimeOffFadeId, TimeOffFadeValue, NUMBER_LEN, "1", "1..255", "min='1' max='255' step='1'");

private:
    char DescriptionId[STRING_LEN];
    char ModusId[STRING_LEN];
    char CountId[STRING_LEN];
    char AddressId[STRING_LEN];
    char TimeOnId[STRING_LEN];
    char TimeOffId[STRING_LEN];
    char TimeEinheitId[STRING_LEN];
    char TimeOnFadeId[STRING_LEN];
    char TimeOffFadeId[STRING_LEN];
};


extern ActionGroup actionGroup1;
extern ActionGroup actionGroup2;
extern ActionGroup actionGroup3;
extern ActionGroup actionGroup4;
extern ActionGroup actionGroup5;
extern ActionGroup actionGroup6;
extern ActionGroup actionGroup7;
extern ActionGroup actionGroup8;
extern ActionGroup actionGroup9;
extern ActionGroup actionGroup10;

#endif

