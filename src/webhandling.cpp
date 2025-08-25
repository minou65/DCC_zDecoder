#define DEBUG_WIFI(m) SERIAL_DBG.print(m)
#define IOTWEBCONF_DEBUG_TO_SERIAL

#include <Arduino.h>
#include <ArduinoOTA.h>

#include <WiFi.h>
#include <DNSServer.h>
#include <IotWebConf.h>
#include <IotWebConfAsyncClass.h>
#include <IotWebConfAsyncUpdateServer.h>
#include <IotWebRoot.h>

#include "webhandling.h"
#include "common.h"
#include "html.h"
#include "favicon.h"
#include "ESP32_DecoderCore.h"

bool ShouldReboot = false;

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "zDecoder";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "123456789";


// -- Configuration specific key. The value should be modified if config structure was changed.
#define CONFIG_VERSION "010"

// -- Status indicator pin.
//      First it will light up (kept LOW), on Wifi connection it will blink,
//      when connected to the Wifi it will turn off (kept HIGH).
#define STATUS_PIN LED_BUILTIN
#define ON_LEVEL HIGH

// -- Method declarations.
void onProgress(size_t prg, size_t sz);
void handleRoot(AsyncWebServerRequest* request);

extern void zDecoderReset();

// -- Callback methods.
void convertSettings();
void configSaved();
void wifiConnected();
void handleConfigSavedPage(iotwebconf::WebRequestWrapper* webRequestWrapper);

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebServerWrapper asyncWebServerWrapper(&server);
AsyncUpdateServer AsyncUpdater;

IotWebConf iotWebConf(thingName, &dnsServer, &asyncWebServerWrapper, wifiInitialApPassword, CONFIG_VERSION);

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
OutputGroup OutputGroup11 = OutputGroup("og11");
OutputGroup OutputGroup12 = OutputGroup("og12");
OutputGroup OutputGroup13 = OutputGroup("og13");
OutputGroup OutputGroup14 = OutputGroup("og14");
OutputGroup OutputGroup15 = OutputGroup("og15");
OutputGroup OutputGroup16 = OutputGroup("og16");

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
            String(FPSTR(html_js_hideclass));
    }
    virtual String getFormEnd() {
        return
            OptionalGroupHtmlFormatProvider::getFormEnd() +
            String(FPSTR(html_form_end));
    }
};
CustomHtmlFormatProvider customHtmlFormatProvider;

class MyHtmlRootFormatProvider : public HtmlRootFormatProvider {
protected:
    virtual String getStyleInner() {
		return 
            HtmlRootFormatProvider::getStyleInner() + 
            String(FPSTR(html_css_code));
    }

    virtual String getScriptInner() {
        String s_ = 
            HtmlRootFormatProvider::getScriptInner() +
			String(FPSTR(html_button_response)) +
			String(FPSTR(html_js_updatedata));
        s_.replace("{millisecond}", "500");
        return s_;
    }
};

