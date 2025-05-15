#include "WifiManager.hpp"
#include "Utils/Log.hpp"

constexpr char SETTING_HTML[] PROGMEM = R"rawliteral(
<html><form action='/save' method='POST'>
<h1>Wi-Fi</h1>
SSID:<input type='text' name='ssid' value='%s'><br />
Pass:<input type='text' name='password' value='%s'><br />
AP SSID:<input type='text' name='ap_ssid' value='%s'><br />
AP Pass:<input type='text' name='ap_password' value='%s'><br />
<label><input type='checkbox' name='ap_hidden' %s>Hidden</label><br />
<h1>Device</h1>
URL:<input type='text' name='url' value='%s'><br />
Token:<input type='text' name='token' value='%s'><br />
PIN:<input type='text' name='admin_pin' value='%s'><br /><br />
<input type='submit' value='Save'><br />
</form></html>
)rawliteral";


WifiManager::WifiManager(DatabaseManager &database) : 
server(80),
database(&database)
{
    
}

WifiManager::~WifiManager()
{
    server.end();
}

bool WifiManager::initialize()
{
    WiFi.mode(WIFI_MODE_APSTA);

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, F("text/plain"), F("Not Found<br/><a href=\"/\">Home</a>"));
    });

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        this->handleSettingForm(request);
    });

    server.on("/save",HTTP_POST,[this](AsyncWebServerRequest *request){
        this->handleSettingSubmitForm(request);
    });

    server.begin();

    LOGD("WiFi initialized");

    return true;
}

bool WifiManager::isConnected()
{
    return WiFi.isConnected();
}

void WifiManager::connect()
{
    if(wifi_ssid.isEmpty()){
        LOGI("SSID is Empty !");
        return;
    }
    disconnect();
    LOGI("Connecting to\n%s", wifi_ssid.c_str());
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (isConnected()) {
        LOGI("Connected to %s\nIP address: %s", wifi_ssid.c_str(),WiFi.localIP().toString().c_str());
    } else {
        LOGE("Failed to connect");
    }
}

void WifiManager::disconnect()
{
    WiFi.disconnect();
    LOGI("Disconnected from WiFi");
}

void WifiManager::startAP()
{
    stopAP();
    // Start the Access Point (AP)
    WiFi.softAP(ap_ssid.c_str(), ap_password.c_str(),1,ap_hidden);
    LOGI("AP Started.\nSSID: %s %s\nIP address: %s",ap_ssid.c_str(),(ap_hidden ? "(H)" : ""),WiFi.softAPIP().toString().c_str());
}

void WifiManager::stopAP()
{
    WiFi.softAPdisconnect();
    LOGI("AP Stopped.");
}

void WifiManager::syncSettings()
{
    cJSON *data = nullptr;
    if (database->getAllSettings(&data) != SQLITE_OK) {
        return;
    }

    wifi_ssid = cJSON_GetObjectItem(data, "ssid") ? cJSON_GetObjectItem(data, "ssid")->valuestring : "";
    wifi_password = cJSON_GetObjectItem(data, "password") ? cJSON_GetObjectItem(data, "password")->valuestring : "";
    ap_ssid = cJSON_GetObjectItem(data, "ap_ssid") ? cJSON_GetObjectItem(data, "ap_ssid")->valuestring : "";
    ap_password = cJSON_GetObjectItem(data, "ap_password") ? cJSON_GetObjectItem(data, "ap_password")->valuestring : "";
    ap_hidden = cJSON_GetObjectItem(data, "ap_hidden") ? cJSON_IsTrue(cJSON_GetObjectItem(data, "ap_hidden")) : true;

    url = cJSON_GetObjectItem(data, "url") ? cJSON_GetObjectItem(data, "url")->valuestring : "";
    token = cJSON_GetObjectItem(data, "token") ? cJSON_GetObjectItem(data, "token")->valuestring : "";
    admin_pin = cJSON_GetObjectItem(data, "admin_pin") ? cJSON_GetObjectItem(data, "admin_pin")->valuestring : "";
}

