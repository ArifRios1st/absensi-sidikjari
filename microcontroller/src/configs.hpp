#pragma once

//************************************************************************
/* Serial Settings. */
#define FINGERPRINT_SERIAL      57600
#define MONIITOR_SERIAL         115200

//************************************************************************
/* display Settings. */
#define USER_SETUP_ID           240
#define ST7789_DRIVER
#define TFT_WIDTH               240
#define TFT_HEIGHT              240

#define TFT_MISO                -1  // Not Connected
#define TFT_MOSI                21  // SDA (HSPI MOSI)
#define TFT_SCLK                22  // SCL (HSPI SCK)
#define TFT_RST                 4  // Reset pin
#define TFT_DC                  2  // Data/Command
#define TFT_CS                  -1  // Not Connected

#define DISABLE_ALL_LIBRARY_WARNINGS

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT

// Define the SPI clock frequency, this affects the graphics rendering speed. Too
// fast and the TFT driver will not keep up and display corruption appears.
// With an ILI9341 display 40MHz works OK, 80MHz sometimes fails
// With a ST7735 display more than 27MHz may not work (spurious pixels and lines)
// With an ILI9163 display 27 MHz works OK.
#define SPI_FREQUENCY  27000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000

//************************************************************************
/* WIFI Default Credentials. */
#define DEFAULT_WIFI_SSID       "AKB"
#define DEFAULT_WIFI_PASSWORD   "Sdaisiyah1@22"
#define DEFAULT_AP_SSID         "Absensi Biometrik"
#define DEFAULT_AP_PASSWORD     "1234567890"
#define DEFAULT_AP_HIDDEN       1

//************************************************************************
/* DEVICE Default Data. */
#define DEFAULT_API_URL         "http://192.168.10.1:8000"
#define DEFAULT_API_TOKEN       "1234567890"
#define DEFAULT_ADMIN_PIN       "123456"

//************************************************************************
/* SDCard Adapter Pins. */
#define SDCARD_CS               5
#define SDCARD_SCK              18
#define SDCARD_MOSI             23
#define SDCARD_MISO             19

//************************************************************************
/* Database. */
#define SDCARD_BASEPATH         "/sd"
#define DATABASE_FILE           "/db_sidikjari.db"

//************************************************************************
/* Keypad. */
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

//************************************************************************
/* Api Endpoint. */