bool formValidator(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    bool allValid_ = true;
    uint8_t outputsUsed_ = 0;
    OutputGroup* group_ = &OutputGroup1;

    while (group_ != nullptr) {
        // Reset error messages
        group_->numberParam().errorMessage = nullptr;
		group_->modeParam().errorMessage = nullptr;
		group_->TimeOnFadeParam().errorMessage = nullptr;
		group_->TimeOffFadeParam().errorMessage = nullptr;

        // Build parameter IDs
        char filenameID_[STRING_LEN];
        snprintf(filenameID_, STRING_LEN, "%s-filename", group_->getId());
        String filename_ = webRequestWrapper->arg(filenameID_);

        char modeID_[STRING_LEN];
        snprintf(modeID_, STRING_LEN, "%s-mode", group_->getId());
        String modeStr_ = webRequestWrapper->arg(modeID_);

        char numberID_[STRING_LEN];
        snprintf(numberID_, STRING_LEN, "%s-number", group_->getId());
        String numberStr_ = webRequestWrapper->arg(numberID_);

        char visibleID_[STRING_LEN];
        snprintf(visibleID_, STRING_LEN, "%sv", group_->getId());

        // Skip inactive groups
        if (webRequestWrapper->hasArg(visibleID_) && webRequestWrapper->arg(visibleID_) == "inactive") {
            Serial.printf("Group %s is inactive, skipping validation.\n", visibleID_);
            group_ = (OutputGroup*)group_->getNext();
            continue;
        }

        int mode_ = atoi(modeStr_.c_str());
        // Skip groups with mode 0 (No mode)
        if (mode_ == 0) {
            Serial.printf("Group %s has mode 0, skipping validation.\n", group_->getId());
            group_ = (OutputGroup*)group_->getNext();
            continue;
        }

        Serial.printf("Validating group %s...\n", group_->getId());

        // Output number validation
        int number_ = 0;
        switch (mode_) {
        case 0: // No mode
            number_ = 0;
            break;
        case 52: case 53: case 54: case 55:
        case 60: case 61: case 62:
            number_ = atoi(numberStr_.c_str());
            break;
        default:
            number_ = group_->getNumber(mode_);
        }

        bool isServo = (mode_ == 190 || mode_ == 191 || mode_ == 192 || mode_ == 193);
        if (isServo && outputsUsed_ >= 5) {
            group_->modeParam().errorMessage = "Fehler: Servo-Ausgänge müssen auf den ersten 5 Positionen liegen.";
            allValid_ = false;
        }

        // Servo-spezifische Validierung für onfade/offfade
        if (isServo) {
            char onfadeID_[STRING_LEN];
            snprintf(onfadeID_, STRING_LEN, "%s-onfade", group_->getId());
            String onfadeStr_ = webRequestWrapper->arg(onfadeID_);
            int onfade_ = onfadeStr_.toInt();

            char offfadeID_[STRING_LEN];
            snprintf(offfadeID_, STRING_LEN, "%s-offfade", group_->getId());
            String offfadeStr_ = webRequestWrapper->arg(offfadeID_);
            int offfade_ = offfadeStr_.toInt();

            if (onfade_ < 0 || onfade_ > 180) {
                group_->TimeOnFadeParam().errorMessage = "Fehler: Limit 1 muss zwischen 0 und 180 liegen.";
                allValid_ = false;
            }
            if (offfade_ < 0 || offfade_ > 180) {
                group_->TimeOffFadeParam().errorMessage = "Fehler: Limit 2 muss zwischen 0 und 180 liegen.";
                allValid_ = false;
            }
        }

        Serial.printf("    Mode: %d, Outputs: %d\n", mode_, number_);
        Serial.print(F("    max outputs: ")); Serial.println(MAX_OUTPUT_PINS);
        Serial.print(F("    Outputs used: ")); Serial.println(outputsUsed_);

        if (number_ < 1) {
            group_->numberParam().errorMessage = "Fehler: Anzahl der Ausgänge muss mindestens 1 sein.";
            allValid_ = false;
        }
        else if (number_ + outputsUsed_ > MAX_OUTPUT_PINS) {
            group_->numberParam().errorMessage = "Fehler: Nicht genügend freie Ausgänge verfügbar.";
            allValid_ = false;
        }
        else {
            outputsUsed_ += number_;
        }

        group_ = (OutputGroup*)group_->getNext();
    }
    return allValid_;
}

void onProgress(size_t prg, size_t sz) {
    static size_t lastPrinted_ = 0;
    size_t currentPercent_ = (prg * 100) / sz;

    if (currentPercent_ % 5 == 0 && currentPercent_ != lastPrinted_) {
        Serial.printf("Progress: %d%%\n", currentPercent_);
        lastPrinted_ = currentPercent_;
    }
}

