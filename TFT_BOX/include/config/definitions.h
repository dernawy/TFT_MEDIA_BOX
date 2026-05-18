#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <ArduinoJson.h>
#include "tft/tft_helper.h"
#include "sensors/sensors_helper.h"
#include "InputButton.h"
#include <TFT_eSPI.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

class definitions {

    public:

    /*=========================================================================
    *                           PUBLIC DEFINITIONS
    ==========================================================================*/

    #define DEF_DEVICE_NAME          "MEDIA BOX"
    #define DEF_DEVICE_MODEL         "CY-DR007MB"
    #define DEF_DEVICE_VER           "1"
    #define DEF_DEVICE_FIRMWARE_NAME "DERNA"
    #define DEF_DEVICE_FIRMWARE_VER  "1.0"
    #define DEF_DEVICE_MEM           "16"   // MB
    #define DEF_DEVICE_HAS_PSRAM     "Yes"
    #define DEF_DEVICE_PSRAM_SIZE    "8"    // MB
    #define DEF_DEVICE_RAM_SIZE      "584"  // KB
    #define DEF_DEVICE_HAS_WIFI      "Yes"   
    #define DEF_DEVICE_HAS_BLE       "Yes"   
    #define SUPPORT_EMAIL            "akram.latrash@gmail.com"
    #define SUPPORT_MOBILE           "+33636128877"


    SemaphoreHandle_t g_send_lock  = NULL;
    TickType_t wait_ticks = 5;

    /*=========================================================================
    *                        PSRAM MEMORY ALLOCATION
    ==========================================================================*/

    #define SPIFFS_CONFIG_FILE_JSON_SIZE 2000

    /*=========================================================================
    *                             SERVER TIME
    ==========================================================================*/

    QueueHandle_t xServerTimeConfigQueue = NULL;
    TaskHandle_t SERVER_TIME;
    server_time_config_t config_server_time;
    server_time_set_t server_time_set;
    server_time_info_t server_time_info;
    

    /*=========================================================================
    *                              TFT SCREEN
    ==========================================================================*/

    TFT_eSPI tft                      = TFT_eSPI();
    TaskHandle_t TFT_MAIN_TASK;
    TaskHandle_t V_STREAM_TASK;
    TaskHandle_t BACKUP_SPIFFS_TASK;
    QueueHandle_t xTftConfigQueue     = NULL;
    QueueHandle_t xTopbarConfigQueue  = NULL;
    QueueHandle_t xDesktopConfigQueue = NULL;
    QueueHandle_t xVideoStreamQueue   = NULL;
    tft_tools_button_config_t set_tft_tools_button_config;
    tft_tools_pallet_config_t set_tft_tools_pallet_config;
    tft_tools_wifi_scan_config_t set_tft_tools_wifi_scan_config;
    tft_device_setup_screen_config_t set_tft_device_setup_screen_config;
    tft_general_config_t set_tft_general_control;
    top_bar_config_t set_top_bar_control;
    main_page_config_t tft_main_page_control;
    main_page_config_t *get_desktop_page_control;
    video_stream_config_t video_stream_config;

    backup_control_t bk_control;

    String PAGE_NAME;

    String V_STREAM_VAR;
    String V_STREAM_VAL;
    int VIDEO_X;
    int VIDEO_Y;

    String WIFI_SCAN_LIVE_SSID;
    
    /*=========================================================================
    *                           SENSORS DECLARTIONS
    ==========================================================================*/

    TaskHandle_t SENSORS_READ_TASK;
    TaskHandle_t BAT_CHARGE_TASK;
    sensors_information_t sensors_information;

    /*=========================================================================
    *                           BATTRY
    ==========================================================================*/
    #define VBAT_PIN     35
    #define BATTV_MAX    4.1     // maximum voltage of battery
    #define BATTV_MIN    3.2     // what we regard as an empty battery
    #define BATTV_LOW    3.4     // voltage considered to be low battery

    /*=========================================================================
    *                           VIRABLES DECLARTIONS
    ==========================================================================*/
    bool SETUP_OK;
    String COUNTRY;
    String CITY;
    String TIMEZONE;
    String TIMEZONE_STR;
    long TIMEZONE_DL;
    long TIMEZONE_OS; //offset
    String ROUTER_SSID;
    String ROUTER_PASS;

    const char* AP_SSID = "ESP32-THAT-PROJECT";
    const char* AP_PASS = "California";

    String BAT_VOLT_STR;
    String BAT_PERCENT_STR;


    TaskHandle_t Task1;                 // Global variables available to BOTH processors 0 and 1
    const uint8_t* arrayName;           // Name of FLASH array containing Jpeg
    uint16_t arrayLength;               // Length of the Jpeg Image
    bool doDecoding = false;            // Mutex flag to start decoding
    bool mcuReady = false;              // Mutex flag to indicate an MCU block is ready for rendering
    uint16_t mcuBuffer[16*16];          // Buffer to grab a snapshot of decoded MCU block
    int32_t mcu_x, mcu_y, mcu_w, mcu_h; // Snapshot of the place to render the MCU
    
    WebsocketsServer soc_server;
    WebsocketsClient soc_client;
    
};



#endif /* _DEFINITIONS_H_ */