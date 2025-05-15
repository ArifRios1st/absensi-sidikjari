#include "ApiManager.hpp"

#include <HTTPClient.h>
#include "Utils/Log.hpp"

ApiManager::ApiManager(const String &url, const String &authToken) : url(&url), authToken(&authToken)
{
}

bool ApiManager::UpdateKapasitas(int kapasitas)
{
    String data = "kapasitas=" + String(kapasitas);
    if(sendPostRequest(F("/api/v1/device"),data,nullptr)){
        LOGD("Capacity Updated !");
        return true;
    }
    return false;
}

bool ApiManager::SetFingerprintScanned(int user_id, int fingerprint_id, String &message)
{
    String data;
    String resultString;

    bool result = sendPutRequest("/api/v1/guru/"+String(user_id)+"/fingerprint/"+String(fingerprint_id),data,&resultString);
    cJSON *resultJson = cJSON_Parse(resultString.c_str());
    message = cJSON_GetObjectItemCaseSensitive(resultJson,"message")->valuestring;
    cJSON_Delete(resultJson);
    
    LOGD("SetApiScanned: %s",message.c_str());

    return result;
}

bool ApiManager::SyncAbsenceDatas(cJSON *data, cJSON **result, String &message)
{
    if (!data || !result) {
        message = "No data to sync or null result pointer.";
        return false;
    }

    *result = nullptr;

    // Convert cJSON to String
    char *jsonStr = cJSON_PrintUnformatted(data);
    if (!jsonStr) {
        message = "Failed to encode JSON.";
        return false;
    }

    String payload(jsonStr);
    free(jsonStr); // Clean up raw C string

    String response;
    bool success = sendPostRequest("/api/v1/absensi", payload, &response);

    if (!success) {
        message = "Failed to send request. Server response: " + response;
        return false;
    }

    // Parse the server's JSON response
    cJSON *parsed = cJSON_Parse(response.c_str());
    if (!parsed || !cJSON_IsArray(parsed)) {
        message = "Invalid or unexpected server response format.";
        if (parsed) cJSON_Delete(parsed);
        return false;
    }

    *result = parsed; // Pass ownership to the caller
    message = "Sync response parsed successfully.";
    return true;
}

bool ApiManager::sendRequest(HTTP_METHOD method, const String &endpoint, String *result, const String *data)
{
    HTTPClient http;
    http.begin(*url + endpoint);
    http.addHeader(F("Content-Type"), F("application/x-www-form-urlencoded"));
    http.addHeader(F("Accept"), F("application/json"));
    http.addHeader(F("Authorization"), "Bearer " + *authToken);

    int httpCode;
    switch (method)
    {
    case HTTP_METHOD::GET:
        httpCode = http.GET();
        break;
    case HTTP_METHOD::POST:
        httpCode = http.POST(*data);
        break;
    case HTTP_METHOD::PUT:
        httpCode = http.PUT(*data);
        break;
    case HTTP_METHOD::PATCH:
        httpCode = http.PATCH(*data);
        break;
    
    default:
        return false;
    }

    String res = http.getString();
    if (result) {
        *result = res;
    }
    http.end();

    switch ((t_http_codes)httpCode)
    {
    case HTTP_CODE_OK:
    case HTTP_CODE_CREATED:
    case HTTP_CODE_ACCEPTED:
        LOGD("HTTP %d : %s", httpCode, res.c_str());
        return true;
    default:
        LOGE("HTTP %d %s : %s", httpCode, http.errorToString(httpCode).c_str(), res.c_str());
        return false;
    }
    
}

bool ApiManager::sendPostRequest(const String &endpoint, const String &data, String *result)
{
    return sendRequest(HTTP_METHOD::POST,endpoint, result, &data);
}

bool ApiManager::sendPutRequest(const String &endpoint, const String &data, String *result)
{
    return sendRequest(HTTP_METHOD::PUT,endpoint, result, &data);
}

bool ApiManager::sendGetRequest(const String &endpoint, String *result)
{
    return sendRequest(HTTP_METHOD::GET,endpoint, result,nullptr);
}