void handleRoot(AsyncWebServerRequest* request) {
    AsyncWebRequestWrapper asyncWebRequestWrapper(request);
    if (iotWebConf.handleCaptivePortal(&asyncWebRequestWrapper)) {
        return;
    }

    AsyncResponseStream* response = request->beginResponseStream("text/html", 512);

    MyHtmlRootFormatProvider fp_;

    response->print(fp_.getHtmlHead(iotWebConf.getThingName()));
    response->print(fp_.getHtmlStyle());
    response->print(fp_.getHtmlHeadEnd());
    response->print(fp_.getHtmlScript());

    response->print("<h2 style=\"text-align:center;\">");
    response->print(iotWebConf.getThingName());
    response->print("</h2>");

    response->print(fp_.getHtmlTable());
    response->print(fp_.getHtmlTableRow());
    response->print(fp_.getHtmlTableCol());

    response->print("<fieldset align=left style=\"border: 1px solid\">\n");
    response->print("<table border=\"0\" align=\"center\" width=\"100%\">\n");
    response->print("<tr><td align=\"left\"> </td></td><td align=\"right\"><span id=\"RSSIValue\">no data</span></td></tr>\n");

    response->print(fp_.getHtmlTableEnd());
    response->print(fp_.getHtmlFieldsetEnd());

    response->print("<fieldset align=left style=\"border: 1px solid\">\n");
    response->print("<table border=\"0\" align=\"center\" width=\"100%\">\n");

    uint8_t i_ = 1;
    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if ((outputgroup_->isActive()) && (outputgroup_->getMode()) >= 10) {
            String b_ = html_button_toggle;
            b_.replace("[value]", String(i_));
            b_.replace("[name]", String(outputgroup_->getDesignation()) + " (" + String(outputgroup_->getMode()) + ")");
            b_.replace("[id]", "output" + String(i_));
            if (decoder.isOn(i_ - 1)) {
                b_.replace("red", "green");
            }
            response->print(b_);
            response->print(fp_.addNewLine(2));
            i_ += 1;
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();
    }

    response->print(fp_.getHtmlTableEnd());
    response->print(fp_.getHtmlFieldsetEnd());

    response->print("<fieldset align=left style=\"border: 1px solid\">\n");
    response->print("<table border=\"0\" align=\"center\" width=\"100%\">\n");
    response->print(fp_.getHtmlTableRow());
    response->print(fp_.getHtmlTableCol());

    String on_ = html_button_blue;
    on_.replace("group", "all");
    on_.replace("[value]", "on");
    on_.replace("[name]", "All on");
    on_.replace("[id]", "allon");
    response->print(on_);

    response->print(fp_.addNewLine(1));

    String off_ = html_button_blue;
    off_.replace("group", "all");
    off_.replace("[value]", "off");
    off_.replace("[name]", "All off");
    off_.replace("[id]", "alloff");
    response->print(off_);

    response->print(fp_.getHtmlTableColEnd());
    response->print(fp_.getHtmlTableRowEnd());
    response->print(fp_.getHtmlTableEnd());
    response->print(fp_.getHtmlFieldsetEnd());

    response->print(fp_.addNewLine(2));

    response->print(fp_.getHtmlTable());
    response->print(fp_.getHtmlTableRowText("<a href = 'config'>Configuration</a>"));
    response->print(fp_.getHtmlTableRowText("<a href = 'settings'>Overview</a>"));
    response->print(fp_.getHtmlTableRowText(fp_.getHtmlVersion(Version)));
    response->print(fp_.getHtmlTableEnd());

    response->print(fp_.getHtmlTableColEnd());
    response->print(fp_.getHtmlTableRowEnd());
    response->print(fp_.getHtmlTableEnd());
    response->print(fp_.getHtmlEnd());

    response->addHeader("Server", "ESP Async Web Server");
    request->send(response);
}

