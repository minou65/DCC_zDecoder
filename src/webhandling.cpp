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
#include <DNSServer.h>
#include <IotWebConf.h>
#include <IotWebRoot.h>
#include <IotWebConfAsyncClass.h>
#include <IotWebConfAsyncUpdateServer.h>
#include <IotWebConfOptionalGroup.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <iostream>
#include <string.h>
#include "version.h"

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
void handleRoot(AsyncWebServerRequest* request);
void handlePost(AsyncWebServerRequest* request);
void handleData(AsyncWebServerRequest* request);
// -- Callback methods.
void configSaved();
void wifiConnected();

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebServerWrapper asyncWebServerWrapper(&server);
AsyncUpdateServer AsyncUpdater;

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

IotWebConf iotWebConf(thingName, &dnsServer, &asyncWebServerWrapper, wifiInitialApPassword, CONFIG_VERSION);

const char ButtonResponse[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<meta http-equiv="refresh" content="1; url='/'"/>
	</head>
	<body><p>processing group</p></body>
</html>
)=====";

const char html_button_code[] PROGMEM = R"=====(
const char html_button_code[] PROGMEM = R"=====(
<form action="/post" method="post">
    <input type="hidden" name="group" value="[value]">
    <button id="[id]" style="background-color: red;" type="submit">[name]</button>
</form>)=====";


const char html_css_code[] PROGMEM = R"=====(
<style>
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
}

.switch input { 
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: #2196F3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196F3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(26px);
  -ms-transform: translateX(26px);
  transform: translateX(26px);
}

/* Rounded sliders */
.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}
</style>
)=====";

const char IOTWEBCONF_HTML_FORM_InputElements_JAVASCRIPT[] PROGMEM = R"=====(
function hideClass(id) {
    var dropdown = document.getElementById(id + '-mode');
    var selectedValue = dropdown.options[dropdown.selectedIndex].value;
    var numberClass = document.getElementsByClassName(id + '-number')[0];
    var timeonClass = document.getElementsByClassName(id + '-timeon')[0];
    var timeoffClass = document.getElementsByClassName(id + '-timeoff')[0];
    var multiplierClass = document.getElementsByClassName(id + '-multiplier')[0];
    var onfadeClass = document.getElementsByClassName(id + '-onfade')[0];
    var offfadeClass = document.getElementsByClassName(id + '-offfade')[0];

    // none is not visible, block is visible
    // The array parameters contains a list of items for which numberclass should be visible
    var parameters = ["52", "53", "54", "55", "60", "61", "62"];
    numberClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which timeonClass should be visible
    parameters = ["1", "50", "51", "52", "53", "54", "55", "60", "81", "83", "201", "202"];
    timeonClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which timeoffClass should be visible
    parameters = ["50", "51", "52", "53", "54", "55", "60", "83"];
    timeoffClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which multiplierClass should be visible
    parameters = ["1", "50", "51", "52", "53", "54", "55", "60", "61", "62", "81", "83", "201", "202"];
    multiplierClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which onfadeClass should be visible
    parameters = ["50", "51", "62", "83", "102", "103", "104", "105", "106", "110", "70"];
    onfadeClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    // The array parameters contains a list of items for which offfadeClass should be visible
    parameters = ["50", "51", "62", "83", "102", "103", "104", "105", "106", "110", "70"];
    offfadeClass.style.display = parameters.includes(selectedValue) ? "block" : "none";

    document.querySelector('label[for="' + id + '-timeon"]').innerHTML = 'Time on (ms)';
    document.querySelector('label[for="' + id + '-timeoff"]').innerHTML = 'Time off (ms)';
    document.querySelector('label[for="' + id + '-onfade"]').innerHTML = 'Fader on (ms)';
    document.querySelector('label[for="' + id + '-offfade"]').innerHTML = 'Fader off (ms)';
    document.querySelector('label[for="' + id + '-multiplier"]').innerHTML = 'Multiplier';

    parameters = ["83"];
    if (parameters.includes(selectedValue)) {
        document.querySelector('label[for="' + id + '-timeon"]').innerHTML = 'Minimum flash time (ms)';
        document.querySelector('label[for="' + id + '-timeoff"]').innerHTML = 'Maximal flash time (ms)';
        document.querySelector('label[for="' + id + '-onfade"]').innerHTML = 'Minimum pause time (s)';
        document.querySelector('label[for="' + id + '-offfade"]').innerHTML = 'Maximal pause time (s)';
    }

    parameters = ["61", "62"];
    if (parameters.includes(selectedValue)) {
        document.querySelector('label[for="' + id + '-multiplier"]').innerHTML = 'Chance';
    }

    parameters = ["62"];
    if (parameters.includes(selectedValue)) {
        document.querySelector('label[for="' + id + '-onfade"]').innerHTML = 'minimal glowing time (ms)';
        document.querySelector('label[for="' + id + '-offfade"]').innerHTML = 'maximal glowing time (ms)';
    }
}
)=====";

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

