#include "OutputManager.hpp"
#include "icons.hpp"
#include <WiFi.h>
#include <time.h>

TFT_eSPI OutputManager::tft = TFT_eSPI(TFT_WIDTH, TFT_HEIGHT);
SPIClass OutputManager::tftSPI = SPIClass(VSPI);
SemaphoreHandle_t OutputManager::mutex;

#define TEXT_SECTION_X 3
#define TEXT_SECTION_Y 66
#define TEXT_SECTION_SIZE 1
#define TEXT_SECTION_MAX_LINE 7

#define VISIBLE_MENU_ITEMS 4

void OutputManager::initialize() 
{
    mutex = xSemaphoreCreateMutex();
    assert(mutex != NULL); // Ensure it's created successfully

    configTime(7 * 3600, 0, "pool.ntp.org"); // Use NTP server, set timezone offset to UTC+7
    setenv("TZ", "Asia/Jakarta", 1); // Set time zone to Jakarta (WIB)
    tzset();

    tftSPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);  
    tft.init();
    tft.fillScreen(TFT_BLACK);
    tft.setRotation(0);
    drawScaledBitmap( 24, 24, rntIcon, 64, 64, 3, TFT_WHITE);
    delay(1000);
    tft.fillScreen(TFT_BLACK);

    drawStatusBar();

    xTaskCreatePinnedToCore(updateDisplay, "updateDisplayTask", 4096, NULL, 1, NULL, 1);
}

void OutputManager::drawWifiIcon()
{
    int signalLevel = getWiFiSignalLevel();
    const unsigned char* wifiIcon;

    switch (signalLevel) {
        case 5: wifiIcon = image_wifi_5_bars_bits; break;
        case 4: wifiIcon = image_wifi_4_bars_bits; break;
        case 3: wifiIcon = image_wifi_3_bars_bits; break;
        case 2: wifiIcon = image_wifi_2_bars_bits; break;
        default: wifiIcon = image_wifi_not_connected_bits; break;
    }

    tft.fillRect(215, 6, image_wifi_width, image_wifi_height, TFT_BLACK);
    tft.drawBitmap(215, 6, wifiIcon, image_wifi_width, image_wifi_height, TFT_WHITE);
}

void OutputManager::drawCloudIcon(bool isSync)
{
    tft.drawBitmap(6, 6, (isSync) ? image_cloud_sync_bits : image_cloud_bits, image_cloud_width, image_cloud_height, TFT_WHITE);
}

void OutputManager::drawClock()
{
    String time, date;
    getFormattedTime(&time, &date);

    tft.setFreeFont();
    drawCenterString(time, 0, 4, TFT_WHITE, TFT_BLACK);

    drawCenterString(date, 35, 1, TFT_WHITE, TFT_BLACK,&FreeMonoBold12pt7b);
}

void OutputManager::drawTextSection(const String& text)
{
    // Wrap text into multiple lines
    String lines[TEXT_SECTION_MAX_LINE];
    int lineCount = 0;

    String currentLine, word;
    for (int i = 0; i < text.length(); i++) {
        char c = text[i];
        if (c == '\n' || c == ' ' || i == text.length() - 1) {
            if (i == text.length() - 1) word += c;

            if (c == '\n' || tft.textWidth(currentLine + " " + word) > (TFT_WIDTH - (TEXT_SECTION_X * 2))) {
                if (!currentLine.isEmpty() && lineCount < TEXT_SECTION_MAX_LINE) { // ✅ Skip empty lines
                    lines[lineCount++] = currentLine;
                }
                currentLine = word;
            } else {
                if (!currentLine.isEmpty()) currentLine += " ";
                currentLine += word;
            }
            word = "";

            if (c == '\n' && !currentLine.isEmpty() && lineCount < TEXT_SECTION_MAX_LINE) { // ✅ Skip empty lines
                lines[lineCount++] = currentLine;
                currentLine = "";
            }
        } else {
            word += c;
        }
    }

    if (!currentLine.isEmpty() && lineCount < TEXT_SECTION_MAX_LINE) {
        lines[lineCount++] = currentLine;
    }

    for (int i = 0; i < lineCount; i++) {
        drawCenterString(lines[i], TEXT_SECTION_Y + (i * tft.fontHeight()), TEXT_SECTION_SIZE, TFT_WHITE, TFT_BLACK, &FreeMonoBold12pt7b);
    }
}

void OutputManager::drawStatusBar()
{
    tft.drawLine(0, 32, TFT_WIDTH, 32, TFT_WHITE);
    drawWifiIcon();
    drawClock();
}

void OutputManager::RenderDisplay()
{
    drawStatusBar();
}