void handleSettings(AsyncWebServerRequest* request) {
    String content_;
    MyHtmlRootFormatProvider fp_;
    uint8_t i_ = 1;
    uint8_t count_ = 0;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName());
    content_ += fp_.getHtmlStyle();
    content_ += fp_.getHtmlHeadEnd();
    content_ += fp_.getHtmlScript();

    content_ += "<h2 style=\"text-align:center;\">";
    content_ += iotWebConf.getThingName();
    content_ += "</h2>";

    content_ += fp_.getHtmlTable();
    content_ += fp_.getHtmlTableRow();
    content_ += fp_.getHtmlTableCol();

    // content_ += "<h2>Overview for " + String(iotWebConf.getThingName()) + "</h2>";

    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if (outputgroup_->isActive()) {
            content_ += String("<div>Output group ") + outputgroup_->getDesignation() + "</div>";
            content_ += F("<ul>");
            content_ += "<li>Mode: " + String(outputgroup_->getMode()) + "</li>";
            content_ += "<li>Outputs used: " + String(outputgroup_->getNumber()) + "</li>";
            content_ += "<li>DCC Address: " + String(outputgroup_->getAddress()) + "</li>";
            content_ += F("</ul>");
        }
        else {
            outputgroup_->applyDefaultValues();
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();
    }

    content_ += fp_.getHtmlTableEnd();
    content_ += fp_.getHtmlFieldsetEnd();

    content_ += fp_.addNewLine(2);

    content_ += fp_.getHtmlTable();
    content_ += fp_.getHtmlTableRowText("<a href = '/'>Home</a>");
    content_ += fp_.getHtmlTableRowText("<a href = 'config'>Configuration</a>");
    content_ += fp_.getHtmlTableRowText(fp_.getHtmlVersion(Version));
    content_ += fp_.getHtmlTableEnd();

    content_ += fp_.getHtmlTableColEnd();
    content_ += fp_.getHtmlTableRowEnd();
    content_ += fp_.getHtmlTableEnd();
    content_ += fp_.getHtmlEnd();

    request->send(200, "text/html", content_);
}

void handleData(AsyncWebServerRequest* request) {
    String json_ = "{";
    json_ += "\"rssi\":\"" + String(WiFi.RSSI()) + "\"";
    uint8_t i_ = 0;
    OutputGroup* outputgroup_ = &OutputGroup1;
    while (outputgroup_ != nullptr) {
        if (outputgroup_->isActive()) {
            json_ += ",\"output" + String(i_ + 1) + "\":" + decoder.isOn(i_);
            i_ += 1;
        }
        outputgroup_ = (OutputGroup*)outputgroup_->getNext();

    }

    json_ += "}";

    request->send(200, "application/json", json_);

}

void handlePost(AsyncWebServerRequest* request) {
    Serial.println("POST request");
    if (request->hasParam("group", true)) {

        String value_ = request->getParam("group", true)->value();
        uint8_t group_ = value_.toInt() - 1;
        Serial.println("    Toggel group: " + String(group_));
        if (group_ < 10) {
            decoder.toggl(group_);

        }
        else {
            request->send(400, "text/plain", "Invalid group");
            return;
        }

        AsyncWebServerResponse* response = request->beginResponse(302, "text/plain", "redirection");
        response->addHeader("Location", "/");
        request->send(response);
        return;
    }

    if (request->hasParam("reset", true)) {
        Serial.println("    Resetting decoder");
        zDecoderReset();
        AsyncWebServerResponse* response = request->beginResponse(302, "text/plain", "Reset successful");
        response->addHeader("Location", "/");
        request->send(response);
        return;
    }

    if (request->hasParam("all", true)) {
        String value_ = request->getParam("all", true)->value();
        if (value_ == "on") {
            for (int i_ = 0; i_ < 10; i_++) {
                decoder.on(i_);
            }
        }
        else if (value_ == "off") {
            for (int i_ = 0; i_ < 10; i_++) {
                decoder.off(i_);
            }
        }
        else {
            request->send(400, "text/plain", "Invalid value");
            return;
        }
        AsyncWebServerResponse* response = request->beginResponse(302, "text/plain", "redirection");
        response->addHeader("Location", "/");
        request->send(response);
        return;
    }
    request->send(400, "text/plain", "Invalid request");
}

