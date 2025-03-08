/* ***********************************************

by DrNeurosurg 2024

Arduino IDE Version: 2.3.2

Libraries used (Arduino):

LovyanGFX by lovyan03
Version 1.1.12

lvgl by kisvegabor
Version 9.1.0


Board: ESP32S3 Dev Module

Board Settings:

USB CDC On Boot: "Enabled"
CPU Frequency: "240MHz (WiFi)"
Core Debug Level: "None"
USB DFU On Boot: "Disabled"
Erase All Flash Before Sketch Upload: "Disabled"
Events Run On: "Core 1"
Flash Mode: "QIO 80MHz"
Flash Size: "8MB (64Mb)"
JTAG Adapter: "Disabled"
Arduino Runs On: "Core 1"
USB Firmware MSC On Boot: "Disabled"
Partition Scheme: "8M with spiffs (3MB APP/1.5MB SPIFFS)"
PSRAM: "QSPI PSRAM"
Upload Mode: "UARTO / Hardware CDC"
Upload Speed: "921600"
USB Mode: "Hardware CDC and JTAG"

*********************************************** */
#ifndef WT32_SC01_PLUS_H
#define WT32_SC01_PLUS_H

#pragma once

#include <Arduino.h>
#include <LovyanGFX.hpp>

//#define SCR 30
#define LGFX_USE_V1

#define WT_USE_LVGL

#ifdef WT_USE_LVGL
  #include <lvgl.h>
  #include "esp_heap_caps.h"
#endif  // WT_USE_LVGL

/* Change to your screen resolution */

// default: LANDSCAPE
#ifdef TFT_PORTRAIT
    #define TFT_HOR_RES 320
    #define TFT_VER_RES 480
#else
    #define TFT_HOR_RES 480
    #define TFT_VER_RES 320
#endif


// SparkOfCode: commented out because I2S pins are defined in lib/AudioTask/src/audiotask.cpp
/* I2S Pins */
/*#define WT_I2S_BCK 36
#define WT_I2S_WS  35
#define WT_I2S_DATA 37 */

/* I2S Pins for external DAC (e.g. UDA1344) */
/*#define WT_I2S_BCK 13 //white
#define WT_I2S_WS  11 //blue
#define WT_I2S_DATA 12 //green*/

#ifdef WT_USE_SDCARD
  /* SD-Card Pins */
  #define WT_SD_CS_PIN 41
  #define WT_SD_MOSI_PIN 40 //MOSI
  #define WT_SD_CLK_PIN  39
  #define WT_SD_MISO_PIN 38 //MISO

  #include <SPI.h>
  #include <SD.h>
    SPIClass hspi = SPIClass(HSPI);
    #define SD_SPI_FREQ 1000000
#endif

class LGFX : public lgfx::LGFX_Device
{

  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX();
};

#ifdef WT_USE_LVGL
// /*Read the touchpad*/
void  touchPadRead(lv_indev_t * indev, lv_indev_data_t * data);

/*const unsigned int lvBufferSize = TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8);
// uint8_t lvBuffer[lvBufferSize];
// uint8_t lvBuffer2[lvBufferSize];
uint8_t *lvBuffer = (uint8_t *)heap_caps_malloc(lvBufferSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
uint8_t *lvBuffer2 = (uint8_t *)heap_caps_malloc(lvBufferSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA);
*/

#endif // WT_USE_LVGL

void init_display();

#endif