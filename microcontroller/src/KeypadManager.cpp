#include "KeypadManager.hpp"
#include "Utils/Log.hpp"

// #define MAX_MENU_ITEMS 5  // Change based on how many menu items you need

// String menuItems[MAX_MENU_ITEMS] = {
//     "Info Device", "Register", "Sync Data", "Reset Data", "Reboot"
// };

KeypadManager::KeypadManager() : keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS)
{
}

bool KeypadManager::initialize()
{
    LOGD("Keypad initialized");
    return true;
}

int KeypadManager::KeypadPoll()
{
    int result = KEYPAD_MODE_NONE;
    char key = keypad.getKey();

    if (key)
    {
        if (key == '*')
        {
            firstPressTime = millis();
            starPressed = true;
        }
        else if (starPressed && (millis() - firstPressTime <= 1000))
        {
            if (key == '0')
            {
                result = KEYPAD_MODE_RESET;
                starPressed = false;
                firstPressTime = 0;
            }
            else if (key == '#')
            {
                result = KEYPAD_MODE_REGISTER;
                starPressed = false;
                firstPressTime = 0;
            }
        }

        if (millis() - firstPressTime > 1000)
        {
            starPressed = false;
            firstPressTime = 0;
        }
    }

    return result;
}

String KeypadManager::GetKeypadInputNumber(const String &label, bool isPin, int maxLength)
{
    String value;
    OutputManager::clearTextSection();
    OutputManager::drawInputNumber(label,isPin,maxLength,value);

    while (true) {
        char key = keypad.getKey();

        if (key && isDigit(key) && value.length() < maxLength) {
            value += key;
            if (isPin)
                Serial.print("*");  // Mask PIN input
            else
                Serial.print(key);   // Show numbers for IDs
            
            OutputManager::drawInputNumber(label,isPin,maxLength,value);
        } 
        else if (key == BACKSPACE_KEY && !value.isEmpty()) {  // Backspace
            value.remove(value.length() - 1);
            Serial.print("\b \b");
            OutputManager::drawInputNumber(label,isPin,maxLength,value);
        } 
        else if (key == RESET_KEY) {  // Clear input
            for (size_t i = 0; i < value.length(); i++) {
                Serial.print("\b \b"); // Remove printed characters
            }
            value = "";
            LOGD("Cleared. Re-enter:");
            OutputManager::drawInputNumber(label,isPin,maxLength,value);
        } 
        else if (key == CANCEL_KEY) {  // Cancel action
            LOGI("Canceled.");
            return "";  
        } 
        else if (key == ENTER_KEY) {  // Enter key
            if (isPin && value.length() != maxLength) {
                LOGI("PIN must be exactly %d digits!",maxLength);
                value = "";  // Reset input
                continue;
            }
            if (!isPin && value.isEmpty()) {
                LOGI("Input cannot be empty!");
                continue;
            }
            break;
        }
    }
    Serial.println();
    return value;
}

bool KeypadManager::GetKeypadInputConfirm()
{
    LOGD("(D = Yes / A = No)");
    while (true) {
        char key = keypad.getKey();

        if (key == CANCEL_KEY) {
            LOGI("No");
            return false;
        } 
        else if (key == ENTER_KEY) {
            LOGI("Yes");
            return true;
        }
    }
}

bool KeypadManager::EnterPin(const char * pin)
{
    LOGD("Enter 6-digit PIN:");
    String pinInput = GetKeypadInputNumber("Enter Pin",true,6);

    if(pinInput.isEmpty()){
        delay(1000);
        OutputManager::clearTextSection();
        return false;
    }

    if (pinInput != pin) {
        LOGD("Wrong Pin !!");
        OutputManager::setTextSection("Wrong Pin !!");
        delay(1000);
        OutputManager::clearTextSection();
        return false;
    }

    return true;
}