void handleAPPasswordMissingPage(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    String content_;
    MyHtmlRootFormatProvider fp_;

    content_ += fp_.getHtmlHead(iotWebConf.getThingName());
    content_ += fp_.getHtmlStyle();
    content_ += fp_.getHtmlHeadEnd();
    content_ += "<body>";
    content_ += "You should change the default AP password to continue.";
    content_ += fp_.addNewLine(2);
    content_ += "Return to <a href = ''>configuration page</a>.";
    content_ += fp_.addNewLine(1);
    content_ += "Return to <a href='/'>home page</a>.";
	content_ += "</body>";
	content_ += fp_.getHtmlEnd();

	webRequestWrapper->sendHeader("Content-Length", String(content_.length()));
	webRequestWrapper->send(200, "text/html", content_);
}

void handleSSIDNotConfiguredPage(iotwebconf::WebRequestWrapper* webRequestWrapper) {
    webRequestWrapper->sendHeader("Location", "/", true);
    webRequestWrapper->send(302, "text/plain", "SSID not configured");
}

void handleConfigSavedPage(iotwebconf::WebRequestWrapper* webRequestWrapper){
    webRequestWrapper->sendHeader("Location", "/", true);
    webRequestWrapper->send(302, "text/plain", "Config saved");
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
	OutputGroup10.setNext(&OutputGroup11);
	OutputGroup11.setNext(&OutputGroup12);
	OutputGroup12.setNext(&OutputGroup13);
	OutputGroup13.setNext(&OutputGroup14);
	OutputGroup14.setNext(&OutputGroup15);
	OutputGroup15.setNext(&OutputGroup16);


    iotWebConf.setHtmlFormatProvider(&customHtmlFormatProvider);

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
    iotWebConf.addParameterGroup(&OutputGroup11);
	iotWebConf.addParameterGroup(&OutputGroup12);
	iotWebConf.addParameterGroup(&OutputGroup13);
	iotWebConf.addParameterGroup(&OutputGroup14);
	iotWebConf.addParameterGroup(&OutputGroup15);
	iotWebConf.addParameterGroup(&OutputGroup16);

    iotWebConf.setConfigSavedCallback(&configSaved);
    iotWebConf.setWifiConnectionCallback(&wifiConnected);
    iotWebConf.setConfigSavedPage(&handleConfigSavedPage);
	iotWebConf.setConfigAPPasswordMissingPage(&handleAPPasswordMissingPage);
	iotWebConf.setConfigSSIDNotConfiguredPage(&handleSSIDNotConfiguredPage);

    iotWebConf.setStatusPin(STATUS_PIN, ON_LEVEL);
    iotWebConf.setConfigPin(CONFIG_PIN);
    iotWebConf.getApTimeoutParameter()->visible = true;

    iotWebConf.setFormValidator(&formValidator);

    // -- Define how to handle updateServer calls.
    iotWebConf.setupUpdateServer(
        [](const char* updatePath) { AsyncUpdater.setup(&server, updatePath, onProgress); },
        [](const char* userName, char* password) { AsyncUpdater.updateCredentials(userName, password); });

    // -- Initializing the configuration.
    iotWebConf.init();


    // -- Set up required URL handlers on the web server.
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { handleRoot(request); });
    server.on("/config", HTTP_ANY, [](AsyncWebServerRequest* request) {
        auto* asyncWebRequestWrapper = new AsyncWebRequestWrapper(request);
        iotWebConf.handleConfig(asyncWebRequestWrapper);
        }
    );
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest* request) { handleSettings(request); });
	server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) { handleData(request); });
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request) { handlePost(request); });
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

	convertSettings();
}

void webloop(){
    iotWebConf.doLoop();
    ArduinoOTA.handle();

    if (ShouldReboot || AsyncUpdater.isFinished()) {
        delay(1000);
        ESP.restart();
    }
}

void wifiConnected() {
    ArduinoOTA.begin();

}

void convertSettings() {
    ArduinoOTA.setHostname(iotWebConf.getThingName());
}

void configSaved(){
    ResetDCCDecoder = true;
	convertSettings();
    Serial.println(F("Configuration was updated."));
}
