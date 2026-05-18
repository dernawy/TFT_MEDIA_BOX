#include "ServerTime.h"
#include "Application.h"

static const char *TAG                  = "ServerTime";

#define SERVER_TIME_UPDATE_SUCCESS    (SERVER_TIME_UPDATE_BASE + 0)
#define SERVER_TIME_UPDATE_FAIL       (SERVER_TIME_UPDATE_BASE + 1)

#define INTERNAL_TIME_UPDATE_SUCCESS    (INTERNAL_TIME_UPDATE_BASE + 0)
#define INTERNAL_TIME_UPDATE_FAIL       (INTERNAL_TIME_UPDATE_BASE + 1)

/* Event source task related definitions */
ESP_EVENT_DEFINE_BASE(SERVER_TIME);

/* Event source task related definitions */
ESP_EVENT_DEFINE_BASE(INTERNAL_TIME);

WiFiUDP ntpUDP;
HTTPClient time_http;

ESP32Time rtc(3600);  // offset in seconds GMT+1

struct tm timeinfo;

server_time_config_t g_server_time_config;
server_time_set_t g_server_time_set;


ServerTime::ServerTime(/* args */){
    default_server           = "pool.ntp.org";
    timeApiUrl               = "http://worldtimeapi.org/api/ip";
    gmtOffset_sec            = 0;
    daylightOffset_sec       = 0;
    use_internal_time        = true;
    set_internal_time_by_ntp = true;
}
ServerTime::~ServerTime(){}

/************************* PRIVATES FUNCTIONS ************************/
server_time_internet_connection_t ServerTime::start_update_time_wifi(){

    WiFi.mode(WIFI_STA);
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN); 
    
    app.ROUTER_SSID = app.getString("ROUTER_SSID");
    app.ROUTER_PASS = app.getString("ROUTER_PASS");

    WiFi.begin(app.ROUTER_SSID.c_str(), app.ROUTER_PASS.c_str());

    while(WiFi.status() != WL_CONNECTED){
        Serial.print("-");
        delay(500);
    }

    switch (WiFi.status()){

        case WL_CONNECTED:
            

            //ESP_ERROR_CHECK(WiFi.setDualAntennaConfig(2, 25, WIFI_RX_ANT_AUTO, WIFI_TX_ANT_AUTO));

           
            return CONNECTED;
        break;

        case WL_NO_SSID_AVAIL:
            return NO_SSID_AVAIL;
        break;

        case WL_CONNECT_FAILED:
            return CONNECT_FAILED;
        break;
    
        default:
        break;
    }

   return UNKNOWN_ERROR;

}

/* This function is in /include/time/server_time_helper.h */
bool create_server_time_queu(){

    app.xServerTimeConfigQueue = xQueueCreate(2, sizeof(tft_general_config_t *));

    if( app.xServerTimeConfigQueue == 0 ){
        Serial.println("[QUEUE] - Failed while creating xServerTimeConfigQueue");
        return false;
    }

    Serial.println("[QUEUE] - xServerTimeConfigQueue created succesfully");

    return true;

}

/* This function is in /include/time/server_time_helper.h */
void send_to_server_time_queu(void *const pxConfig){

    // Send a pointer to a struct AMessage object.  Don't block if the queue is already full.
    if(xQueueSend(app.xServerTimeConfigQueue, &pxConfig, 0 ) != pdTRUE){
        Serial.println("[Error] - Faild send data to xServerTimeConfigQueue queue");
    }
}

NTPClient ServerTime::timeClient(){

    NTPClient timeClient(ntpUDP, default_server);

    return timeClient;

}

static void time_update_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data){

    if (base != SERVER_TIME) {
        return;
    }

    switch (id) {

        case SERVER_TIME_UPDATE_FAIL: {

            //ESP_LOGI(TAG, "-> Server time update faild\n");

            break;
        }

        case SERVER_TIME_UPDATE_SUCCESS: {

            //ESP_LOGI(TAG, "-> Server time updated successfully\n");

            app.set_top_bar_control.time = app.server_time_info.time_string;
            app.set_top_bar_control.day  = app.server_time_info.week_day_string;
                
        
            send_to_top_bar_config_queu(&app.set_top_bar_control);

            //INTF.GENERAL_TIME = INTF.set_time.generalTime;

            break;
        }

        default:
        break;
    }

    

}

static void time_internal_update_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data){

    if (base != INTERNAL_TIME) {
        return;
    }

    switch (id) {

        case INTERNAL_TIME_UPDATE_FAIL: {
    
            ESP_LOGI(TAG, "-> Internal time update faild\n");
            break;
        }

        case INTERNAL_TIME_UPDATE_SUCCESS: {

            ESP_LOGI(TAG, "-> Internal time updated successfully\n");

            app.set_top_bar_control.time = app.server_time_info.time_string;
            app.set_top_bar_control.day  = app.server_time_info.week_day_string;
        
            //send_to_top_bar_config_queu(&app.set_top_bar_control);

            /*if (xSemaphoreTake(app.g_send_lock, pdMS_TO_TICKS(app.wait_ticks)) != pdPASS) {
                return;
            }*/

            
            
            app.tft_main_page_control.elements[0] = app.server_time_info.time_string;
            //send_to_desktop_config_queu(&app.tft_main_page_control);

            //xSemaphoreGive(app.g_send_lock);

            break;
        }

        default:
        break;
    }
}

