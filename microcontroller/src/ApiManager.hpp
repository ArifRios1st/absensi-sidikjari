#pragma once

#include <cJSON.h>
#include "Arduino.h"
#include "configs.hpp"

class ApiManager
{
public:
    ApiManager(const String &url, const String &authToken);
    bool UpdateKapasitas(int kapasitas);
    bool SetFingerprintScanned(int user_id,int fingerprint_id,String &message);
    bool SyncAbsenceDatas(cJSON *data, cJSON **result,String &message);
    
    enum HTTP_METHOD {
        GET,
        POST,
        PUT,
        PATCH
    };



protected:
    const String *url;
    const String *authToken;

    bool sendPostRequest(const String &endpoint, const String &data, String *result);
    bool sendPutRequest(const String &endpoint, const String &data, String *result);
    bool sendGetRequest(const String &endpoint, String *result);

    bool sendRequest(HTTP_METHOD method, const String &endpoint, String *result, const String *data = nullptr);
};