void handleRoot(AsyncWebServerRequest* _request) {

    AsyncWebRequestWrapper* _asyncWebRequestWrapper = new AsyncWebRequestWrapper(_request);
    if (iotWebConf.handleCaptivePortal(_asyncWebRequestWrapper)) {
        return;
    }

    std::string _content;
    MyHtmlRootFormatProvider _fp;


    _content += _fp.getHtmlHead(iotWebConf.getThingName()).c_str();
    _content += _fp.getHtmlStyle().c_str();
    _content += _fp.getHtmlHeadEnd().c_str();
    _content += _fp.getHtmlScript().c_str();

    _content += _fp.getHtmlTable().c_str();
    _content += _fp.getHtmlTableRow().c_str();
    _content += _fp.getHtmlTableCol().c_str();

    _content += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    _content += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();
    _content += String(F("<tr><td align=\"left\"> </td></td><td align=\"right\"><span id=\"RSSIValue\">no data</span></td></tr>\n")).c_str();
    _content += _fp.getHtmlTableEnd().c_str();
    _content += _fp.getHtmlFieldsetEnd().c_str();

    _content += String(F("<fieldset align=left style=\"border: 1px solid\">\n")).c_str();
    _content += String(F("<table border=\"0\" align=\"center\" width=\"100%\">\n")).c_str();

    uint8_t _i = 1;

    OutputGroup* _outputgroup = &OutputGroup1;
    while (_outputgroup != nullptr) {
        if ((_outputgroup->isActive()) && (atoi(_outputgroup->ModeValue) >= 10)) {
            String _b = html_button_code;
            _b.replace("[value]", String(_i));
            _b.replace("[name]", String(_outputgroup->DesignationValue) + " (" + String(_outputgroup->ModeValue) + ")");
            _b.replace("[id]", "output" + String(_i));
            if (ChannelIsOn(_i - 1)) {
                _b.replace("red", "green");
            }

            _content += _b.c_str();
            _content += _fp.addNewLine(2).c_str();
            _i += 1;
        }
        _outputgroup = (OutputGroup*)_outputgroup->getNext();
    }

    ServoGroup* _servogroup = &ServoGroup1;
    while (_servogroup != nullptr) {
        if (_servogroup->isActive()) {
            String _b = html_button_code;
            _b.replace("[value]", String(_i));
            _b.replace("[name]", String(_servogroup->DesignationValue));
            _b.replace("[id]", "servo" + String(_i));
            if (ChannelIsOn(_i - 1)) {
                _b.replace("red", "green");
            }

            _content += _b.c_str();
            _content += _fp.addNewLine(2).c_str();
            _i += 1;

        }
        _servogroup = (ServoGroup*)_servogroup->getNext();
    }

    _content += _fp.getHtmlTableEnd().c_str();
    _content += _fp.getHtmlFieldsetEnd().c_str();



    _content += _fp.addNewLine(2).c_str();

    _content += _fp.getHtmlTable().c_str();
    _content += _fp.getHtmlTableRowText("<a href = 'config'>Configuration</a>").c_str();
    _content += _fp.getHtmlTableRowText(_fp.getHtmlVersion(Version)).c_str();
    _content += _fp.getHtmlTableEnd().c_str();

    _content += _fp.getHtmlTableColEnd().c_str();
    _content += _fp.getHtmlTableRowEnd().c_str();
    _content += _fp.getHtmlTableEnd().c_str();
    _content += _fp.getHtmlEnd().c_str();

    AsyncWebServerResponse* _response = _request->beginChunkedResponse("text/html", [_content](uint8_t* _buffer, size_t _maxLen, size_t _index) -> size_t {

        std::string _chunk = "";
        size_t _len = min(_content.length() - _index, _maxLen);
        if (_len > 0) {
            _chunk = _content.substr(_index, _len - 1);
            _chunk.copy((char*)_buffer, _chunk.length());
        }
        if (_index + _len <= _content.length())
            return _chunk.length();
        else
            return 0;

        }
    );
    _response->addHeader("Server", "ESP Async Web Server");
    _request->send(_response);

}

