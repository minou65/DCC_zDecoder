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
#define CONFIG_VERSION "004"

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN GPIO_NUM_4

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

// -- We need to declare an instance for all ActionGroup items, that can
//    appear in the config portal
ActionGroup OutputGroup1 = ActionGroup("og1");
ActionGroup OutputGroup2 = ActionGroup("og2");
ActionGroup OutputGroup3 = ActionGroup("og3");
ActionGroup OutputGroup4 = ActionGroup("og4");
ActionGroup OutputGroup5 = ActionGroup("og5");
ActionGroup OutputGroup6 = ActionGroup("og6");
ActionGroup OutputGroup7 = ActionGroup("og7");
ActionGroup OutputGroup8 = ActionGroup("og8");
ActionGroup OutputGroup9 = ActionGroup("og9");
ActionGroup OutputGroup10 = ActionGroup("og10");

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);

// -- An instance must be created from the class defined above.
iotwebconf::OptionalGroupHtmlFormatProvider optionalGroupHtmlFormatProvider;

void handleRoot(){
    // -- Let IotWebConf test and handle captive portal requests.
    if (iotWebConf.handleCaptivePortal())
    {
        // -- Captive portal request were already served.
        return;
    }
    String s = F("<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>");
    s += F("<title>zDecoder</title></head><body><div>Status page of ");
    s += iotWebConf.getThingName();
    s += ".</div>";
    uint8_t _i = 1;
    uint8_t _count = 0;

    ActionGroup* group = &OutputGroup1;
    while (group != nullptr)
    {
        if (group->isActive()) {
            s += "<div>Output group " + String(_i) + "</div>";
            s += "<ul>";
            s += "<li>Designation: ";
            s += group->DesignationValue;
            s += "<li>Mode: ";
            s += group->ModeValue;
            s += "<li>Number of outputs: ";
            s += group->NumberValue;
            s += "<li>DCC Address: ";
            s += group->AddressValue;
            s += "</ul>";
            _count += atoi(group->NumberValue);
            _i += 1;
        }
        group = (ActionGroup*)group->getNext();
    }

    s += F("Total outputs used: ");
    s += String(_count);
    s += "<br><br>";
    s += F("Go to <a href='groups'>groups page</a> for enabling or disabling channels.");
    s += "<br>";
    s += F("Go to <a href='config'>configure page</a> to change values.");
    s += "</body></html>\n";

    server.send(200, "text/html", s);
}

#define ButtonResponse \
"<!DOCTYPE html>\
    <html><head>\
        <meta\
             http-equiv=\"refresh\"\
             content=\"1; url='/groups'\"/>\
    </head>\
    <body><p>Group enabled/disabled</p></body></html>\
"



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
    String _s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
    _s += "<title>zDecoder</title></head><body><div>Enable / Disable group for ";
    _s += iotWebConf.getThingName();
    _s += ".</div>";
    _s += "<form align=left action=\"/setgroup\" method=\"POST\">";

    uint8_t _i = 1;

    ActionGroup* _group = &OutputGroup1;
    while (_group != nullptr)
    {
        if (_group->isActive()) {
            String _b = "<button style=\"background-color:red;\" formaction=\"" + String(_i) + "\"type = \"submit\">Enable</button>";
            if (ChannelIsOn(_i - 1)) {
                _b.replace("red", "green");
                _b.replace("Enable", "Disable");
            }

            _s += _b;
            _s += _group->DesignationValue;
            _s += " (" + String(_group->ModeValue) + ")";
            _s += "<br>";
            _i += 1;
        }
        _group = (ActionGroup*)_group->getNext();
    }

    _s += "<br>";
    _s += "<br>";
    _s += F("Go to <a href='/'>main page</a>.");
    _s += "<br>";
    _s += F("Go to <a href='\config'>configure page</a> to change values.");

    _s += "</form>";
    server.send(200, "text/html", _s);
}

void websetup()
{
    Serial.begin(115200);
    Serial.println();
    Serial.println("Starting up...");

    OutputGroup1.setNext(&OutputGroup2);
    OutputGroup2.setNext(&OutputGroup3);
    OutputGroup3.setNext(&OutputGroup4);
    OutputGroup4.setNext(&OutputGroup5);
    OutputGroup5.setNext(&OutputGroup6);
    OutputGroup6.setNext(&OutputGroup7);
    OutputGroup7.setNext(&OutputGroup8);
    OutputGroup8.setNext(&OutputGroup9);
    OutputGroup9.setNext(&OutputGroup10);


    iotWebConf.setStatusPin(STATUS_PIN);
    iotWebConf.setConfigPin(CONFIG_PIN);
    iotWebConf.setHtmlFormatProvider(&optionalGroupHtmlFormatProvider);
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

    iotWebConf.setConfigSavedCallback(&configSaved);
    iotWebConf.setWifiConnectionCallback(&wifiConnected);

    iotWebConf.getApTimeoutParameter()->visible = true;

    // -- Initializing the configuration.
    iotWebConf.init();

    // -- Set up required URL handlers on the web server.
    server.on("/", handleRoot);
    server.on("/config", [] { iotWebConf.handleConfig(); });
    server.on("/Groups", handleGroups);
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

    Serial.println("Ready.");
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
