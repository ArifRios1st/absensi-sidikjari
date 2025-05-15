#pragma once
#include "OutputManager.hpp"

#define LOG(tag, format, ...) Serial.printf_P(PSTR("[%s] "), tag); Serial.printf_P(PSTR(format), ##__VA_ARGS__); Serial.println()
#define LOGD(format, ...) LOG(F("DEBUG"), format, ##__VA_ARGS__)
#define LOGI(format, ...) LOG(F("INFO"), format, ##__VA_ARGS__); \
OutputManager::setTextSection(format, ##__VA_ARGS__)
#define LOGW(format, ...) LOG(F("WARNING"), format, ##__VA_ARGS__)
#define LOGE(format, ...) LOG(F("ERROR"), format, ##__VA_ARGS__)
