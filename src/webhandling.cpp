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
#include <IotWebConfESP32HTTPUpdateServer.h>
#include <IotWebRoot.h>
#include <time.h>
#include <DNSServer.h>
#include <iostream>
#include <string.h>
#include "common.h"
#include "html.h"

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "zDecoder";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "123456789";


// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "008"

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN

// -- Method declarations.
void handleRoot();
// -- Callback methods.
void configSaved();
void wifiConnected();

DNSServer dnsServer;
WebServer server(80);
HTTPUpdateServer httpUpdater;

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

class MyHtmlRootFormatProvider : public HtmlRootFormatProvider {
protected:
    virtual String getScriptInner() {
        String _s = HtmlRootFormatProvider::getScriptInner();
        _s.replace("{millisecond}", "1000");
        _s += F("function updateData(jsonData) {\n");
        _s += F("   document.getElementById('RSSIValue').innerHTML = jsonData.rssi + \"dBm\";\n");
        _s += F("   for (var key in jsonData) {\n");
        _s += F("       if (jsonData.hasOwnProperty(key) && (key.startsWith('output') || key.startsWith('servo'))) {\n");
        _s += F("           var button = document.getElementById(key);\n");
        _s += F("           if (button) {\n");
        _s += F("               button.style.backgroundColor = jsonData[key] == '1' ? 'green' : 'red';\n");
        _s += F("           }\n");
        _s += F("       }\n");
        _s += F("   }\n");
        _s += F("}\n");
        _s += html_css_code;
        return _s;
    }
};

void handleRoot() {
    if (iotWebConf.handleCaptivePortal()) {
        return;
    }

    std::string content_;
    MyHtmlRootFormatProvider fp_;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName()).c_str();
    content_ += fp_.getHtmlStyle().c_str();
    content_ += fp_.getHtmlHeadEnd().c_str();
    content_ += fp_.getHtmlScript().c_str();

    content_ += fp_.getHtmlTable().c_str();
    content_ += fp_.getHtmlTableRow().c_str();
    content_ += fp_.getHtmlTableCol().c_str();

    content_ += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    content_ += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();
    content_ += String(F("<tr><td align=\"left\"> </td></td><td align=\"right\"><span id=\"RSSIValue\">no data</span></td></tr>\n")).c_str();
    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlFieldsetEnd().c_str();

    content_ += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    content_ += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();

    uint8_t i_ = 1;

    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if ((outputgroup_->isActive()) && (atoi(outputgroup_->ModeValue) >= 10)) {
            String b_ = html_button_code;
            b_.replace("[value]", String(i_));
            b_.replace("[name]", String(outputgroup_->DesignationValue) + " (" + String(outputgroup_->ModeValue) + ")");
            b_.replace("[id]", "output" + String(i_));
            if (ChannelIsOn(i_ - 1)) {
                b_.replace("red", "green");
            }

            content_ += b_.c_str();
            content_ += fp_.addNewLine(2).c_str();
            i_ += 1;
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();
    }

    ServoGroup* servogroup_ = &ServoGroup1;
    while (servogroup_ != nullptr) {
        if (servogroup_->isActive()) {
            String b_ = html_button_code;
            b_.replace("[value]", String(i_));
            b_.replace("[name]", String(servogroup_->DesignationValue));
            b_.replace("[id]", "servo" + String(i_));
            if (ChannelIsOn(i_ - 1)) {
                b_.replace("red", "green");
            }

            content_ += b_.c_str();
            content_ += fp_.addNewLine(2).c_str();
            i_ += 1;
        }
        servogroup_ = (ServoGroup*)servogroup_->getNext();
    }

    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlFieldsetEnd().c_str();

    content_ += fp_.addNewLine(2).c_str();

    content_ += fp_.getHtmlTable().c_str();
    content_ += fp_.getHtmlTableRowText("<a href = 'config'>Configuration</a>").c_str();
    content_ += fp_.getHtmlTableRowText(fp_.getHtmlVersion(Version)).c_str();
    content_ += fp_.getHtmlTableEnd().c_str();

    content_ += fp_.getHtmlTableColEnd().c_str();
    content_ += fp_.getHtmlTableRowEnd().c_str();
    content_ += fp_.getHtmlTableEnd().c_str();
    content_ += fp_.getHtmlEnd().c_str();

    server.send(200, "text/html", content_.c_str());
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
    String _s = HTML_Start_Doc;
    _s = HTML_Style;
    _s.replace("center", "left");
    _s.replace("{v}", iotWebConf.getThingName());
    _s.replace("center", "left");

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

    iotWebConf.addParameterGroup(&ServoGroup1);
    iotWebConf.addParameterGroup(&ServoGroup2);
    iotWebConf.addParameterGroup(&ServoGroup3);
    iotWebConf.addParameterGroup(&ServoGroup4);
    iotWebConf.addParameterGroup(&ServoGroup5);

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

    // -- Define how to handle updateServer calls.
    iotWebConf.setupUpdateServer(
        [](const char* updatePath) { httpUpdater.setup(&server, updatePath); },
        [](const char* userName, char* password) { httpUpdater.updateCredentials(userName, password); });

    iotWebConf.setConfigSavedCallback(&configSaved);
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