bool ServerTime::setOffset(long val){
    
    rtc.offset    = val; // change offset value
    gmtOffset_sec = val;

    if(rtc.offset != val){
        return false;
    }

    return true;
}

bool ServerTime::setDayLight(int val){
    
    daylightOffset_sec = val;

    if(daylightOffset_sec != val){
        return false;
    }

    return true;
}

void vserver_time_cb(void * arg){

    Serial.print("\nServer time task started\n");

    for(;;){

        if( app.xServerTimeConfigQueue != 0 ){

            if(xQueueReceive(app.xServerTimeConfigQueue, &app.config_server_time, 0) == pdTRUE){

                app.configTft(&app.set_tft_general_control);

            }

        }

        app.server_time_loop();

        vTaskDelay(500 / portTICK_PERIOD_MS);

    }

}

void ServerTime::_do_init(server_time_config_t config){
    
    ESP_ERROR_CHECK(esp_event_handler_register(SERVER_TIME, ESP_EVENT_ANY_ID, time_update_event_handler, NULL));

    SpiRamJsonDocument doc(1000);

    time_http.begin(timeApiUrl);
    int httpCode = time_http.GET();

    if (httpCode != 200){
        printf("Unable to fetch the time info\r\n");
        configTime(gmtOffset_sec, daylightOffset_sec, default_server); 
        return;
    }

    String response = time_http.getString();
    
    deserializeJson(doc, response);

    app.TIMEZONE     = doc["utc_offset"].as<String>();
    app.TIMEZONE_STR = doc["timezone"].as<String>();
    app.TIMEZONE_OS  = doc["raw_offset"].as<long>();
    app.TIMEZONE_DL  = doc["dst_offset"].as<long>();
    
    setOffset(app.TIMEZONE_OS);
    setDayLight(app.TIMEZONE_DL);

    time_http.end();

    configTime(gmtOffset_sec, daylightOffset_sec, default_server); 

    if(!config.use_ntp_only && config.update_by_ntp){

        Serial.println("\n\nStart setting time  by ntp\n");

        ESP_LOGI(TAG, "-> Start setting time  by ntp\n");

        if (getLocalTime(&timeinfo)){

            ESP_ERROR_CHECK(esp_event_handler_register(INTERNAL_TIME, ESP_EVENT_ANY_ID, time_internal_update_event_handler, NULL));

            rtc.setTimeStruct(timeinfo); 
            timeClient().end();

            if(config.disconnect_internet){
                if(WiFi.disconnect()){
                    Serial.println("\nWifi disconnected\n");
                }
            }
            
            esp_event_post(INTERNAL_TIME, INTERNAL_TIME_UPDATE_SUCCESS, &config, sizeof(server_time_config_t), 0);
        }
        else
        {
            esp_event_post(INTERNAL_TIME, INTERNAL_TIME_UPDATE_FAIL, &config, sizeof(server_time_config_t), 0);
        }
    }
    else
    {
       // configTime(gmtOffset_sec, daylightOffset_sec, default_server);
    }
}

void ServerTime::init_time_wifi(){

    if(WiFi.status() != WL_CONNECTED){
        start_update_time_wifi();
    }

}

void ServerTime::init_server_time() {

    ESP_LOGI(TAG, "-> init()\n");

    g_server_time_config = SERVER_TIME_CONFIG_DEFAULT();

    //ESP_LOGI(TAG, "\n ENABLED:  [%d] \n F24:      [%d]\n F_24:     [%d]\n F_12:     [%d]\n W_DIS:    [%d]\n USE_NTP:  [%d]\n U_TYPE:   [%d]\n NTP_ONLY: [%d]\n ", g_server_time_config.enabled, g_server_time_config.format_24h, g_server_time_config.t_12_fm, g_server_time_config.t_24_fm, g_server_time_config.disconnect_internet, g_server_time_config.update_by_ntp, g_server_time_config.updateType, g_server_time_config.use_ntp_only);

    if(!g_server_time_config.enabled){

        ESP_LOGI(TAG, "-> Server time disabled please enable it to be able using it!\n");
        return;
    }

    //ESP_LOGI(TAG, "-> Config server time \n");

    app.init_time_wifi();
    
    _do_init(g_server_time_config);
            
    //TODO: on screen message 
    
    xTaskCreate( vserver_time_cb, "SERVER_TIME", 2000, NULL, tskIDLE_PRIORITY + 10, &app.SERVER_TIME);
   
}

