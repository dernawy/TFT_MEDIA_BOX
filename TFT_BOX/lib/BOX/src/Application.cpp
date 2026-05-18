#include "Application.h"
#include "network/network_helper.h"

Application::Application(/* args */){}
Application::~Application(){}


void Application::init_application(){

    ledcSetup( 4, 12000, 8 ); // ledc: 4  => Group: 0, Channel: 2, Timer: 1, led frequency, resolution  bits 
	ledcAttachPin( GPIO_NUM_12, 4 );   // gpio number and channel
	ledcWrite( 4, 255 ); // write to channel number 4}

    app.g_send_lock = xSemaphoreCreateMutex();
    
    
    app.init_display();

    if(app.SETUP_OK){

        app.init_server_time();
        app.init_sensors();
        
        start_wifi_ap();
        socketServerListen();
    }
    else
    {

    }

    

    
    
    

}

















Application APP;
Application app;