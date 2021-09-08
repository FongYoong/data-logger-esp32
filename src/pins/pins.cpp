#include "pins.h"

SSD1306Wire oledDisplay(0x3c, SDA, SCL);
DHT dht(DHTPIN, DHTTYPE);

void pinsSetup() {
    pinMode(button_left, INPUT_PULLUP);
    pinMode(button_right, INPUT_PULLUP);
    pinMode(button_enter, INPUT_PULLUP);
    pinMode(button_back, INPUT_PULLUP);
    pinMode(warning_LED, OUTPUT);

    dht.begin();
}