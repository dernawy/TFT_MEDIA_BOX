#include "wifi.h"
#include "Application.h"
#include "network/network_helper.h"





wifi::wifi(/* args */){}
wifi::~wifi(){}

void start_wifi_ap(){

    WiFi.softAP(app.AP_SSID, app.AP_PASS);
    
    Serial.print("\nIP: ");Serial.println(WiFi.softAPIP());
    Serial.println();
        
    
}

void wifi::socketServerListen(){
    app.soc_server.listen(8888);  
}

bool wifi::socketServerPoll(){
    return app.soc_server.poll();
}



