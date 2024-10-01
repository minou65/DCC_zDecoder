#define DEBUG_WIFI(m) SERIAL_DBG.print(m)
//#define IOTWEBCONF_DEBUG_TO_SERIAL

#include <Arduino.h>
#include <ArduinoOTA.h>

#if ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>      
#endif

#include "webhandling.h"
#include "common.h"
#include "html.h"
#include "favicon.h"

#include <time.h>
#include <iostream>
#include <string.h>
#include <DNSServer.h>
#include <IotWebConfTParameter.h>
#include <IotWebConfAsyncClass.h>
#include <IotWebConfAsyncUpdateServer.h>
#include <IotWebRoot.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

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
void handleData(AsyncWebServerRequest* request);
void handleRoot(AsyncWebServerRequest* request);

// -- Callback methods.
void configSaved();
void wifiConnected();

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebServerWrapper asyncWebServerWrapper(&server);
AsyncUpdateServer AsyncUpdater;

IotWebConf iotWebConf(thingName, &dnsServer, &asyncWebServerWrapper, wifiInitialApPassword, CONFIG_VERSION);

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


void handleRoot(AsyncWebServerRequest* request) {
    AsyncWebRequestWrapper asyncWebRequestWrapper(request);
    if (iotWebConf.handleCaptivePortal(&asyncWebRequestWrapper)) {
        return;
    }

    std::string content_;
    MyHtmlRootFormatProvider fp_;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName()).c_str();

    AsyncWebServerResponse* response = request->beginChunkedResponse("text/html", [content_](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {

        std::string chunk_ = "";
        size_t len_ = min(content_.length() - index, maxLen);
        if (len_ > 0) {
            chunk_ = content_.substr(index, len_ - 1);
            chunk_.copy((char*)buffer, chunk_.length());
        }
        if (index + len_ <= content_.length())
            return chunk_.length();
        else
            return 0;

        });
    response->addHeader("Server", "ESP Async Web Server");
    request->send(response);
}

void websetup(){

    OutputGroup1.setNext(&OutputGroup2);
    OutputGroup2.setNext(&OutputGroup3);
    OutputGroup3.setNext(&OutputGroup4);
    OutputGroup4.setNext(&OutputGroup5);
    OutputGroup5.setNext(&OutputGroup6);
    OutputGroup6.setNext(&OutputGroup7);
    OutputGroup7.setNext(&OutputGroup8);
    //OutputGroup8.setNext(&OutputGroup9);
    //OutputGroup9.setNext(&OutputGroup10);

    ServoGroup1.setNext(&ServoGroup2);
    ServoGroup2.setNext(&ServoGroup3);
    ServoGroup3.setNext(&ServoGroup4);
    ServoGroup4.setNext(&ServoGroup5);

    iotWebConf.setStatusPin(STATUS_PIN);
    iotWebConf.setConfigPin(CONFIG_PIN);
    iotWebConf.setHtmlFormatProvider(&customHtmlFormatProvider);

    //iotWebConf.addParameterGroup(&ServoGroup1);
    //iotWebConf.addParameterGroup(&ServoGroup2);
    //iotWebConf.addParameterGroup(&ServoGroup3);
    //iotWebConf.addParameterGroup(&ServoGroup4);
    //iotWebConf.addParameterGroup(&ServoGroup5);

    iotWebConf.addParameterGroup(&OutputGroup1);
    iotWebConf.addParameterGroup(&OutputGroup2);
    iotWebConf.addParameterGroup(&OutputGroup3);
    iotWebConf.addParameterGroup(&OutputGroup4);
    iotWebConf.addParameterGroup(&OutputGroup5);
    iotWebConf.addParameterGroup(&OutputGroup6);
    iotWebConf.addParameterGroup(&OutputGroup7);
    iotWebConf.addParameterGroup(&OutputGroup8);
    //iotWebConf.addParameterGroup(&OutputGroup9);
    //iotWebConf.addParameterGroup(&OutputGroup10);

    iotWebConf.setupUpdateServer(
        [](const char* updatePath) { AsyncUpdater.setup(&server, updatePath); },
        [](const char* userName, char* password) { AsyncUpdater.updateCredentials(userName, password); });

    iotWebConf.setConfigSavedCallback(&configSaved);
    iotWebConf.setWifiConnectionCallback(&wifiConnected);

    iotWebConf.getApTimeoutParameter()->visible = true;

    // -- Initializing the configuration.
    iotWebConf.init();

    // -- Set up required URL handlers on the web server.
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { 
        handleRoot(request); 
        }
    );
    server.on("/config", HTTP_ANY, [](AsyncWebServerRequest* request) {
        AsyncWebRequestWrapper asyncWebRequestWrapper(request);
        iotWebConf.handleConfig(&asyncWebRequestWrapper);
        }
    );
    server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest* request) {
        AsyncWebServerResponse* response = request->beginResponse_P(200, "image/x-icon", favicon_ico, sizeof(favicon_ico));
        request->send(response);
        }
    );
    server.onNotFound([](AsyncWebServerRequest* request) {
        AsyncWebRequestWrapper asyncWebRequestWrapper(request);
        iotWebConf.handleNotFound(&asyncWebRequestWrapper);
        }
    );
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

