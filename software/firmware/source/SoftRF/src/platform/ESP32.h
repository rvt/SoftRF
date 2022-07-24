/*
 * Platform_ESP32.h
 * Copyright (C) 2018-2022 Linar Yusupov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#if defined(ESP32)

#ifndef PLATFORM_ESP32_H
#define PLATFORM_ESP32_H

#include "sdkconfig.h"

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <WiFiClient.h>
#include <SPIFFS.h>

/* Maximum of tracked flying objects is now SoC-specific constant */
#define MAX_TRACKING_OBJECTS    8

#define DEFAULT_SOFTRF_MODEL    SOFTRF_MODEL_STANDALONE

#define SerialOutput            Serial
#define SoftwareSerial          HardwareSerial
#define Serial_GNSS_In          Serial1
#define Serial_GNSS_Out         Serial_GNSS_In

#if defined(CONFIG_IDF_TARGET_ESP32)
#define UATSerial               Serial2
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
#if ARDUINO_USB_CDC_ON_BOOT
#define UATSerial               Serial0
#undef  SerialOutput
#define SerialOutput            Serial0
#else
#define UATSerial               Serial
#endif /* ARDUINO_USB_CDC_ON_BOOT */
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#define UATSerial               Serial2
#if ARDUINO_USB_CDC_ON_BOOT
#undef  SerialOutput
#define SerialOutput            Serial0
#endif /* ARDUINO_USB_CDC_ON_BOOT */
#else
#error "This ESP32 family build variant is not supported!"
#endif /* CONFIG_IDF_TARGET_ESP32 */

#define EEPROM_commit()         EEPROM.commit()

#define isValidFix()            isValidGNSSFix()

#if defined(ESP_IDF_VERSION_MAJOR) && ESP_IDF_VERSION_MAJOR>=4
#define WIRE_FINI(bus)          { bus.end(); }
#else
#define WIRE_FINI(bus)          { } /* AC 1.0.x has no Wire.end() */
#endif

#define LED_STATE_ON            HIGH  // State when LED is litted

#if defined(CONFIG_IDF_TARGET_ESP32S3)
/* Adafruit_NeoPixel still has "flickering" issue of ESP32 caused by 1 ms scheduler */
#define USE_ADAFRUIT_NEO_LIBRARY

#else
/*
 * NeoPixelBus is already "flickering-free" on ESP32 (with I2S)
 * but the "Core" needs update onto the most recent one
 */
#define USE_NEOPIXELBUS_LIBRARY
#endif

#if defined(USE_NEOPIXELBUS_LIBRARY)
#include <NeoPixelBus.h>

#define uni_begin()             strip.Begin()
#define uni_show()              strip.Show()
#define uni_setPixelColor(i, c) strip.SetPixelColor(i, c)
#define uni_numPixels()         strip.PixelCount()
#define uni_Color(r,g,b)        RgbColor(r,g,b)
#define color_t                 RgbColor

extern NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip;
#else /* USE_ADAFRUIT_NEO_LIBRARY */
#include <Adafruit_NeoPixel.h>

#define uni_begin()             strip.begin()
#define uni_show()              strip.show()
#define uni_setPixelColor(i, c) strip.setPixelColor(i, c)
#define uni_numPixels()         strip.numPixels()
#define uni_Color(r,g,b)        strip.Color(r,g,b)
#define color_t                 uint32_t

extern Adafruit_NeoPixel strip;
#endif /* USE_NEOPIXELBUS_LIBRARY */

#define LEDC_CHANNEL_BUZZER     0
#define BACKLIGHT_CHANNEL       ((uint8_t)1)
#define LEDC_RESOLUTION_BUZZER  8

/* Peripherals */
#define SOC_GPIO_PIN_GNSS_RX    23
#define SOC_GPIO_PIN_GNSS_TX    12
#define SOC_GPIO_PIN_BATTERY    36
#if defined(CONFIG_IDF_TARGET_ESP32)
#define SOC_GPIO_PIN_LED        25
#else
#define SOC_GPIO_PIN_LED        7
#endif

