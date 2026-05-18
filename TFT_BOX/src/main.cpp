#include "esp_wifi.h"
#include "esp_netif.h"
#include "Application.h"
#include "InputButton.h"
#include "network/network_helper.h"
/*uint8_t led = 0;
uint8_t led1 = 0;
uint8_t led2 = 0;
uint8_t led_all = 0;

InputButton btn_up(BUTTON_UP, true, ACTIVE_HIGH);
InputButton btn_enter(BUTTON_ENTER, true, ACTIVE_HIGH);
InputButton btn_down(BUTTON_DOWN, true, ACTIVE_HIGH);
InputButton btn_edit(BUTTON_EDIT, true, ACTIVE_HIGH);*/
void decodeJpg(void* pvParameter){

    // This is an infinite loop, effectively the same as the normal sketch loop()
  // but this function and loop is running on processor 0
  for(;;) {
    // Decode the Jpeg image
    if (app.doDecoding) { // Only start decoding if main sketch sets this flag
      //TJpgDec.drawJpg(0, 7, app.arrayName, app.arrayLength); // Runs until complete image decoded
      app.doDecoding = false; // Set mutex false to indicate decoding has ended
    }
    yield(); // Must yield in this loop
  }

}



void drawingTimeText(uint32_t deltaT, int yPos){ 
   app.tft.setTextColor(TFT_GREEN,TFT_BLACK); 
   app.tft.fillRect(163, yPos, 77, 20,TFT_BLACK);
   app.tft.setCursor(163, yPos, 4);   
   app.tft.print(deltaT);
}

static bool (*_input)(PlayerInputType type, int key) = nullptr;

/*void input_loop() {

    if ( btn_up.shortPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_up shortPress (4) = %d PIN [%d] \n", led1, BUTTON_UP);
    }

    if ( btn_up.longPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_up longPress (4) = %d PIN [%d] \n", led1, BUTTON_UP);
    }

    

    if ( btn_enter.shortPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_enter shortPress (2) = %d PIN [%d] \n", led1, BUTTON_ENTER);
    }


    if ( btn_enter.longPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_enter longPress (2) = %d PIN [%d] \n", led1, BUTTON_ENTER);
    }

    
    if ( btn_down.shortPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_down shortPress (2) = %d PIN [%d] \n", led1, BUTTON_DOWN);
    }


    if ( btn_down.longPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_down longPress (2) = %d PIN [%d] \n", led1, BUTTON_DOWN);
    }

    
    if ( btn_edit.shortPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_edit shortPress (2) = %d PIN [%d] \n", led1, BUTTON_EDIT);
    }


    if ( btn_edit.longPress() )	{// KeyPressLong muss vor KeyPressShort abgefragt werden!
      digitalWrite(25, led1 ^= 1);
      Serial.printf("btn_edit longPress (2) = %d PIN [%d] \n", led1, BUTTON_EDIT);
    }
    

    if (btn_up.longPress())                Serial.println(BTN_B1LONG);
    if (btn_up.shortPress())               Serial.println(BTN_B1SHORT);

    if (btn_enter.longPress())             Serial.println(BTN_B2LONG);
    if (btn_enter.shortPress())            Serial.println(BTN_B2SHORT);

    if (btn_down.longPress())              Serial.println(BTN_B3LONG);
    if (btn_down.shortPress())             Serial.println(BTN_B3SHORT);

    if (btn_edit.longPress())              Serial.println(BTN_B4LONG);
    if (btn_edit.shortPress())             Serial.println(BTN_B4SHORT);*/

    //return false;
//}


void setup() {

	Wire.begin();

	Serial.begin(115200);
	Serial.println("Serial started");

	esp_log_level_set("*", ESP_LOG_INFO);

	WiFi.mode(WIFI_AP_STA);
	esp_wifi_set_ps(WIFI_PS_NONE);
 
	app.init_application();

  

	//app.make_config_backup();

}  

void loop() {
  // put your main code here, to run repeatedly:

  
  //delay(5000);
 // delay(10);

}

