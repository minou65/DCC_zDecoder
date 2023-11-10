#define DEBUG_WIFI(m) SERIAL_DBG.print(m)
#define IOTWEBCONF_DEBUG_TO_SERIAL

#include <Arduino.h>
#include <ArduinoOTA.h>

#if ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>      
#endif

#include "webhandling.h"
#include <IotWebConf.h>
#include <IotWebConfOptionalGroup.h>
#include <time.h>
#include <DNSServer.h>
#include <iostream>
#include <string.h>
#include "common.h"

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "testThing";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "123456789";


// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "007"

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

// -- Method declarations.
void handleRoot();
// -- Callback methods.
void configSaved();
void wifiConnected();
bool formValidator(iotwebconf::WebRequestWrapper* webRequestWrapper);

DNSServer dnsServer;
WebServer server(80);

// -- We need to declare an instance for all OutputGroup items, that can
//    appear in the config portal
OutputGroup OutputGroup1 = OutputGroup("og1");
OutputGroup OutputGroup2 = OutputGroup("og2");
OutputGroup OutputGroup3 = OutputGroup("og3");
OutputGroup OutputGroup4 = OutputGroup("og4");
OutputGroup OutputGroup5 = OutputGroup("og5");
OutputGroup OutputGroup6 = OutputGroup("og6");
OutputGroup OutputGroup7 = OutputGroup("og7");
OutputGroup OutputGroup8 = OutputGroup("og8");
OutputGroup OutputGroup9 = OutputGroup("og9");
OutputGroup OutputGroup10 = OutputGroup("og10");

ServoGroup ServoGroup1 = ServoGroup("sg1");
ServoGroup ServoGroup2 = ServoGroup("sg2");
ServoGroup ServoGroup3 = ServoGroup("sg3");
ServoGroup ServoGroup4 = ServoGroup("sg4");
ServoGroup ServoGroup5 = ServoGroup("sg5");
ServoGroup ServoGroup6 = ServoGroup("sg6");

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION); 

const char IOTWEBCONF_HTML_FORM_InputElements_JAVASCRIPT[] PROGMEM =
"function hideClass(id) {\n"
"   var dropdown = document.getElementById(id + '-mode');\n"
"   var selectedValue = dropdown.options[dropdown.selectedIndex].value;\n"
"   var numberClass = document.getElementsByClassName(id + '-number')[0];\n"
"   var timeonClass = document.getElementsByClassName(id + '-timeon')[0];\n"
"   var timeoffClass = document.getElementsByClassName(id + '-timeoff')[0];\n"
"   var multiplierClass = document.getElementsByClassName(id + '-multiplier')[0];\n"
"   var onfadeClass = document.getElementsByClassName(id + '-onfade')[0];\n"
"   var offfadeClass = document.getElementsByClassName(id + '-offfade')[0];\n"
"\n"
"   if (selectedValue === \"0\") {\n"
"		numberClass.style.display = \"block\";\n"
"		timeonClass.style.display = \"none\";\n"
"		timeoffClass.style.display = \"none\";\n"
"		multiplierClass.style.display = \"none\";\n"
"		onfadeClass.style.display = \"none\";\n"
"		offfadeClass.style.display = \"none\";\n"
"	} else if ((selectedValue === \"1\") ) {\n"
"		numberClass.style.display = \"none\";\n"
"		timeonClass.style.display = \"block\";\n"
"		timeoffClass.style.display = \"none\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"none\";\n"
"		offfadeClass.style.display = \"none\";	"
"	} else if ((selectedValue === \"40\") || (selectedValue === \"80\") || (selectedValue === \"82\")) {\n"
"		numberClass.style.display = \"none\";\n"
"		timeonClass.style.display = \"none\";\n"
"		timeoffClass.style.display = \"none\";\n"
"		multiplierClass.style.display = \"none\";\n"
"		onfadeClass.style.display = \"none\";\n"
"		offfadeClass.style.display = \"none\";	"
"	} else if ((selectedValue === \"50\") || (selectedValue === \"51\")) {"
"		numberClass.style.display = \"none\";\n"
"		timeonClass.style.display = \"block\";\n"
"		timeoffClass.style.display = \"block\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"block\";\n"
"		offfadeClass.style.display = \"block\";\n"
"	} else if ((selectedValue === \"52\") || (selectedValue === \"53\") || (selectedValue === \"54\") || (selectedValue === \"55\")) {\n"
"		numberClass.style.display = \"block\";\n"
"		timeonClass.style.display = \"block\";\n"
"		timeoffClass.style.display = \"block\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"none\";\n"
"		offfadeClass.style.display = \"none\";\n"
"\n"
"	} else if ((selectedValue === \"60\") || (selectedValue === \"61\")) {\n"
"		numberClass.style.display = \"block\";\n"
"		timeonClass.style.display = \"block\";\n"
"		timeoffClass.style.display = \"block\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"none\";\n"
"		offfadeClass.style.display = \"none\";\n"
"\n"
"	} else if ((selectedValue === \"62\")) {\n"
"		numberClass.style.display = \"block\";\n"
"		timeonClass.style.display = \"none\";\n"
"		timeoffClass.style.display = \"none\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"block\";\n"
"		offfadeClass.style.display = \"block\";	"
"\n"
"	} else if ((selectedValue === \"81\")) {\n"
"		numberClass.style.display = \"none\";\n"
"		timeonClass.style.display = \"block\";\n"
"		timeoffClass.style.display = \"block\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"none\";\n"
"		offfadeClass.style.display = \"none\";\n"
"\n"
"	} else {\n"
"		numberClass.style.display = \"block\";\n"
"		timeonClass.style.display = \"block\";\n"
"		timeoffClass.style.display = \"block\";\n"
"		multiplierClass.style.display = \"block\";\n"
"		onfadeClass.style.display = \"block\";\n"
"		offfadeClass.style.display = \"block\";\n"
"	}\n"
"}\n";