#define SOC_GPIO_PIN_STATUS   (hw_info.model != SOFTRF_MODEL_PRIME_MK2 ?\
                                SOC_UNUSED_PIN :                        \
                                (hw_info.revision == 2 ?                \
                                  SOC_GPIO_PIN_TBEAM_LED_V02 :          \
                                  (hw_info.revision == 5 ?              \
                                    SOC_GPIO_PIN_TBEAM_LED_V05 :        \
                                    (hw_info.revision == 11 ?           \
                                      SOC_GPIO_PIN_TBEAM_LED_V11 :      \
                                      SOC_UNUSED_PIN))))

#define SOC_GPIO_PIN_GNSS_PPS (hw_info.model != SOFTRF_MODEL_PRIME_MK2 ?\
                                SOC_UNUSED_PIN :                        \
                                (hw_info.revision == 8 ?                \
                                  SOC_GPIO_PIN_TBEAM_V08_PPS :          \
                                  SOC_UNUSED_PIN))

#define SOC_GPIO_PIN_BUZZER   (hw_info.model == SOFTRF_MODEL_PRIME_MK2 ?\
                                SOC_UNUSED_PIN :                        \
                                (esp32_board == ESP32_DEVKIT ?          \
                                  13 : SOC_UNUSED_PIN))

/* SPI (does match Heltec & TTGO LoRa32 pins mapping) */
#define SOC_GPIO_PIN_MOSI       27
#define SOC_GPIO_PIN_MISO       19
#define SOC_GPIO_PIN_SCK        5
#define SOC_GPIO_PIN_SS         18

/* NRF905 */
#define SOC_GPIO_PIN_TXE        26
#define SOC_GPIO_PIN_CE         2
#define SOC_GPIO_PIN_PWR        14

/* SX1276 [RFM95W] (does match Heltec & TTGO LoRa32 pins mapping) */
#define SOC_GPIO_PIN_RST        14
#define SOC_GPIO_PIN_DIO0       26
#define SOC_GPIO_PIN_SDA        14
#define SOC_GPIO_PIN_SCL        2

/* TTGO T-BEAM section */
// GNSS module
#define SOC_GPIO_PIN_TBEAM_V05_RX       12
#define SOC_GPIO_PIN_TBEAM_V05_TX       15
#define SOC_GPIO_PIN_TBEAM_V08_RX       34
#define SOC_GPIO_PIN_TBEAM_V08_TX       12
#define SOC_GPIO_PIN_TBEAM_V08_PPS      37
// battery voltage
#define SOC_GPIO_PIN_TBEAM_V05_BATTERY  35
// status LED
#define SOC_GPIO_PIN_TBEAM_LED_V02      21
#define SOC_GPIO_PIN_TBEAM_LED_V05      14
#define SOC_GPIO_PIN_TBEAM_LED_V11      4
// button
#define SOC_GPIO_PIN_TBEAM_V05_BUTTON   39
#define SOC_GPIO_PIN_TBEAM_V08_BUTTON   38
// PMU
#define SOC_GPIO_PIN_TBEAM_V08_PMU_IRQ  35
// SX1276 RESET
#define SOC_GPIO_PIN_TBEAM_RF_RST_V02   SOC_UNUSED_PIN
#define SOC_GPIO_PIN_TBEAM_RF_RST_V05   23
// SX1262 BUSY
#define SOC_GPIO_PIN_TBEAM_RF_BUSY_V08  32
// 1st I2C bus on the T-Beam
#define SOC_GPIO_PIN_TBEAM_SDA          13
#define SOC_GPIO_PIN_TBEAM_SCL          2

/* TTGO T-Watch section */
// GNSS module
#define SOC_GPIO_PIN_TWATCH_RX          34
#define SOC_GPIO_PIN_TWATCH_TX          33
// button
#define SOC_GPIO_PIN_TWATCH_BUTTON      36

// PMU
#define SOC_GPIO_PIN_TWATCH_PMU_IRQ     35

