#ifndef UNTITLED2_BUTTON_H
#define UNTITLED2_BUTTON_H

#endif //UNTITLED2_BUTTON_H

#include "Arduino.h"

struct button {
    byte pin;
    byte state = HIGH;
    byte previousState = HIGH;
    byte lastState = HIGH;

    explicit button(byte pin) : pin(pin) {}

    void initialize() const {
        pinMode(pin, INPUT_PULLUP);
    }

    bool isPressed() {
        state = digitalRead(pin);
        previousState = lastState;
        lastState = state;

        return previousState == LOW && state == HIGH;
    }

    bool isPressing() {
        state = digitalRead(pin);
        previousState = lastState;
        lastState = state;

        return state == LOW;
    }
};