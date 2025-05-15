#include "FingerprintManager.hpp"
#include "configs.hpp"
#include "Utils/Log.hpp"

// FingerprintManager::FingerprintManager(HardwareSerial &fingerSerial) : finger(new Adafruit_Fingerprint(&fingerSerial))
// {
    
// }

FingerprintManager::FingerprintManager(HardwareSerial &fingerSerial, DatabaseManager &databaseManager) : finger(new Adafruit_Fingerprint(&fingerSerial)), database(&databaseManager)
{
}

FingerprintManager::FingerprintManager(const FingerprintManager &other)
{
    if (other.finger)
    {
        this->finger = new Adafruit_Fingerprint(*other.finger);
    }
    else
    {
        this->finger = nullptr;
    }

    this->database = other.database;
}

FingerprintManager &FingerprintManager::operator=(const FingerprintManager &other)
{
    if (this != &other)
    {
        delete finger;
        if (other.finger)
        {
            finger = new Adafruit_Fingerprint(*other.finger);
        }
        else
        {
            finger = nullptr;
        }

        this->database = other.database;
    }
    return *this;
}

FingerprintManager::~FingerprintManager()
{
    delete finger;
}

bool FingerprintManager::initialize()
{
    // Try to connect to the sensor
    const int maxRetries = 10;
    for (int i = 0; i < maxRetries; ++i) {
        if (!IsSensorConnected()) {
            vTaskDelay(pdMS_TO_TICKS(250)); // Non-blocking delay
        }
        finger->getParameters();
        LOGD("Sensor initialized");
        LOGD("Sensor contains %d templates", GetTemplateCount());
        return true;
    }

    LOGE("Fingerprint sensor initialization failed after %d retries", maxRetries);

    return false;
}

bool FingerprintManager::IsSensorConnected()
{
    return finger->verifyPassword();
}

const bool FingerprintManager::AddFinger(const int id)
{
    if(id < 1 || id > 1000){
        LOGE("ID is invalid");
        return false;
    }

    uint8_t p = -1;
    for(uint8_t i = 0; i < 2; i++){
        LOGD("#%u Scan model for ID %d",i,id);
        if(i == 0)
            LOGI("Place Finger!");
        do{
            p = finger->getImage();
            if(IsOperationError(p)){
                LOGI("Remove Finger !");
                IsFingerRemoved();
                return false;
            }
        } while(p != FINGERPRINT_OK);

        p = finger->image2Tz(i+1);
        if(IsOperationError(p)){
            LOGI("Remove Finger !");
            IsFingerRemoved();
            return false;
        }
        LOGI("Remove Finger !");
        IsFingerRemoved();
        
        if(i == 0){
            LOGI("Place same finger again");
        }
    }

    p = finger->createModel();
    if(IsOperationError(p)){
        LOGI("Remove Finger !");
        IsFingerRemoved();
        return false;
    }

    p = finger->storeModel(id);
    if(IsOperationError(p)){
        LOGI("Remove Finger !");
        IsFingerRemoved();
        return false;
    }
    LOGD("Stored!");

    return true;
}



const bool FingerprintManager::GetFingerID(int *result)
{
    uint8_t p = finger->getImage();
    if(IsOperationError(p) || p == FINGERPRINT_NOFINGER){
        return false;
    }
    LOGD("Get Fingerprint ID");

    // OK success!
    p = finger->image2Tz();
    if(IsOperationError(p)){
        IsFingerRemoved();
        return false;
    }
    
    // OK converted!
    p = finger->fingerFastSearch();
    

    if(IsOperationError(p)){
        IsFingerRemoved();
        return false;
    }
    *result = finger->fingerID;
    LOGD("Fingerprint match to ID %d with confidence of %d !",finger->fingerID,finger->confidence);

    IsFingerRemoved();
    return true;
}

const bool FingerprintManager::DeleteFinger(const int id)
{
    uint8_t p = finger->deleteModel(id);
    if(IsOperationError(p)){
        return false;
    }
    LOGD("Deleted!");
    return true;
}

const bool FingerprintManager::ResetData()
{
    return !IsOperationError(finger->emptyDatabase());
}

void FingerprintManager::setColorRed()
{
    finger->LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
}

void FingerprintManager::setColorBlue()
{
    finger->LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
}

const bool FingerprintManager::IsFingerRemoved()
{
    LOGD("Remove Finger !");
    setColorRed();
    uint8_t p = finger->getImage();
    while(p != FINGERPRINT_NOFINGER){
        p = finger->getImage();
    }
    setColorBlue();
    return true;
}

const bool FingerprintManager::IsOperationError(const uint8_t operationValue)
{
    switch (operationValue) {
        case FINGERPRINT_OK:
        case FINGERPRINT_NOFINGER:{
            return false;
        }
        case FINGERPRINT_PACKETRECIEVEERR:
            LOGE("Communication error");
            return true;
        case FINGERPRINT_IMAGEFAIL:
            LOGE("Imaging error");
            return true;
        case FINGERPRINT_IMAGEMESS:
            LOGE("Image too messy");
            return true;
        case FINGERPRINT_FEATUREFAIL:
            LOGE("Could not find fingerprint features");
            return true;
        case FINGERPRINT_INVALIDIMAGE:
            LOGE("Invalid Image Data");
            return true;
        case FINGERPRINT_NOTFOUND:
            LOGE("Did not find a match");
            return true;
        case FINGERPRINT_ENROLLMISMATCH:
            LOGE("Fingerprints did not match");
            return true;
        case FINGERPRINT_BADLOCATION:
            LOGE("Could not store in that location");
            return true;
        case FINGERPRINT_FLASHERR:
            LOGE("Error writing to flash");
            return true;
        default:
            LOGE("Unknown error");
            return true;
    }
}

const int FingerprintManager::GetTemplateCount()
{
    uint8_t p = finger->getTemplateCount();
    return finger->templateCount;
}

const int FingerprintManager::GetCapacity()
{
    return finger->capacity;
}