// TFT
#define SOC_GPIO_PIN_TWATCH_TFT_MOSI    19
#define SOC_GPIO_PIN_TWATCH_TFT_MISO    SOC_UNUSED_PIN
#define SOC_GPIO_PIN_TWATCH_TFT_SCK     18
#define SOC_GPIO_PIN_TWATCH_TFT_SS      5
#define SOC_GPIO_PIN_TWATCH_TFT_DC      27
#define SOC_GPIO_PIN_TWATCH_TFT_RST     SOC_UNUSED_PIN
#define SOC_GPIO_PIN_TWATCH_TFT_BL      12

// 1st I2C bus on the T-Watch
#define SOC_GPIO_PIN_TWATCH_SEN_SDA     21
#define SOC_GPIO_PIN_TWATCH_SEN_SCL     22

/* TTGO T8 S2 section */
#define SOC_GPIO_PIN_T8_S2_CONS_RX      44
#define SOC_GPIO_PIN_T8_S2_CONS_TX      43

// GNSS module
#define SOC_GPIO_PIN_T8_S2_GNSS_RX      1
#define SOC_GPIO_PIN_T8_S2_GNSS_TX      2

// USB
#define SOC_GPIO_PIN_T8_S2_USB_DP       20
#define SOC_GPIO_PIN_T8_S2_USB_DN       19

// SD SPI
#define SOC_GPIO_PIN_T8_S2_MOSI         11
#define SOC_GPIO_PIN_T8_S2_MISO         13
#define SOC_GPIO_PIN_T8_S2_SCK          12
#define SOC_GPIO_PIN_T8_S2_SS           10

/* SX1276 */
#define SOC_GPIO_PIN_T8_S2_LORA_RST     5
#define SOC_GPIO_PIN_T8_S2_LORA_SS      6

// TFT
#define SOC_GPIO_PIN_T8_S2_TFT_MOSI     35
#define SOC_GPIO_PIN_T8_S2_TFT_MISO     SOC_UNUSED_PIN
#define SOC_GPIO_PIN_T8_S2_TFT_SCK      36
#define SOC_GPIO_PIN_T8_S2_TFT_SS       34
#define SOC_GPIO_PIN_T8_S2_TFT_DC       37
#define SOC_GPIO_PIN_T8_S2_TFT_RST      38
#define SOC_GPIO_PIN_T8_S2_TFT_BL       33

// button
#define SOC_GPIO_PIN_T8_S2_BUTTON       0 // 21

// I2C
#define SOC_GPIO_PIN_T8_S2_SDA          3
#define SOC_GPIO_PIN_T8_S2_SCL          4

// battery voltage
#define SOC_GPIO_PIN_T8_S2_BATTERY      9

// V3V Power enable
#define SOC_GPIO_PIN_T8_S2_PWR_EN       14

// 32768 Hz crystal
#define SOC_GPIO_PIN_T8_S2_XP           15
#define SOC_GPIO_PIN_T8_S2_XN           16

#define SOC_GPIO_PIN_TDISPLAY_S2_LED    39
#define SOC_GPIO_PIN_TDONGLE_S2_LED     39

#if !defined(CONFIG_IDF_TARGET_ESP32S2)
#define LV_HOR_RES                      (240) //Horizontal
#else
#define LV_HOR_RES                      (135) //Horizontal
#endif
#define LV_VER_RES                      (240) //vertical

/* ESP32-S3 DevKit section */
#define SOC_GPIO_PIN_S3_CONS_RX         44
#define SOC_GPIO_PIN_S3_CONS_TX         43

// GNSS module
#define SOC_GPIO_PIN_S3_GNSS_RX         18
#define SOC_GPIO_PIN_S3_GNSS_TX         17
#define SOC_GPIO_PIN_S3_GNSS_PPS        7

// USB
#define SOC_GPIO_PIN_S3_USB_DP          20
#define SOC_GPIO_PIN_S3_USB_DN          19

