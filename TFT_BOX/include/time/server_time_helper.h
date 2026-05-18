#pragma once

#include "Arduino.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include <ESP32Time.h>
#include <NTPClient.h>


#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

ESP_EVENT_DECLARE_BASE(SERVER_TIME);
#define SERVER_TIME_UPDATE_BASE      0x100

ESP_EVENT_DECLARE_BASE(INTERNAL_TIME);
#define INTERNAL_TIME_UPDATE_BASE     0x100

typedef struct {
    bool enabled;
    bool update_by_ntp;
    const char *updateType;
    bool use_ntp_only;
    bool format_24h;
    const char *t_12_fm;
    const char *t_24_fm;
    bool disconnect_internet;
} server_time_config_t;

#define SERVER_TIME_CONFIG_DEFAULT() { \
    .enabled             = true, \
    .update_by_ntp       = true, \
    .updateType          = "12H", \
    .use_ntp_only        = false, \
    .format_24h          = false, \
    .t_12_fm             = "%I:%M %p", \
    .t_24_fm             = "%R", \
    .disconnect_internet = true, \
}

typedef struct {
    char t_stamp[25];
    char time_12[16];
    char time_24[16];
    char t_h[3];
    char t_m[3];
    char t_s[3];
    char week_day[10]; // text : sat, sun mon etc...
    char dayOfWeek[2];  // number sat = 0, sun = 1, etc...
    char dayOfMonth[3]; // number
    char month[10]; // number
    char year[5]; // number
    char fullDateNum[10];
} server_time_set_t;

typedef struct {
    String time_string;
    String week_day_string;
    String timeStamp;
    String fullDateNumbers;
} server_time_info_t;

typedef enum {
    CONNECTED,
    NO_SSID_AVAIL,
    CONNECT_FAILED,
    UNKNOWN_ERROR

} server_time_internet_connection_t;

bool create_server_time_queu();

void send_to_server_time_queu(void *const pxConfig);


#ifdef __cplusplus
}
#endif /**< _cplusplus */