void ServerTime::server_time_loop(){

    if(g_server_time_config.enabled){

        if(!getLocalTime(&timeinfo)){
            Serial.println("Failed to obtain time");
            ESP_LOGI(TAG, "-> Failed to obtain time\n");
            
            esp_event_post(SERVER_TIME, SERVER_TIME_UPDATE_FAIL, &g_server_time_config, sizeof(server_time_config_t), 0);
            esp_event_post(INTERNAL_TIME, INTERNAL_TIME_UPDATE_FAIL, &g_server_time_config, sizeof(server_time_config_t), 0);
            return;
        }

        if(g_server_time_config.format_24h){

            app.config_server_time.updateType  = "24H";

            strftime(app.server_time_set.time_24, sizeof(app.server_time_set.time_24), g_server_time_config.t_24_fm, &timeinfo);

            app.server_time_info.time_string = app.server_time_set.time_24;

            strftime(app.server_time_set.t_stamp, sizeof(app.server_time_set.t_stamp), "%c", &timeinfo);

            app.server_time_info.timeStamp = app.server_time_set.t_stamp;

            strftime(app.server_time_set.t_h, sizeof(app.server_time_set.t_h),"%H", &timeinfo);
            strftime(app.server_time_set.t_m, sizeof(app.server_time_set.t_m),"%M", &timeinfo);
            strftime(app.server_time_set.t_s, sizeof(app.server_time_set.t_s),"%S", &timeinfo);

            strftime(app.server_time_set.dayOfWeek, sizeof(app.server_time_set.dayOfWeek),"%w", &timeinfo);
            strftime(app.server_time_set.week_day, sizeof(app.server_time_set.week_day),"%A", &timeinfo);
            strftime(app.server_time_set.dayOfMonth, sizeof(app.server_time_set.dayOfMonth),"%d", &timeinfo);
            strftime(app.server_time_set.month, sizeof(app.server_time_set.month),"%m", &timeinfo);
            strftime(app.server_time_set.year, sizeof(app.server_time_set.year),"%Y", &timeinfo);
            strftime(app.server_time_set.fullDateNum, sizeof(app.server_time_set.fullDateNum),"%x", &timeinfo);

            app.server_time_info.fullDateNumbers = app.server_time_set.fullDateNum;

            //Serial.print("T_24");Serial.println(app.server_time_info.time_string);

            if(g_server_time_config.update_by_ntp){
                esp_event_post(INTERNAL_TIME, INTERNAL_TIME_UPDATE_SUCCESS, &app.server_time_info, sizeof(server_time_info_t), 0);
            }
            else
            {
                esp_event_post(SERVER_TIME, SERVER_TIME_UPDATE_SUCCESS, &app.server_time_info, sizeof(server_time_info_t), 0);
            }
        }
        else
        {
            app.config_server_time.updateType  = "12H";
            
            strftime(app.server_time_set.time_12, sizeof(app.server_time_set.time_12), g_server_time_config.t_12_fm, &timeinfo);

            app.server_time_info.time_string = app.server_time_set.time_12;
        
            strftime(app.server_time_set.t_stamp, sizeof(app.server_time_set.t_stamp), "%c", &timeinfo);

            app.server_time_info.timeStamp = app.server_time_set.t_stamp;

            strftime(app.server_time_set.t_h, sizeof(app.server_time_set.t_h),"%H", &timeinfo);
            strftime(app.server_time_set.t_m, sizeof(app.server_time_set.t_m),"%M", &timeinfo);
            strftime(app.server_time_set.t_s, sizeof(app.server_time_set.t_s),"%S", &timeinfo);

            strftime(app.server_time_set.dayOfWeek, sizeof(app.server_time_set.dayOfWeek),"%w", &timeinfo);
            strftime(app.server_time_set.week_day, sizeof(app.server_time_set.week_day),"%A", &timeinfo);

            app.server_time_info.week_day_string = app.server_time_set.week_day;

            strftime(app.server_time_set.dayOfMonth, sizeof(app.server_time_set.dayOfMonth),"%d", &timeinfo);
            strftime(app.server_time_set.month, sizeof(app.server_time_set.month),"%m", &timeinfo);
            strftime(app.server_time_set.year, sizeof(app.server_time_set.year),"%Y", &timeinfo);
            strftime(app.server_time_set.fullDateNum, sizeof(app.server_time_set.fullDateNum),"%x", &timeinfo);

            app.server_time_info.fullDateNumbers = app.server_time_set.fullDateNum;

            /*Serial.print("T_12");Serial.println(app.server_time_info.dayOfWeek);
            Serial.print("T_12_1");Serial.println(app.server_time_info.week_day);*/

            if(g_server_time_config.update_by_ntp){
                esp_event_post(INTERNAL_TIME, INTERNAL_TIME_UPDATE_SUCCESS, &app.server_time_info, sizeof(server_time_info_t), 0);
            }
            else
            {
                esp_event_post(SERVER_TIME, SERVER_TIME_UPDATE_SUCCESS, &app.server_time_info, sizeof(server_time_info_t), 0);
            }
        }
    }
    else
    {
        //TODO: on screen time not enabled
    }

}