// TFT
#define SOC_GPIO_PIN_S3_TFT_MOSI        35
#define SOC_GPIO_PIN_S3_TFT_MISO        SOC_UNUSED_PIN
#define SOC_GPIO_PIN_S3_TFT_SCK         36
#define SOC_GPIO_PIN_S3_TFT_SS          34
#define SOC_GPIO_PIN_S3_TFT_DC          37
#define SOC_GPIO_PIN_S3_TFT_RST         38
#define SOC_GPIO_PIN_S3_TFT_BL          33

// SX1262 (HPD16A)
#define SOC_GPIO_PIN_S3_MOSI            35
#define SOC_GPIO_PIN_S3_MISO            40
#define SOC_GPIO_PIN_S3_SCK             36
#define SOC_GPIO_PIN_S3_SS              41
#define SOC_GPIO_PIN_S3_RST             38 /* shared with TFT RST (and/or I2C OLED RST) */
#define SOC_GPIO_PIN_S3_BUSY            42 /* shared with HPD13A DIO2 */
// SX1276 (HPD13A)
#define SOC_GPIO_PIN_S3_DIO0            47
#define SOC_GPIO_PIN_S3_DIO1            48
#define SOC_GPIO_PIN_S3_DIO2            42 /* shared with HPD16A BUSY */

// I2C
#define SOC_GPIO_PIN_S3_SDA             8
#define SOC_GPIO_PIN_S3_SCL             9

/* 2nd I2C bus (PMU) */
#define SOC_GPIO_PIN_S3_OLED_SDA        1
#define SOC_GPIO_PIN_S3_OLED_SCL        4

// microSD SPI
#define SOC_GPIO_PIN_S3_SD_MOSI         11
#define SOC_GPIO_PIN_S3_SD_MISO         13
#define SOC_GPIO_PIN_S3_SD_SCK          12
#define SOC_GPIO_PIN_S3_SD_SS           10

// button (BOOT)
#define SOC_GPIO_PIN_S3_BUTTON          0 // "strapping" pin (S)

// battery voltage (ADC)
#define SOC_GPIO_PIN_S3_BATTERY         2

// 32768 Hz crystal
#define SOC_GPIO_PIN_S3_XP              15
#define SOC_GPIO_PIN_S3_XN              16

// LEDs, active state - HIGH
#define SOC_GPIO_PIN_S3_STATUS          39
/* auxillary */
#define SOC_GPIO_PIN_S3_LED_RED         5
#define SOC_GPIO_PIN_S3_LED_GREEN       6
#define SOC_GPIO_PIN_S3_LED_BLUE        7
#define SOC_GPIO_PIN_S3_LED4            38

// Spare ESP32-S3 pins:
// 46(S), 45(S), 21, 26
// 3(S), 14
// More spare pins when the devkit's Red and Green LEDs are not in use:
// 5, 6
// Few more when TFT is not connected:
// 33, 34, 37
// two more when there is no 32768 Hz crystal:
// 15, 16
// One more when SX1262 (HPD16A) is the only radio option:
// 47

// Hardware pin definitions for TTGO LoRa V2 board
// with OLED SSD1306 0,96" I2C Display
#define TTGO_V2_OLED_PIN_RST    U8X8_PIN_NONE // connected to CPU RST/EN
#define TTGO_V2_OLED_PIN_SDA    21
#define TTGO_V2_OLED_PIN_SCL    22
#define TTGO_V2_PIN_GNSS_RX     34
#define TTGO_V2_PIN_GNSS_TX     12
#define TTGO_V2_PIN_GNSS_PPS    39

// Hardware pin definitions for Heltec and TTGO-V1 LoRa-32 Boards with OLED SSD1306 I2C Display
#define HELTEC_OLED_PIN_RST     U8X8_PIN_NONE // 16
#define HELTEC_OLED_PIN_SDA     4
#define HELTEC_OLED_PIN_SCL     15

extern WebServer server;