void handleSettings(AsyncWebServerRequest* request){

    std::string _content;
    MyHtmlRootFormatProvider _fp;
    uint8_t _i = 1;
    uint8_t _count = 0;

    _content += _fp.getHtmlHead(iotWebConf.getThingName()).c_str();
    _content += _fp.getHtmlStyle().c_str();
    _content += _fp.getHtmlHeadEnd().c_str();
    _content += _fp.getHtmlScript().c_str();

    _content += _fp.getHtmlTable().c_str();
    _content += _fp.getHtmlTableRow().c_str();
    _content += _fp.getHtmlTableCol().c_str();

    // _content += "<h2>Overview for " + String(iotWebConf.getThingName()).c_str() + "</h2>";

    OutputGroup* _outputgroup = &OutputGroup1;
    while (_outputgroup != nullptr) {
        if (_outputgroup->isActive()) {
            _content += String("<div>Output group " + String(_outputgroup->DesignationValue) + "</div>").c_str();
            _content += "<ul>";
            _content += String("<li>Mode: " + String(_outputgroup->ModeValue) + "</li>").c_str();
            _content += String("<li>Number of outputs: " + String(_outputgroup->NumberValue) + "</li>").c_str();
            _content += String("<li>DCC Address: " + String(_outputgroup->AddressValue) + +"</li>").c_str();
            _content += "</ul>";
        } else {
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
    while (_servogroup != nullptr) {
        if (_servogroup->isActive()) {
            _content += String("<div>Servo group " + String(_servogroup->DesignationValue) + "</div>").c_str();
            _content += String("<ul>").c_str();
            _content += String("<li>DCC Address: " + String(_servogroup->AddressValue) + +"</li>").c_str();
            _content += String("<li>Travel time: " + String(_servogroup->TravelTimeValue) + +"</li>").c_str();
            _content += String("<li>Multiplier:  " + String(_servogroup->MultiplierValue) + +"</li>").c_str();
            _content += String("<li>Limit 1:     " + String(_servogroup->Limit1Value) + +"</li>").c_str();
            _content += String("<li>Limit 2:     " + String(_servogroup->Limit2Value) + +"</li>").c_str();
            _content += String("</ul>").c_str();
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

    _content += String("Total outputs used: ").c_str();
    _content += String(_count).c_str();
    _content += _fp.addNewLine(2).c_str();
    _content += String("Go to <a href='groups'>groups page</a> for enabling or disabling channels.").c_str();
    _content += _fp.addNewLine().c_str();
    _content += String("Go to <a href='config'>configure page</a> to change values.").c_str();
    _content += _fp.addNewLine().c_str();

    AsyncWebServerResponse* _response = request->beginChunkedResponse("text/html", [_content](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {

        std::string _chunk = "";
        size_t _len = min(_content.length() - index, maxLen);
        if (_len > 0) {
            _chunk = _content.substr(index, _len - 1);
            _chunk.copy((char*)buffer, _chunk.length());
        }
        if (index + _len <= _content.length())
            return _chunk.length();
        else
            return 0;

        });
    _response->addHeader("Server", "ESP Async Web Server");
    request->send(_response);
}

void handlePost(AsyncWebServerRequest* _request) {
    if (_request->hasParam("group", true)) {
        String _number = _request->getParam("group", true)->value();
        handleChannel(_number.toInt() - 1);
        _request->send(200, "text/plain", ButtonResponse);
    }

    // test if parameter all exists. If on enable all channels, if off disable all channels
    if (_request->hasParam("all", true)) {
        String _all = _request->getParam("all", true)->value();
        if (_all == "on") {
            for (int _i = 0; _i < 10; _i++) {
                handleChannel(_i);
            }
        }
        if (_all == "off") {
            for (int _i = 0; _i < 10; _i++) {
                handleChannel(_i);
            }
        }
        _request->send(200, "text/plain", ButtonResponse);
    }
}

void handleData(AsyncWebServerRequest* request) {
    AsyncJsonResponse* _response = new AsyncJsonResponse();
    _response->addHeader("Server", "ESP Async Web Server");
    JsonVariant& _json = _response->getRoot();

    _json["rssi"] = WiFi.RSSI();

    uint8_t _i = 0;
    OutputGroup* _outputgroup = &OutputGroup1;
    while (_outputgroup != nullptr) {
        if (_outputgroup->isActive()) {
            _json["output" + String(_i)] = ChannelIsOn(_i);
        }
        _outputgroup = (OutputGroup*)_outputgroup->getNext();
        _i += 1;
    }

    _i = 0;
    ServoGroup* _servogroup = &ServoGroup1;
    while (_servogroup != nullptr) {
        if (_servogroup->isActive()) {
            _json["servo" + String(_i)] = ChannelIsOn(_i);
        }
        _servogroup = (ServoGroup*)_servogroup->getNext();
        _i += 1;
    }

    _response->setLength();
    request->send(_response);
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
        [](const char* updatePath) { AsyncUpdater.setup(&server, updatePath); },
        [](const char* userName, char* password) { AsyncUpdater.updateCredentials(userName, password); }
    );

    iotWebConf.setConfigSavedCallback(&configSaved);
    iotWebConf.setWifiConnectionCallback(&wifiConnected);

    iotWebConf.getApTimeoutParameter()->visible = true;

    // -- Initializing the configuration.
    iotWebConf.init();

    // -- Set up required URL handlers on the web server.
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { handleRoot(request); });
    server.on("/config", HTTP_ANY, [](AsyncWebServerRequest* request) {
        AsyncWebRequestWrapper asyncWebRequestWrapper(request);
        iotWebConf.handleConfig(&asyncWebRequestWrapper);
        }
    );
    server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) { handleData(request); });
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request) { handlePost(request); });
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
