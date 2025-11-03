// common.h

#ifndef _COMMON_h
#define _COMMON_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define STRING_LEN 30
#define NUMBER_LEN 5

// -- When CONFIG_PIN is pulled to ground on startup, the Thing will use the initial
//      password to buld an AP. (E.g. in case of lost password)
#define CONFIG_PIN GPIO_NUM_39

extern bool ResetDCCDecoder;
extern char Version[];

const char OutputModeValues[][4] = {
    "0",
    "1",
    "40",
    "50",
    "51",

    "80",
    "81",
    "82",
    "83",

    "52",
    "53",
    "54",
    "55",

    "60",
    "61",
    "62",

    "70",
    "71",
    "72",
    "73",

    //"102",
    //"103",
    //"104",
    //"105",
    //"106",
    //"110",

	"190", // Servo Impuls
	"191", // Servo Flip
	"192", // Servo Pendel
	"193", // Servo Weiche

    "201",
    "202",

    "203"
};

const char OutputModeNames[][50] = {
    "-",                    // 0
    "Watchdog",             // 1
    "einfacher Ausgang (1)",// 40
    "Blinker (1)",          // 50
    "Wechselblinker (2)",   // 51

    "Fernseher (1)",        // 80
    "Schweisslicht (3)",    // 81
    "Feuer (3)",            // 82
    "Blitzlicht (1)",       // 83

    "Lauflicht 1 (1..16)", // 52
    "Lauflicht 2 (1..16)", // 53
    "Lauflicht 3 (1..16)", // 54
    "Lauflicht 4 (1..16)", // 55

    "Hausbeleuchtung (1..16)", // 60
    "Neonlampen (1..16)",      // 61
    "Natriumlampen (1..16)",   // 62

    //"SBB_Hauptsignal_102 (2)", // 102
    //"SBB_Hauptsignal_103 (3)", // 103
    //"SBB_Hauptsignal_104 (4)", // 104
    //"SBB_Hauptsignal_105 (5)", // 105
    //"SBB_Hauptsignal_106 (5)", // 106
    //"SBB_Zwergsignal_110 (3)", // 110
    // 
    "Lichtsignal (3)",  // 70
    "Lichtsignal Haupt-/Nebenstrasse (6)", // 71
    "Lichtsignal Hauptstrasse/Fussgänger (6)", // 72
    "Lichtsignal Haupt-/Nebenstrasse/Fussgänger (9)", // 73

    "Servo Impuls (1)",      // 190, nach einer eingestellten Zeit geht wieder zurück in die Ausgangsposition
    "Servo Flip (1)",        // 191, wechselt zwischen zwei Positionen
    "Servo Pendel (1)",       // 192, wechsselt zwischen den positionen bis der Servo ausgeschaltet wird
    "Servo Weiche (1)",        //193
    
    "Entkuppler (1)",          // 201
    "Weiche (2)",              // 202
    "Motor (1)"              // 203
};

const char InputPinValues[][4] = {
    "0", 
	"255",
    "101", "102", "103", "104", "105", "106", "107", "108",
    "109", "110", "111", "112", "113", "114", "115", "116"
};
const char InputPinNames[][12] = {
    "-",
	"Autostart",
    "Output1", "Output2", "Output3", "Output4", "Output5", "Output6", "Output7", "Output8",
    "Output9", "Output10", "Output11", "Output12", "Output13", "Output14", "Output15", "Output16",
};

const char InputEdgeValues[][3] = { "0", "1", "2" };
const char InputEdgeNames[][8] = { "Rising", "Falling", "Both" };

#endif