enum rst_reason {
  REASON_DEFAULT_RST      = 0,  /* normal startup by power on */
  REASON_WDT_RST          = 1,  /* hardware watch dog reset */
  REASON_EXCEPTION_RST    = 2,  /* exception reset, GPIO status won't change */
  REASON_SOFT_WDT_RST     = 3,  /* software watch dog reset, GPIO status won't change */
  REASON_SOFT_RESTART     = 4,  /* software restart ,system_restart , GPIO status won't change */
  REASON_DEEP_SLEEP_AWAKE = 5,  /* wake up from deep-sleep */
  REASON_EXT_SYS_RST      = 6   /* external system reset */
};

enum esp32_board_id {
  ESP32_DEVKIT,
  ESP32_TTGO_V2_OLED,
  ESP32_HELTEC_OLED,
  ESP32_TTGO_T_BEAM,
  ESP32_TTGO_T_WATCH,
  ESP32_S2_T8_V1_1,
  ESP32_S3_DEVKIT,
};

struct rst_info {
  uint32_t reason;
  uint32_t exccause;
  uint32_t epc1;
  uint32_t epc2;
  uint32_t epc3;
  uint32_t excvaddr;
  uint32_t depc;
};

/* Boya Microelectronics Inc. */
#define BOYA_ID                 0x68
#define BOYA_BY25Q32AL          0x4016

#define MakeFlashId(v,d)        ((v << 16) | d)

/* Disable brownout detection (avoid unexpected reset on some boards) */
#define ESP32_DISABLE_BROWNOUT_DETECTOR 0

#define NMEA_TCP_SERVICE
#define USE_NMEALIB
#define USE_OLED
#define EXCLUDE_OLED_049
//#define EXCLUDE_OLED_BARO_PAGE
#define USE_TFT
#define USE_NMEA_CFG
#define USE_BASICMAC

#define USE_TIME_SLOTS

/* Experimental */
//#define USE_BLE_MIDI
//#define USE_GDL90_MSL
#define USE_OGN_ENCRYPTION

//#define EXCLUDE_GNSS_UBLOX    /* Neo-6/7/8 */
#if !defined(STRATUX)
  #define ENABLE_UBLOX_RFS        /* revert factory settings (when necessary) but not in STRATUX case  */
#endif /* STRATUX */
#define EXCLUDE_GNSS_GOKE       /* 'Air530' GK9501 GPS/GLO/BDS (GAL inop.)   */
//#define EXCLUDE_GNSS_AT65     /* 'fake Neo-6/8' on some 2018 T-Beam boards */
#define EXCLUDE_GNSS_SONY
#define EXCLUDE_GNSS_MTK

#define EXCLUDE_CC13XX
#define EXCLUDE_SOFTRF_HEARTBEAT
#define EXCLUDE_LK8EX1

#if defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define EXCLUDE_NRF905
#define EXCLUDE_UATM
#define EXCLUDE_LED_RING

/* Experimental */
//#define USE_USB_HOST

#if defined(USE_USB_HOST)
#undef  SOC_GPIO_PIN_T8_S2_CONS_RX
#undef  SOC_GPIO_PIN_T8_S2_CONS_TX
#define SOC_GPIO_PIN_T8_S2_CONS_RX      46
#define SOC_GPIO_PIN_T8_S2_CONS_TX      45
#endif /* USE_USB_HOST */
#endif /* CONFIG_IDF_TARGET_ESP32S2 */

#define POWER_SAVING_WIFI_TIMEOUT 600000UL /* 10 minutes */

//#define PMK2_SLEEP_MODE 1    // 0.6 mA : esp_deep_sleep_start()
//#define PMK2_SLEEP_MODE 2    // 0.9 mA : axp.setSleep()
#define PMK2_SLEEP_MODE 3      //  60 uA : axp.shutdown()

#if defined(USE_OLED)
#define U8X8_OLED_I2C_BUS_TYPE  U8X8_SSD1306_128X64_NONAME_2ND_HW_I2C
#endif /* USE_OLED */

#endif /* PLATFORM_ESP32_H */

#endif /* ESP32 */
