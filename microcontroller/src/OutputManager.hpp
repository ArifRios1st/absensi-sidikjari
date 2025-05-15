#pragma once
#include "configs.hpp"
#include <TFT_eSPI.h>
#include <SPI.h>


class OutputManager
{
public:
    static void initialize();

    static void drawWifiIcon();
    static void drawCloudIcon(bool isSync = false);
    static void drawClock();
    static void drawTextSection(const String& text);
    static void drawStatusBar();

    static void RenderDisplay();

    static void drawInputNumber(const String& label,bool isPin, int maxLength, const String& text);

    static void drawConfirm(const String& label);

    static void clearTextSection();
    static void setTextSection(const String& text);
    static void setTextSection(const char *format, ...);

private:
    static TFT_eSPI tft;
    static SPIClass tftSPI;

    static SemaphoreHandle_t mutex;

    static int getWiFiSignalLevel();
    static void getFormattedTime(String *time,String *date);

    static void drawScaledBitmap(int x, int y, const uint8_t *bitmap, int w, int h, int scale, uint16_t color);
    static void drawString(const String& string, int32_t x, int32_t y,uint8_t size,uint16_t fgcolor = TFT_WHITE, uint16_t bgcolor = TFT_BLACK, const GFXfont *f = (const GFXfont *)__null, bool bgfill = true);
    static void drawCenterString(const String& string,int32_t y,uint8_t size,uint16_t fgcolor = TFT_WHITE, uint16_t bgcolor = TFT_BLACK, const GFXfont *f = (const GFXfont *)__null, bool bgfill = true);

    static void updateDisplay(void *pvParameters);

    static int getCenterX(const String &text);

};