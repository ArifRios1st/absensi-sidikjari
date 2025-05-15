#pragma once
#include <Keypad.h>
#include "configs.hpp"

#define KEYPAD_MODE_NONE -1
#define KEYPAD_MODE_REGISTER 0
#define KEYPAD_MODE_RESET 1

class KeypadManager
{
public:
    KeypadManager();
    bool initialize();

    int KeypadPoll();

    String GetKeypadInputNumber(const String &label,bool isPin, int maxLength);
    bool GetKeypadInputConfirm();
    bool EnterPin(const char * pin);

protected:
    Keypad keypad;

    const char keys[KEYPAD_ROWS][KEYPAD_COLS] PROGMEM = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };
    byte rowPins[KEYPAD_ROWS] = {13, 12, 14, 27}; // Connect to the row pinouts
    byte colPins[KEYPAD_COLS] = {26, 25, 33, 32}; // Connect to the column pinouts

    unsigned long firstPressTime = 0;
    bool starPressed = false;
    bool hashPressed = false;

    const char CANCEL_KEY = 'A';
    const char BACKSPACE_KEY = 'B';
    const char RESET_KEY = 'C';
    const char ENTER_KEY = 'D';

    const char UP_KEY = '2';
    const char DOWN_KEY = '8';
    const char LEFT_KEY = '4';
    const char RIGHT_KEY = '6';
};