MySelectParameter::MySelectParameter(
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
    ) : iotwebconf::SelectParameter(label, id, valueBuffer, length, optionValues, optionNames, optionCount, nameLength, defaultValue) {
        this->customHtml = customHtml;
};


class CustomHtmlFormatProvider : public iotwebconf::OptionalGroupHtmlFormatProvider {
protected:
    String getScriptInner() override {
        return
            HtmlFormatProvider::getScriptInner() + 
            String(FPSTR(IOTWEBCONF_HTML_FORM_OPTIONAL_GROUP_JAVASCRIPT)) +
            String(FPSTR(IOTWEBCONF_HTML_FORM_InputElements_JAVASCRIPT));
    }
};

CustomHtmlFormatProvider customHtmlFormatProvider;

// -- An instance must be created from the class defined above.
// iotwebconf::OptionalGroupHtmlFormatProvider optionalGroupHtmlFormatProvider;



void handleRoot(){
    // -- Let IotWebConf test and handle captive portal requests.
    if (iotWebConf.handleCaptivePortal())
    {
        // -- Captive portal request were already served.
        return;
    }
    //String s = F("<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>");
    String s = HTML_Start_Doc;
    s += HTML_Style;
    s.replace("center", "left");
    s.replace("{v}", iotWebConf.getThingName());

    uint8_t _i = 1;
    uint8_t _count = 0;

    s += HTML_Start_Body;

    s += "<h2>Overview for " + String(iotWebConf.getThingName()) + "</h2>";

    OutputGroup* _outputgroup = &OutputGroup1;
    while (_outputgroup != nullptr)
    {
        if (_outputgroup->isActive()) {
            s += "<div>Output group " + String(_i) + "</div>";
            s += "<ul>";
            if (_outputgroup->ModeValue != 0) {
                s += "<li>Designation: " + String(_outputgroup->DesignationValue) + "</li>";
            }
            else {
                s += "<li>Designation:  - </li>";
            }

            s += "<li>Mode: " + String(_outputgroup->ModeValue) + "</li>";
            s += "<li>Number of outputs: " + String(_outputgroup->NumberValue) + "</li>";
            if (_outputgroup->ModeValue != 0) {
                s += "<li>DCC Address: " + String(_outputgroup->AddressValue) + +"</li>";
            }
            
            s += "</ul>";
            _count += atoi(_outputgroup->NumberValue);
            _i += 1;
        }
        else {
            _outputgroup->DesignationParam.applyDefaultValue();
            _outputgroup->ModeParam.applyDefaultValue();
            _outputgroup->NumberParam.applyDefaultValue();
            _outputgroup->AddressParam.applyDefaultValue();
            _outputgroup->TimeOnParam.applyDefaultValue();
            _outputgroup->TimeOffParam.applyDefaultValue();
            _outputgroup->TimeOnFadeParam.applyDefaultValue();
            _outputgroup->TimeOffFadeParam.applyDefaultValue();
            
        }
        _outputgroup = (OutputGroup*)_outputgroup->getNext();
    }

    ServoGroup* _servogroup = &ServoGroup1;
    while (_servogroup != nullptr)
    {
        if (_servogroup->isActive()) {
            s += "<div>Servo group " + String(_i) + "</div>";
            s += "<ul>";

            s += "<li>DCC Address: " + String(_servogroup->AddressValue) + +"</li>";
            s += "<li>Travel time: " + String(_servogroup->TravelTimeValue) + +"</li>";
            s += "<li>Multiplier:  " + String(_servogroup->MultiplierValue) + +"</li>";
            s += "<li>Limit 1:     " + String(_servogroup->Limit1Value) + +"</li>";
            s += "<li>Limit 2:     " + String(_servogroup->Limit2Value) + +"</li>";

            s += "</ul>";
            _count += 1;
            _i += 1;
        }
        else {
            _servogroup->DesignationParam.applyDefaultValue();
            _servogroup->AddressParam.applyDefaultValue();
            _servogroup->TravelTimeParam.applyDefaultValue();
            _servogroup->MultiplierParam.applyDefaultValue();
            _servogroup->Limit1Param.applyDefaultValue();
            _servogroup->Limit2Param.applyDefaultValue();

        }
        _servogroup = (ServoGroup*)_servogroup->getNext();
    }
    s += F("Total outputs used: ");
    s += String(_count);
    s += "<br><br>";
    s += F("Go to <a href='groups'>groups page</a> for enabling or disabling channels.");
    s += "<br>";
    s += F("Go to <a href='config'>configure page</a> to change values.");
    s += "<br>";
    s += "<font size = 1>Version: " + String(Version) + "</font>";

    s += HTML_End_Body;
    s += HTML_End_Doc;

    server.send(200, "text/html", s);
}

