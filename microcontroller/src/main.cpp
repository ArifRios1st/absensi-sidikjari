#include <Arduino.h>

#include "configs.hpp"
#include "FingerprintManager.hpp"
#include "DatabaseManager.hpp"
#include "KeypadManager.hpp"
#include "WifiManager.hpp"
#include "ApiManager.hpp"

#include "Utils/Log.hpp"

HardwareSerial mySerial(2);
DatabaseManager database;
FingerprintManager finger(mySerial, database);
KeypadManager keypad;
WifiManager wifiManager(database);
ApiManager apiManager(wifiManager.url, wifiManager.token);

int finger_id = -1;
int user_id = -1;

void registerUser();
void resetData();
void syncData();

void syncDataTask(void *pvParameters);

void setup()
{
    Serial.begin(MONIITOR_SERIAL);
    mySerial.begin(FINGERPRINT_SERIAL);
    while (!Serial || !mySerial)
        ;

    OutputManager::initialize();
    delay(1000);
    OutputManager::setTextSection("Initializing");
    delay(1000);

    if (
        !database.Initialize() ||
        !finger.initialize() ||
        !wifiManager.initialize() ||
        !keypad.initialize())
    {
        ESP.restart();
    }

    wifiManager.syncSettings();

    wifiManager.connect();
    wifiManager.startAP();

    if (wifiManager.isConnected())
    {
        apiManager.UpdateKapasitas(finger.GetCapacity());
    }

    OutputManager::setTextSection("Initializing Complete");
    delay(1000);
    OutputManager::clearTextSection();
}

void loop()
{
    switch (keypad.KeypadPoll())
    {
    case KEYPAD_MODE_REGISTER:
        registerUser();
        break;
    case KEYPAD_MODE_RESET:
        resetData();
        break;

    default:
        break;
    }

    if (finger.GetFingerID(&finger_id))
    {
        if (database.getUserIdByFingerprint(finger_id, &user_id) == SQLITE_OK)
        {
            Serial.printf("userID: %d\n", user_id);
            delay(1000);
            OutputManager::clearTextSection();
        }
    }
    else
    {
        finger_id = -1;
        user_id = -1;
    }
}

void resetData()
{
    if (!keypad.EnterPin(wifiManager.admin_pin.c_str()))
    {
        return;
    }

    OutputManager::clearTextSection();
    LOGD("Are you sure to reset data ?");
    OutputManager::drawConfirm("reset data");
    if (keypad.GetKeypadInputConfirm())
    {
        OutputManager::setTextSection("reseting data");
        finger.ResetData();
        database.ResetData();

        LOGD("Data Reset Successfully !");
        OutputManager::setTextSection("Success !");
        delay(3000);
        OutputManager::clearTextSection();
    }
}

void syncData()
{
    cJSON *unsynced = nullptr;
    if (database.getUnsyncedAbsences(&unsynced) == SQLITE_OK && unsynced)
    {
        cJSON *syncResult = nullptr;
        String syncMessage;

        if (apiManager.SyncAbsenceDatas(unsynced, &syncResult, syncMessage))
        {
            for (cJSON *item = syncResult->child; item; item = item->next)
            {
                cJSON *fid = cJSON_GetObjectItem(item, "fingerprint_id");
                cJSON *status = cJSON_GetObjectItem(item, "status");

                if (!cJSON_IsNumber(fid) || !cJSON_IsNumber(status))
                    continue;

                int absenceId  = fid->valueint;
                int stat = status->valueint;

                if (stat == 1)
                {
                    database.markAbsenceAsSyncedById(absenceId);
                }
                else if (stat == 3)
                {
                    database.deleteAbsenceById(absenceId);
                }
                // status == 2: do nothing
            }
            cJSON_Delete(syncResult);
        }
        else
        {
            LOGE("Sync failed: %s", syncMessage.c_str());
        }

        cJSON_Delete(unsynced);
    }
}

void syncDataTask(void *pvParameters)
{
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000 * 60 * 1));
        if(wifiManager.isConnected()){
            syncData();
        }
    }
    
}

void registerUser()
{
    if (!keypad.EnterPin(wifiManager.admin_pin.c_str()))
    {
        delay(3000);
        OutputManager::clearTextSection();
        return;
    }

    LOGD("Enter User ID:");
    String userID = keypad.GetKeypadInputNumber("User ID", false, 3);

    if (userID.isEmpty())
    {
        delay(3000);
        OutputManager::clearTextSection();
        return;
    }

    LOGD("Enter Fingerprint ID:");
    String fingerprintId = keypad.GetKeypadInputNumber("Finger ID", false, 3);

    if (fingerprintId.isEmpty())
        return;

    int fingerPrintIdInt = fingerprintId.toInt();
    int userIdInt = userID.toInt();

    if (fingerPrintIdInt >= 1000)
    {
        LOGD("Exceed max ID");
        return;
    }

    String message;
    if (!finger.AddFinger(fingerPrintIdInt) || database.insertFingerprint(userIdInt, fingerPrintIdInt) != SQLITE_OK || !apiManager.SetFingerprintScanned(userIdInt, fingerPrintIdInt, message))
    {
        finger.DeleteFinger(fingerPrintIdInt);
        database.deleteFingerprintById(fingerPrintIdInt);

        LOGD("User %s finger ID %s Registered Failed!\n", userID.c_str(), fingerprintId.c_str());
        OutputManager::setTextSection("Registered Failed!\n" + message);
        delay(3000);
        OutputManager::clearTextSection();
        return;
    }

    LOGD("User %s finger ID %s Registered Successfully!\n", userID.c_str(), fingerprintId.c_str());
    OutputManager::setTextSection("Registered Successfully!");
    delay(3000);
    OutputManager::clearTextSection();
}
