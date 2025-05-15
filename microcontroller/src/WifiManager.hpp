#pragma once
#include <ESPAsyncWebServer.h>
#include "DatabaseManager.hpp"

#include "configs.hpp"

class WifiManager
{
public:
    String wifi_ssid = DEFAULT_WIFI_SSID;
    String wifi_password = DEFAULT_WIFI_PASSWORD;
    
    String ap_ssid = DEFAULT_AP_SSID;
    String ap_password = DEFAULT_AP_PASSWORD;
    bool ap_hidden = DEFAULT_AP_HIDDEN;

    String url = DEFAULT_API_URL;
    String token = DEFAULT_API_TOKEN;
    String admin_pin = DEFAULT_ADMIN_PIN;

public:
    WifiManager(DatabaseManager &database);
    ~WifiManager();
    bool initialize();

    bool isConnected();
    void connect();
    void disconnect();

    void startAP();
    void stopAP();

    void syncSettings();

protected:
    void handleSettingForm(AsyncWebServerRequest *request);
    void handleSettingSubmitForm(AsyncWebServerRequest *request);

protected:
    AsyncWebServer server;
    DatabaseManager *database = nullptr;
};