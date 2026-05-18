#ifndef _PATH_H_
#define _PATH_H_

#include <Arduino.h>
#include <iostream>
#include "Print.h"
#include <math.h>

#include <SPI.h>
#include <Wire.h>

/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_timer.h"

/* BATTRY */
#include <driver/adc.h>
#include <SimpleKalmanFilter.h>

/* ESP CORE */
#include "esp_timer.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include <esp_task_wdt.h>
#include "esp_log.h"

/* BUTTONS */
#include "InputButton.h"

/* TIME */
#include "time.h"
#include <ESP32Time.h>
#include "time/ServerTime.h"


/* SPI FILE SYSTEM */
#include <ff.h>
#include <FS.h>
#include <SPIFFS.h>

/* WIFI */
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <ArduinoWebsockets.h>
#include <WebSocketsClient.h>
#include <ESPAsyncWebServer.h>
#include "AsyncJson.h"

#include "network/wifi.h"

/* AUDIO */
#include "driver/i2s.h"

/* JSON */
#include <ArduinoJson.h>


/* SENSORS */
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_BMP085.h>
#include "sensors/sensors.h"

/**************************************/

#include "config/mem.h"
#include "files/SpiffsIo.h"
#include "tft/tft_helper.h"
#include "tft/tft_tools.h"
#include "tft/display.h"




#endif /* _PATH_H_ */