void WifiManager::handleSettingForm(AsyncWebServerRequest *request)
{
    cJSON *data = nullptr;
    
    if (database->getAllSettings(&data) != SQLITE_OK || !data) {
        request->send(500, F("text/plain"), F("Failed to load settings<br/><a href=\"/\">Home</a>"));
        return;
    }

    // Ensure safe access to JSON fields
    const char *wifi_ssid = cJSON_GetObjectItem(data, "ssid") ? cJSON_GetObjectItem(data, "ssid")->valuestring : "";
    const char *wifi_password = cJSON_GetObjectItem(data, "password") ? cJSON_GetObjectItem(data, "password")->valuestring : "";
    const char *ap_ssid = cJSON_GetObjectItem(data, "ap_ssid") ? cJSON_GetObjectItem(data, "ap_ssid")->valuestring : "";
    const char *ap_password = cJSON_GetObjectItem(data, "ap_password") ? cJSON_GetObjectItem(data, "ap_password")->valuestring : "";
    bool ap_hidden = cJSON_GetObjectItem(data, "ap_hidden") ? cJSON_IsTrue(cJSON_GetObjectItem(data, "ap_hidden")) : true;
    const char *url = cJSON_GetObjectItem(data, "url") ? cJSON_GetObjectItem(data, "url")->valuestring : "";
    const char *token = cJSON_GetObjectItem(data, "token") ? cJSON_GetObjectItem(data, "token")->valuestring : "";
    const char *admin_pin = cJSON_GetObjectItem(data, "admin_pin") ? cJSON_GetObjectItem(data, "admin_pin")->valuestring : "";

    String hidden = (ap_hidden) ? "checked" : "";

    // Dynamically allocate buffer for HTML response
    size_t buffer_size = snprintf(nullptr, 0, SETTING_HTML, 
                                  wifi_ssid, 
                                  wifi_password, 
                                  ap_ssid, 
                                  ap_password, 
                                  hidden.c_str(),
                                  url,
                                  token,
                                  admin_pin) + 1;

    char* buffer = (char*)malloc(buffer_size);
    if (!buffer) {
        request->send(500, F("text/plain"), F("Memory allocation failed<br/><a href=\"/\">Home</a>"));
        cJSON_Delete(data);
        return;
    }

    snprintf(buffer, buffer_size, SETTING_HTML, 
             wifi_ssid, 
             wifi_password, 
             ap_ssid, 
             ap_password, 
             hidden.c_str(),
             url,
             token,
             admin_pin);

    request->send(200, F("text/html"), buffer);

    free(buffer);
    cJSON_Delete(data);
}


void WifiManager::handleSettingSubmitForm(AsyncWebServerRequest *request)
{
    const char *requiredParams[] = {"ssid", "password", "ap_ssid", "ap_password", "url", "token", "admin_pin"};
    const size_t numParams = sizeof(requiredParams) / sizeof(requiredParams[0]);

    // Check if all required parameters exist
    for (size_t i = 0; i < numParams; i++) {
        if (!request->hasParam(requiredParams[i], true)) {
            request->send(400, "text/html", F("Missing parameters<br/><a href=\"/\">Home</a>"));
            return;
        }
    }

    // Retrieve parameters (POST method) into safe `String` storage
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();
    String ap_ssid = request->getParam("ap_ssid", true)->value();
    String ap_password = request->getParam("ap_password", true)->value();
    bool ap_hidden = request->hasParam("ap_hidden", true);
    String url = request->getParam("url", true)->value();
    String token = request->getParam("token", true)->value();
    String admin_pin = request->getParam("admin_pin", true)->value();

    // Validate input (avoid SQL injection, empty values, or excessive length)
    if (ssid.length() > 32 || password.length() > 64 ||
        ap_ssid.length() > 32 || ap_password.length() > 64 ||
        url.length() > 64 || token.length() > 16 ||
        admin_pin.length() != 6) {
        request->send(400, "text/html", F("Invalid input length<br/><a href=\"/\">Home</a>"));
        return;
    }

    // Ensure admin_pin is numeric
    for (size_t i = 0; i < admin_pin.length(); i++) {
        if (!isdigit(admin_pin[i])) {
            request->send(400, "text/html", F("Invalid admin PIN format<br/><a href=\"/\">Home</a>"));
            return;
        }
    }

    // Create cJSON object for database update
    cJSON *settings = cJSON_CreateObject();
    if (!settings) {
        request->send(500, F("text/html"), F("Memory allocation failed<br/><a href=\"/\">Home</a>"));
        return; // No need to `cJSON_Delete(settings)` since it was never allocated
    }

    cJSON_AddStringToObject(settings, "ssid", ssid.c_str());
    cJSON_AddStringToObject(settings, "password", password.c_str());
    cJSON_AddStringToObject(settings, "ap_ssid", ap_ssid.c_str());
    cJSON_AddStringToObject(settings, "ap_password", ap_password.c_str());
    cJSON_AddBoolToObject(settings, "ap_hidden", ap_hidden);
    cJSON_AddStringToObject(settings, "url", url.c_str());
    cJSON_AddStringToObject(settings, "token", token.c_str());
    cJSON_AddStringToObject(settings, "admin_pin", admin_pin.c_str());

    if (database->updateAllSettings(settings) != SQLITE_OK) {
        cJSON_Delete(settings);
        request->send(500, "text/html", F("Failed to update settings<br/><a href=\"/\">Home</a>"));
        return;
    }

    cJSON_Delete(settings);

    // Update class variables
    this->wifi_ssid = ssid;
    this->wifi_password = password;
    this->ap_ssid = ap_ssid;
    this->ap_password = ap_password;
    this->ap_hidden = ap_hidden;
    this->url = url;
    this->token = token;
    this->admin_pin = admin_pin;

    request->send(200, "text/html", F("Settings updated successfully<br/><a href=\"/\">Home</a>"));

    delay(1000);

    ESP.restart();
}