void handle1() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(0);
}

void handle2() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(1);
}

void handle3() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(2);
}

void handle4() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(3);
}

void handle5() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(4);
}

void handle6() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(5);
}

void handle7() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(6);
}

void handle8() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(7);
}

void handle9() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(8);
}

void handle10() {
    server.send(200, "text/html", ButtonResponse);
    handleChannel(9);
}

void handleGroups() {
    // String _s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
    String _s = HTML_Style;
    _s.replace("center", "left");
    //_s += "<title>zDecoder</title></head><body><div>Enable / Disable group for ";
    //_s += iotWebConf.getThingName();
    //_s += ".</div>";

    _s += HTML_Start_Body;
    _s += "<table border=0 align=center>";
    _s += "<tr><td>";

    _s += HTML_Start_Fieldset;
    _s += HTML_Fieldset_Legend;
    _s.replace("{l}", String(iotWebConf.getThingName()) + " groups");

    _s += HTML_Start_Table;

    _s += "<form align=left action=\"/setgroup\" method=\"POST\">";

    uint8_t _i = 1;

    OutputGroup* _outputgroup = &OutputGroup1;
    while (_outputgroup != nullptr){
        if ((_outputgroup->isActive()) && (atoi(_outputgroup->ModeValue) >= 10)) {
            String _b = "<button style=\"background-color:red;\" formaction=\"" + String(_i) + "\"type = \"submit\">[Text]</button>";
            if (ChannelIsOn(_i - 1)) {
                _b.replace("red", "green");
            }
            String _bText = String(_outputgroup->DesignationValue) + " (" + String(_outputgroup->ModeValue) + ")";
            _b.replace("[Text]", _bText);

            _s += _b;
            _s += "<br><br>";
            _i += 1;
        }
        _outputgroup = (OutputGroup*)_outputgroup->getNext();
    }

    ServoGroup* _servogroup = &ServoGroup1;
    while (_servogroup != nullptr) {
        if (_servogroup->isActive()) {
            String _b = "<button style=\"background-color:red;\" formaction=\"" + String(_i) + "\"type = \"submit\">[Text]</button>";
            if (ChannelIsOn(_i - 1)) {
                _b.replace("red", "green");
            }
            String _bText = String(_servogroup->DesignationValue);
            _b.replace("[Text]", _bText);

            _s += _b;
            _s += "<br><br>";
            _i += 1;
        }
        _servogroup = (ServoGroup*)_servogroup->getNext();
    }

    _s += "</form>";

    _s += HTML_End_Table;
    _s += HTML_End_Fieldset;

    _s += "</td></tr>";
    _s += HTML_End_Table;

    _s += "<br>";
    _s += "<br>";

    _s += HTML_Start_Table;
    _s += "<tr><td align=left>Go to <a href = 'config'>configure page</a> to change configuration.</td></tr>";
    _s += "<tr><td align=left>Go to <a href='/'>main page</a> to change runtime data.</td></tr>";
    _s += "<tr><td><font size=1>Version: " + String(Version) + "</font></td></tr>";
    _s += HTML_End_Table;
    _s += HTML_End_Body;

    _s += HTML_End_Doc;


    server.send(200, "text/html", _s);
}

