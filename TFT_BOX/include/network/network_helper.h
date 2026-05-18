#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>


#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */




void start_wifi_ap();

// This next function will be called by the TJpg_Decoder library during decoding of the jpeg file
// A copy of the decoded MCU block is grabbed for rendering so decoding can then continue while
// the MCU block is rendered on the TFT. Note: This function is called by processor 0






#ifdef __cplusplus
}
#endif /**< _cplusplus */