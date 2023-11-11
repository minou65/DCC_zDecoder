# Blinkers

## Inhalt
- [Blinkers](#blinkers)
  - [Inhalt](#inhalt)
  - [einfacher Ausgang](#einfacher-ausgang)
  - [Blinker](#blinker)
  - [Wechselblinker](#wechselblinker)
  - [Fernseher](#fernseher)
  - [Schweissen](#schweissen)
  - [Feuer](#feuer)
  - [Blitzlicht](#blitzlicht)
  - [Lauflicht 1](#lauflicht-1)
  - [Lauflicht 2](#lauflicht-2)
  - [Lauflicht 3](#lauflicht-3)
  - [Lauflicht 4](#lauflicht-4)
  - [Hausbeleuchtung](#hausbeleuchtung)
  - [Natrium Lampe](#natrium-lampe)
  - [Neon Lampe](#neon-lampe)



## einfacher Ausgang
Ein einfacher Ausgang um z.B. eine Lampe oder Relais ein und auszuschalten.

| Channel | Led Farbe | 
|:-:|---|
| 1 | n.a.  |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | Beschreibung des Channels | |
| Modus | ein/aus | einfacher Ausgang |
| Address | DCC Adresse des Ausgangs | default 3 |


## Blinker
Einfacher blinker, welcher es erlaubt auch ein und aus zu faden

| Channel | Led Farbe |
|:-:|---|
| 1 | n.a.  |
| 2  | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Blinker |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | ein in ms | |
| Blinker time off | aus in ms | |
| Multiplikator | Multiplikator mit welchem die ein / aus Zeit multipliziert wird | |
| Fade Time on | Zeit in ms für das Fade on | |
| Fade Time off | Zeit in ms für das Fade off | |

## Wechselblinker
Wechselblinker, welcher es erlaubt auch ein und aus zu faden

| Channel | Led Farbe |
|:-:|---|
| 1 | n.a.  |
| 2  | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Wechselblinker |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | ein in ms | |
| Blinker time off | aus in ms | |
| Multiplikator | Multiplikator mit welchem die ein / aus Zeit multipliziert wird | |
| Fade Time on | Zeit in ms für das Fade on | |
| Fade Time off | Zeit in ms für das Fade off | |

## Fernseher
Simulation eines Fernsehers

| Channel | Led Farbe |
|:-:|---|
| 1 | blau  |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Fernseher |
| Address | DCC Adresse des Ausgangs | default 3 |



## Schweissen
Simuliert ein Schweisslicht

| Channel | Led Farbe |
|:-:|---|
| 1 | weiss  |
| 2  |blau|
| 3  |organge|

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Schweissen |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | minimalae Pause in ms zwischen zwei Schweiss vorgängen | |
| Blinker time off | maximale Pause in ms zwischen zwei Scheiss vorgängen  | |
| Multitiplikator | Faktor mit welchem die ein / aus Zeit mulitpliziert wird| |


## Feuer
Simuliert ein flackerndes Feuer

| Channel | Led Farbe |
|:-:|---|
| 1 | gelb |
| 2  | rot |
| 3  | gelb |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus |  |  Feuer |
| Address | DCC Adresse des Ausgangs | default 3 |

## Blitzlicht
Simuliert einen Fotoblitz. Es wird ein Blitzlicht gewitter erzeugt und dann jeweils zwischen 5 und 10 sekunden gewartet, bis das nächste Gewitter erzeugt wird

| Channel | Led Farbe |
|:-:|---|
| 1 | weiss |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus |  |  Feuer |
| Address | DCC Adresse des Ausgangs | default 3 |

## Lauflicht 1
Kette durchgehen und notwendige LED einschalten, die anderen aussschalten

| Channel | Led Farbe |
|:-:|---|
| 1 - max | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Lauflicht 1 |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | | |
| Blinker time off | | |
| Einheit | | |


## Lauflicht 2
Alle Lampen werden nacheinander eingschaltet, am Schluss werden alle ausgeschaltet

| Channel | Led Farbe |
|:-:|---|
| 1 - max | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Lauflicht 2 |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | | |
| Blinker time off | | |
| Einheit | | |

## Lauflicht 3
Lampe für Lampe wird eingeschaltet. Sind alle eingeschaltet, wird mit der ersten Lampe begonnen zu löschen.

| Channel | Led Farbe |
|:-:|---|
| 1 - max | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Lauflicht 3 |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | | |
| Blinker time off | | |
| Einheit | | |

## Lauflicht 4
Funktion analaog Lauflicht 1, aber in beide richtungen (Knigth rider)

| Channel | Led Farbe |
|:-:|---|
| 1 - max | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | |
| Modus | | Lauflicht 4 |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | | |
| Blinker time off | | |
| Einheit | | |

## Hausbeleuchtung

| Channel | Led Farbe |
|:-:|---|
| 1 - max | n.a. |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Hausbleuchtung |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 |
| Blinker time on | | |
| Blinker time off | | |
| Einheit | | |

## Natrium Lampe
Simuliert eine oder mehere Natriumlampen. Beim einschalten beginnt die lampe zu glühen um dann den Betriebsdruck aufzubauen und voll zu leuchten. In dieser Zeit leuchtet die LED immer heller. Beim ausschalten glüht die Lampe aus um nach einer bestimmten Zeit komplett dunckel zu sein. Über die Zahl chance wird bestimmt ob es allenfalls ein defekte Lampe gibt. Es wird jweils gewürfelt, welche Lampe defekt ist.

| Channel | Led Farbe |
|:-:|---|
| 1 - max | gelb |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Natriumlampen |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 ||
| Mulitiplier | Prozentuale chance auf defekte Lampe: <br/> für 50:50 den Wert 1. <br/> für 33:66 den Wert 2, etc. <br/> wenn Null dann gibt es keine defekte Lampe <br/> wenn 255, dann gibt es immer eine defekte Lampe | |
| Fade Time on | minimale Zeit für Glühen| |
| Fade Time off | maximale Zeit für Glüchen | |

## Neon Lampe
Simuliert eine oder mehere Nenolampen. Beim einschalten blinken die Lampen 1-5 mal. Je nach Anzahl Lampen und einstellung der Chance kommt es immer mal vor das eis eine defekte Lampe gibt. Diese wird dann dauerhaft versuchen einzuschalten.

| Channel | Led Farbe |
|:-:|---|
| 1 - max | weiss |

| Feld | Beschreibung | Wert |
| --- | --- | --- |
| Designation | | |
| Modus | | Neonlampen |
| Number of outputs| 1 - 16 | |
| Address | DCC Adresse des Ausgangs | default 3 |
| Mulitiplier | Prozentuale chance auf defekte Lampe: <br/> für 50:50 den Wert 1. <br/> für 33:66 den Wert 2, etc. <br/> wenn Null dann gibt es keine defekte Lampe <br/> wenn 255, dann gibt es immer eine defekte Lampe | |