void websetup(){

    OutputGroup1.setNext(&OutputGroup2);
    OutputGroup2.setNext(&OutputGroup3);
    OutputGroup3.setNext(&OutputGroup4);
    OutputGroup4.setNext(&OutputGroup5);
    OutputGroup5.setNext(&OutputGroup6);
    OutputGroup6.setNext(&OutputGroup7);
    OutputGroup7.setNext(&OutputGroup8);
    OutputGroup8.setNext(&OutputGroup9);
    OutputGroup9.setNext(&OutputGroup10);

    ServoGroup1.setNext(&ServoGroup2);
    ServoGroup2.setNext(&ServoGroup3);
    ServoGroup3.setNext(&ServoGroup4);
    ServoGroup4.setNext(&ServoGroup5);
    ServoGroup5.setNext(&ServoGroup6);


    iotWebConf.setStatusPin(STATUS_PIN);
    iotWebConf.setConfigPin(CONFIG_PIN);
    iotWebConf.setHtmlFormatProvider(&customHtmlFormatProvider);
    // iotWebConf.setHtmlFormatProvider(&optionalGroupHtmlFormatProvider);
    // We also need to add all these parameter groups to iotWebConf
    iotWebConf.addParameterGroup(&OutputGroup1);
    iotWebConf.addParameterGroup(&OutputGroup2);
    iotWebConf.addParameterGroup(&OutputGroup3);
    iotWebConf.addParameterGroup(&OutputGroup4);
    iotWebConf.addParameterGroup(&OutputGroup5);
    iotWebConf.addParameterGroup(&OutputGroup6);
    iotWebConf.addParameterGroup(&OutputGroup7);
    iotWebConf.addParameterGroup(&OutputGroup8);
    iotWebConf.addParameterGroup(&OutputGroup9);
    iotWebConf.addParameterGroup(&OutputGroup10);

    iotWebConf.addParameterGroup(&ServoGroup1);
    iotWebConf.addParameterGroup(&ServoGroup2);
    iotWebConf.addParameterGroup(&ServoGroup3);
    iotWebConf.addParameterGroup(&ServoGroup4);
    iotWebConf.addParameterGroup(&ServoGroup5);

    iotWebConf.setConfigSavedCallback(&configSaved);
    iotWebConf.setFormValidator(&formValidator);
    iotWebConf.setWifiConnectionCallback(&wifiConnected);

    iotWebConf.getApTimeoutParameter()->visible = true;

    // -- Initializing the configuration.
    iotWebConf.init();

    // -- Set up required URL handlers on the web server.
    server.on("/", handleRoot);
    server.on("/config", [] { iotWebConf.handleConfig(); });
    server.on("/groups", handleGroups);
    server.on("/1", HTTP_POST, handle1);
    server.on("/2", HTTP_POST, handle2);
    server.on("/3", HTTP_POST, handle3);
    server.on("/4", HTTP_POST, handle4);
    server.on("/5", HTTP_POST, handle5);
    server.on("/6", HTTP_POST, handle6);
    server.on("/7", HTTP_POST, handle7);
    server.on("/8", HTTP_POST, handle8);
    server.on("/9", HTTP_POST, handle9);
    server.on("/10", HTTP_POST, handle10);
    server.onNotFound([]() { iotWebConf.handleNotFound(); });
}

void webloop(){
    iotWebConf.doLoop();
    ArduinoOTA.handle();
}

void wifiConnected() {
    ArduinoOTA.begin();

}

void configSaved(){
    ResetDCCDecoder = true;
    Serial.println(F("Configuration was updated."));
}

bool formValidator(iotwebconf::WebRequestWrapper* webRequestWrapper){
    return true;
}