void OutputManager::drawInputNumber(const String& label,bool isPin, int maxLength, const String& text)
{
    if (mutex != NULL)
        xSemaphoreTake(mutex, portMAX_DELAY);

    clearTextSection();
    drawCenterString(label,78,2,0xFFEA,TFT_BLACK);

    // Draw underline
    String underline = "";
    for (size_t i = 0; i < maxLength; i++) {
        underline += "_";
    }
    drawCenterString(underline,138,5,TFT_WHITE);
    int startX = getCenterX(underline);
    int totalCharWidth = tft.textWidth(String(underline[0]));

    // tft.drawBitmap(43+(30*(text.length()-1)), 148, image_arrow_down_bits, 5, 7, TFT_WHITE);
    for (size_t i = 0; i < text.length(); i++){
        int xPos = isPin ? (startX + i * totalCharWidth) : (startX + (maxLength - text.length() + i) * totalCharWidth);
        String show = (isPin) ? String("*") : String(text[i]);
        drawString(show,xPos,128,5,TFT_WHITE,TFT_BLACK);
    }

    if (mutex != NULL)
        xSemaphoreGive(mutex);
}

void OutputManager::drawConfirm(const String &label)
{
    drawCenterString("Confirm to",79,2,0xFFEA);
    drawCenterString(label+"?",104,2,0xFFEA);
    drawCenterString("D = Yes", 154,2,0x57EA);
    drawCenterString("A = No", 175,2,0xA800);
}

void OutputManager::clearTextSection()
{
    tft.fillRect(0, 60, TFT_WIDTH, 180, TFT_BLACK);
}

void OutputManager::setTextSection(const String &text)
{
    if (mutex != NULL)
        xSemaphoreTake(mutex, portMAX_DELAY);

    clearTextSection();
    drawTextSection(text);

    if (mutex != NULL)
        xSemaphoreGive(mutex);
}

void OutputManager::setTextSection(const char *format, ...)
{
    char buffer[128]; // Adjust size as needed
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    setTextSection(String(buffer));
}

int OutputManager::getWiFiSignalLevel()
{
    if (WiFi.status() != WL_CONNECTED) {
        return 0; // Not connected
    }
    
    int rssi = WiFi.RSSI();
    if (rssi > -55) return 5; // Excellent
    if (rssi > -65) return 4; // Good
    if (rssi > -75) return 3; // Fair
    if (rssi > -85) return 2; // Weak
    return 1; // Very weak
}

void OutputManager::getFormattedTime(String *time,String *date)
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        *time = "--:--";       // Fallback if time isn't fetched
        *date = "---, --- --";  // Fallback if date isn't fetched
        return;
    }

    const char* hari[] = {"Ming", "Sen", "Sel", "Rab", "Kam", "Jum", "Sab"};
    const char* bulan[] = {"Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agu", "Sep", "Okt", "Nov", "Des"};

    char timeStr[6];  // HH:MM
    char dateStr[12];

    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    snprintf(dateStr, sizeof(dateStr), "%s, %s %d", hari[timeinfo.tm_wday], bulan[timeinfo.tm_mon], timeinfo.tm_mday);
    
    *time = String(timeStr);
    *date = String(dateStr);
}

void OutputManager::drawScaledBitmap(int x, int y, const uint8_t *bitmap, int w, int h, int scale, uint16_t color)
{
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            if (pgm_read_byte(bitmap + (j * w + i) / 8) & (0x80 >> (i % 8))) {
                // Draw a "scaled" pixel block
                tft.fillRect(x + i * scale, y + j * scale, scale, scale, color);
            }
        }
    }
}

void OutputManager::drawString(const String& string, int32_t x, int32_t y,uint8_t size,uint16_t fgcolor, uint16_t bgcolor, const GFXfont *f, bool bgfill)
{
    tft.setTextColor(fgcolor, bgcolor,bgfill);
    tft.setTextSize(size);
    tft.setFreeFont(f);
    tft.drawString(string, x, y);
}

void OutputManager::drawCenterString(const String &string, int32_t y, uint8_t size, uint16_t fgcolor, uint16_t bgcolor, const GFXfont *f, bool bgfill)
{
    tft.setTextSize(size);
    tft.setFreeFont(f);
    int x = getCenterX(string);
    drawString(string,x,y,size,fgcolor,bgcolor,f,bgfill);
}

void OutputManager::updateDisplay(void *pvParameters)
{
    while (true)
    {
        if (mutex != NULL) // Ensure mutex exists
        {
            xSemaphoreTake(mutex, portMAX_DELAY);
            drawStatusBar();
            xSemaphoreGive(mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int OutputManager::getCenterX(const String &text)
{
    return (TFT_WIDTH - tft.textWidth(text)) / 2;
}
