#pragma once
#include <Adafruit_Fingerprint.h>
#include "DatabaseManager.hpp"

class FingerprintManager
{
protected:
    Adafruit_Fingerprint *finger;
    DatabaseManager *database;

    const bool IsFingerRemoved();
    const bool IsOperationError(const uint8_t operationValue);

    const int GetTemplateCount();
    public:
    // FingerprintManager(HardwareSerial &fingerSerial);
    FingerprintManager(HardwareSerial &fingerSerial, DatabaseManager &databaseManager);
    FingerprintManager(const FingerprintManager &other);
    FingerprintManager &operator=(const FingerprintManager &other);
    ~FingerprintManager();
    
    bool initialize();
    bool IsSensorConnected();
    const int GetCapacity();
    const bool AddFinger(const int id);
    const bool GetFingerID(int *result);
    const bool DeleteFinger(const int id);
    const bool ResetData();

    void setColorRed();
    void setColorBlue();
};
