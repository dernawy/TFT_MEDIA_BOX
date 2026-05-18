#include "display.h"


#include <Adafruit_GFX.h>    // https://github.com/adafruit/Adafruit-GFX-Library
#include "Application.h"
#include "network/network_helper.h"
/* 2 - 27 -> SMALLS / 28 - 53 -> CAPITALS / 54 - 63 -> NUMS / 64 - 83 SIGNS / 84 - 89 CONTROMS */
String KYEPAD_ARRAY[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", /* 0 - 25 */ 
};

String KYEPAD_NUM_ARRAY[] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
};

String KYEPAD_CAPITAL_ARRAY[] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", /* 0 - 25 */
};

String KYEPAD_NUM_SIGN_ARRAY[] = {
    "input", "Back", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "-", "+", "/", "*", "=", "@", "_", ".", ",", "?", ":", "[", "]", "(", ")", "{", /* 0 - 1 / 0 = input /  2 - 11 -> nums / 12 - 31 signs */
    "}", "|", "#", "&", "123", "Ok", "Close", "S P A C E", "Delete"  /* 32 - 36 =  controlls */
};

#define CENTRE 240



InputButton btn_up(BUTTON_UP, true, ACTIVE_HIGH);
InputButton btn_enter(BUTTON_ENTER, true, ACTIVE_HIGH);
InputButton btn_down(BUTTON_DOWN, true, ACTIVE_HIGH);
InputButton btn_edit(BUTTON_EDIT, true, ACTIVE_HIGH);

tft_general_config_t *tft_general_control;
top_bar_config_t *top_bar_control;
main_page_config_t desktop_page_control;
video_stream_config_t *tft_video_stream_config;

//top_bar_config_t set_top_bar_control;

TFT_eSprite top_bar        = TFT_eSprite(&app.tft); // Sprite for desktop top bar
TFT_eSprite desktop_upper  = TFT_eSprite(&app.tft); // Sprite for desktop top bar
TFT_eSprite video_stream   = TFT_eSprite(&app.tft); // Sprite for desktop top bar
TFT_eSprite key_pad        = TFT_eSprite(&app.tft); // Sprite for desktop top bar
TFT_eSprite edit_pallet    = TFT_eSprite(&app.tft); // Sprite for desktop top bar


TFT_eSprite errors_messages  = TFT_eSprite(&app.tft); // Sprite for desktop top bar

int elements_index     = -1;
bool button_move       = false;
bool editNow           = false;
bool show_key_pad      = false;
bool key_pad_move      = false;
bool key_pad_small     = true;
bool key_pad_capital   = false;
bool key_pad_nums      = false;
int key_pad_index      = -1;

bool key_pad_caps   = false;
bool key_pad_smals  = false;

String KEY_PAD_LIVE_TEXT = "";
int live_text_length     = 0;
String last_char         = "";
int last_char_length     = 0;

bool error_msg_active = false;

using namespace websockets;
WebsocketsServer server;
WebsocketsClient soc_client;

String debug_elem_name = "";
IPAddress w_server(192,168,4,2);
String setting_link = "";
String v_encode_net_speed_str = "";
String v_encode_speed_str     = "";

/******** EDIT *******/
edit_pallet_t config_edit_pallet;

String EDIT_LIVE_TEXT = "";
bool edit_move        = false;
bool still_edit       = false;
bool use_caps         = false;
int edit_index        = 0;
int edit_chars_index  = -1;
bool show_edit_palet  = false;

display::display(/* args */){
    _show_logo        = true;
    _logo_delay       = 3000;
    _enable_animation = false;
    _show_errors      = true;
    _current_width    = app.tft.width();
    _current_hieght   = app.tft.height();
    _default_rotation = 3;
    _current_rotation = -1;
    _show_top_bar     = true;
}
display::~display(){}

/* This function is in /include/tft/tft_helper.h */
bool create_tft_config_queu(){

    app.xTftConfigQueue = xQueueCreate(2, sizeof(tft_general_config_t *));

    if( app.xTftConfigQueue == 0 ){
        Serial.println("[QUEUE] - Failed while creating xTftConfigQueue");
        return false;
    }

    Serial.println("[QUEUE] - xTftConfigQueue created succesfully");

    return true;

}

/* This function is in /include/tft/tft_helper.h */
void send_to_tft_config_queu(void *const pxConfig) {

    // Send a pointer to a struct AMessage object.  Don't block if the queue is already full.
    if(xQueueSend(app.xTftConfigQueue, &pxConfig, 0 ) != pdTRUE){
        Serial.println("[Error] - Faild send data to xTftConfigQueue queue");
    }

}

/* This function is in /include/tft/tft_helper.h */
bool create_top_bar_config_queu() {

    app.xTopbarConfigQueue = xQueueCreate(2, sizeof(top_bar_config_t *));

    if( app.xTopbarConfigQueue == 0 ){
        Serial.println("[QUEUE] - Failed while creating xTopbarConfigQueue");
        return false;
    }

    Serial.println("[QUEUE] - xTopbarConfigQueue created succesfully");

    return true;

}

/* This function is in /include/tft/tft_helper.h */
void send_to_top_bar_config_queu(void *const pxConfig) {

    // Send a pointer to a struct AMessage object.  Don't block if the queue is already full.
    if(xQueueSend(app.xTopbarConfigQueue, &pxConfig, 0) != pdTRUE){
        Serial.println("[Error] - Faild send data to xTopbarConfigQueue queue");
    }
}

/* This function is in /include/tft/tft_helper.h */
bool create_desktop_config_queu() {

    app.xDesktopConfigQueue = xQueueCreate(2, sizeof(tft_general_config_t *));

    if( app.xDesktopConfigQueue == 0 ){
        Serial.println("[QUEUE] - Failed while creating xDesktopConfigQueue");
        return false;
    }

    Serial.println("[QUEUE] - xDesktopConfigQueue created succesfully");

    return true;

}

/* This function is in /include/tft/tft_helper.h */
void send_to_desktop_config_queu(void *const pxConfig) {

    // Send a pointer to a struct AMessage object.  Don't block if the queue is already full.
    if(xQueueSend(app.xDesktopConfigQueue, &pxConfig, 0 ) != pdTRUE){
        Serial.println("[Error] - Faild send data to xDesktopConfigQueue queue");
    }

}

void vTftMainTask_cb(void * arg){

    Serial.println("\n[TFT MAIN TASK] -> Tft main task started\n");

    for(;;){

        

        if( app.xTftConfigQueue != 0 ){

            if(xQueueReceive(app.xTftConfigQueue, &tft_general_control, 0) == pdTRUE){

                app.configTft(tft_general_control);
            }
        }

        if(app.SETUP_OK){

            if( app.xTopbarConfigQueue != 0 ){

                if(xQueueReceive(app.xTopbarConfigQueue, &top_bar_control, 0) == pdTRUE){

                    app.controlTopbar(top_bar_control);
                }
            }

            app.controlTopbar(top_bar_control);

            if( app.xDesktopConfigQueue != 0 ){

                if(xQueueReceive(app.xDesktopConfigQueue, &app.get_desktop_page_control, 0) == pdTRUE){

                    app.controlDesktopPage(app.get_desktop_page_control);

                    if(app.PAGE_NAME.equals("DESKTOP")){
                        app.desktop_page();
                    }
                }
            }

            app.controlDesktopPage(app.get_desktop_page_control);

            if(app.PAGE_NAME.equals("DESKTOP")){
                app.desktop_page();
            }

            if(app.PAGE_NAME.equals("SETTINGS")){
                
                app.setting_page(&desktop_page_control);
            }

            if(app.PAGE_NAME.equals("LOCATION")){
                app.setting_location_page();
            }
        }
        else
        {
            //if( show_edit_palet ){
                app.main_setup_display();
            //}
            
        }

        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

/**************************************************************************************************************************************************************************************
*                                                                                 PRIVATE FUNCTIONS
**************************************************************************************************************************************************************************************/
void display::setPrvShowLogo(bool show_logo){
    _show_logo = show_logo;
}

void display::setPrvDefaultRotation(uint8_t rotation){
    _default_rotation = rotation;
}

void display::setPrvCurrentRotation(uint8_t rotation){
    
    _current_rotation = app.tft.getRotation();

    if(_current_rotation != rotation){
        _current_rotation = rotation;
    }

    _current_width  = app.tft.width();
    _current_hieght = app.tft.height();
}

void display::logo(){
    // Draw the eclips_logo -> in images/eclips_logo.h
    int x_1 = ((app.tft.width() - 128) / 2);
    int x_2 = ((app.tft.width() - 180) / 2);

    app.tft.pushImage(x_1, 50, 128, 128, eclips_logo); 
    app.tft.pushImage(x_2, 180, 200, 100, cy_home_p, TFT_TRANSPARENT);
}

/**************************************************************************************************************************************************************************************
*                                                                                 PUBLIC FUNCTIONS
**************************************************************************************************************************************************************************************/


/* This function is in /include/tft/tft_helper.h */
void init_display_backlight(){

    ledcSetup( BACK_LIGHT_CHANNEL, BACK_LIGHT_FREQUENCY, BACK_LIGHT_RESOLUTION ); // ledc: 4  => Group: 0, Channel: 2, Timer: 1, led frequency, resolution  bits 
    ledcAttachPin( GPIO_NUM_12, BACK_LIGHT_CHANNEL );   // gpio number and channel
    ledcWrite( BACK_LIGHT_CHANNEL, 255 ); // write to channel number 4

}

void display::init_display(){
 
    create_tft_config_queu();

    app.set_tft_general_control = TFT_DESKTOP_CONFIG_DEFAULT();

    send_to_tft_config_queu(&app.set_tft_general_control);

    xTaskCreate( vTftMainTask_cb, "TFT_MAIN_TASK", 20000, NULL, tskIDLE_PRIORITY + 5, &app.TFT_MAIN_TASK);
}

int setup_live_text_length     = 0;

String display::setup_key_pad_backspace(String live_text){

    if(last_char.length() > 0){
        last_char      = "";
    }
    else
    {

        EDIT_LIVE_TEXT = EDIT_LIVE_TEXT.substring(0, setup_live_text_length-1);

        setup_live_text_length = EDIT_LIVE_TEXT.length();
    }
    
    return EDIT_LIVE_TEXT;
}

void display::setup_edit_pallet(edit_pallet_t pallet){

    
    uint16_t edit_color = 0x20E3;

    if(!edit_pallet.created()){

        edit_pallet.setSwapBytes(true);
        edit_pallet.createSprite(pallet.w, pallet.h, 2);
        edit_pallet.fillRect(0, 0, edit_pallet.width(), edit_pallet.height(), SETTINGS_PAGE_BG); // to hide the fill color 
    }
    
    edit_pallet.drawRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, TFT_WHITE);
    edit_pallet.fillRoundRect(pallet.x, (pallet.y - 4), (pallet.w - 2), (pallet.h - 2), 4, SETTINGS_PAGE_BG);
    
    edit_pallet.setTextColor(pallet.txtColor, pallet.bgColor);
    edit_pallet.setFreeFont(pallet.font);

    int _x, _y, _w, _h;

    int input_x = 4;
    int input_y = 4;
    int input_w = (pallet.w - 8);
    int input_h = 30;

    String edit_elements[8] = {"input", "<", "OK", "Del", ">", "Cap", "Save", "X"};

    int edit_elements_length = STRING_ARRAY_SIZE(edit_elements);
    int chars_length         = STRING_ARRAY_SIZE(KYEPAD_ARRAY);

    int l_btn_w   = edit_pallet.textWidth(edit_elements[1]);
    int ok_btn_w  = edit_pallet.textWidth(edit_elements[2]);
    int del_btn_w = edit_pallet.textWidth(edit_elements[3]);
    int r_btn_w   = edit_pallet.textWidth(edit_elements[4]);
    int ca_btn_w  = edit_pallet.textWidth(edit_elements[5]);
    int sv_btn_w  = edit_pallet.textWidth(edit_elements[6]);
    int x_btn_w   = edit_pallet.textWidth(edit_elements[7]);

    if ( btn_up.shortPress() ){

        edit_move = true;

        edit_index ++;

        if(edit_index >= edit_elements_length/*elements_length*/){
            edit_index = 0;
        } 

    }

    if ( btn_enter.longPress() ){

        if(edit_index == 3){ // Del

            show_edit_palet = false;

            last_char      = "";
            EDIT_LIVE_TEXT = "";
            edit_chars_index = -1;
            elements_index   = 4;
            still_edit       = false;
            edit_move        = false;
            use_caps         = false;
            

        }
        
    }

    if ( btn_enter.shortPress() ){

        if(edit_index == 0){ // input

        }

        if(edit_index == 1){ // <

            still_edit = true;

            if(pallet.chars_type == 0){

                if(use_caps){

                    last_char = KYEPAD_CAPITAL_ARRAY[edit_chars_index];
                    last_char_length       = last_char.length(); // get new length after deleting last char
                }
                else
                {
                    last_char = KYEPAD_ARRAY[edit_chars_index];
                    last_char_length       = last_char.length(); // get new length after deleting last char
                }
            }

            if(pallet.chars_type == 1){
                last_char = KYEPAD_NUM_ARRAY[edit_chars_index];
            }

            if(edit_chars_index <= 0){
                edit_chars_index = chars_length;
            }

            edit_chars_index--;

        }

        if(edit_index == 2){ // OK

            EDIT_LIVE_TEXT   += last_char;
            edit_chars_index = -1;
            last_char        = "";
            still_edit       = false;

        }

        if(edit_index == 3){ // Del
            setup_key_pad_backspace(EDIT_LIVE_TEXT); // private function
        }

        if(edit_index == 4){ //>

            edit_chars_index++;

            still_edit = true;

            if(pallet.chars_type == 0){

                if(use_caps){

                    last_char = KYEPAD_CAPITAL_ARRAY[edit_chars_index];
                    last_char_length       = last_char.length(); // get new length after deleting last char
                    
                }
                else
                {
                    last_char = KYEPAD_ARRAY[edit_chars_index];
                    last_char_length       = last_char.length(); // get new length after deleting last char
                }
            }

            if(pallet.chars_type == 1){
                last_char = KYEPAD_NUM_ARRAY[edit_chars_index];
            }
            
            if(edit_chars_index >= chars_length){
                edit_chars_index = -1;
            }
        }

        if(edit_index == 5){ // Caps

            if(use_caps){
                use_caps = false;
            }
            else
            {
                use_caps = true;
            }
            
        }

        if(edit_index == 6){ // Save

            String _key = pallet.key;
            _key.toUpperCase();

            app.setString(_key.c_str(), EDIT_LIVE_TEXT.c_str());

            if(config_edit_pallet.current_index == 1){
                app.COUNTRY = app.getString(_key.c_str());
            }

            if(config_edit_pallet.current_index == 2){
                app.CITY = app.getString(_key.c_str());
            }

            if(config_edit_pallet.current_index == 3){
                app.TIMEZONE = app.getString(_key.c_str());
            }
            

            still_edit              = false;
            edit_move               = false;
            use_caps                = false;
            edit_chars_index        = -1;
            elements_index          = -1;
            config_edit_pallet.edit = false;
            edit_pallet.deleteSprite();
            desktop_upper.fillRect(0, 0, desktop_upper.width(), desktop_upper.height(), SETTINGS_PAGE_BG);

        }
        
        if(edit_index == 7){ // close

            edit_pallet.deleteSprite();

        }

        setup_live_text_length = EDIT_LIVE_TEXT.length(); // get new length after deleting last char
        

    }

    if ( btn_down.shortPress() ){
            
        edit_move = true;
        

        if(edit_index <= 0){
            edit_index = edit_elements_length/*elements_length*/;
        } 

        edit_index --;
    }

    EDIT_LIVE_TEXT[0] = toupper(EDIT_LIVE_TEXT[0]);

    for(int i = 0; i < edit_elements_length; i++){

        if( i == 0 ){

            edit_pallet.drawRoundRect(input_x, input_y, input_w, input_h, 4, TFT_WHITE); // input
            edit_pallet.fillRoundRect((input_x + 2), (input_y + 2), (input_w - 4), (input_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(EDIT_LIVE_TEXT, (input_x + 5), (input_y + 5));

            int len = edit_pallet.textWidth(EDIT_LIVE_TEXT);

            if(edit_move && edit_index == i && !still_edit){

                edit_pallet.fillRoundRect((input_x + 2), (input_y + 2), (input_w - 4), (input_h - 4), 4, edit_color);
                edit_pallet.setTextColor(TFT_WHITE, edit_color);

                edit_pallet.drawString(EDIT_LIVE_TEXT, (input_x + 5), (input_y + 8));
    
            }
            else
            {
                edit_pallet.fillRoundRect((input_x + 2), (input_y + 2), (input_w - 4), (input_h - 4), 4, edit_color);
                edit_pallet.setTextColor(TFT_WHITE, edit_color);

                edit_pallet.drawString(EDIT_LIVE_TEXT, (input_x + 5), (input_y + 8));
                edit_pallet.drawString(last_char, (input_x + len + 7), (input_y + 8));
            }
        }

        if( i == 1 ){

            _x = (input_x + 8);
            _y = (input_h + 10);
            _w = (l_btn_w + 10);
            _h = (input_h - 12);

            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // <  cadre
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }
        }

        if( i == 2 ){

            _x = (_x + _w + 10);
            _y = (input_h + 10);
            _w = (ok_btn_w + 8);
            _h = (input_h - 12);

            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // ok cadre
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // OK

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }
        }

        if( i == 3 ){

            _x = (_x + _w + 10);
            _y = (input_h + 10);
            _w = (del_btn_w + 8);
            _h = (input_h - 12);

            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // Del cadre
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // Del

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }
        }

        if( i == 4 ){

            _x = (_x + _w + 10);
            _y = (input_h + 10);
            _w = (r_btn_w + 10);
            _h = (input_h - 12);

            
            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // >  cadre
            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // >

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }
        }

        if( i == 5 ){
            _x = (_x + _w + 10);
            _y = (input_h + 10);
            _w = (ca_btn_w + 10);
            _h = (input_h - 12);

            
            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // Cap cadre
            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // Cap

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }
        }
        
        if( i == 6 ){
            _x = 75;
            _y = (_y + 30);
            _w = (sv_btn_w + 10);
            _h = (input_h - 12);

            
            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // Save cadre
            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // Save

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // Save
            }
        }

        if( i == 7 ){

            _x = (_x + _w + 45);
            _y = _y;
            _w = (x_btn_w + 10);
            _h = (input_h - 12);

            edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // close cadre
            edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
            edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            edit_pallet.drawString(edit_elements[i], (_x + 5), (_y + 4)); // close

            if(edit_move && edit_index == i){
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                edit_pallet.setTextColor(edit_color, TFT_WHITE);
                edit_pallet.drawString(edit_elements[i], (_x + 5), (_y + 4)); // close
            }

        }
    
    }


    edit_pallet.pushToSprite(&desktop_upper, pallet.x, pallet.y);

}

int setup_step = 0;
uint16_t setup_bgColor = 0x2451;
bool setup_country_ok = false;

void display::createEditBtn(int step, TFT_eSprite target, int32_t x, int32_t y, int16_t txtColor, int16_t cadreColor, int16_t fillColor, const GFXfont *font){

    String btn_txt      = "Edit";
    int btn_txt_w       = 0;
    

    if(!edit_pallet.created()){

        edit_pallet.setSwapBytes(true);
        edit_pallet.createSprite(47, 20, 2);
    }

    edit_pallet.setFreeFont(font);
    btn_txt_w = edit_pallet.textWidth(btn_txt);
        
    edit_pallet.drawRoundRect(1, 1, (btn_txt_w + 8), 18, 4, cadreColor); 
    edit_pallet.fillRoundRect(3, 3, (btn_txt_w + 5), 15, 4, fillColor);
    edit_pallet.setTextColor(txtColor, fillColor);
    edit_pallet.drawString(btn_txt, 5, 5);

    edit_pallet.pushToSprite(&target, x, y);
}

void display::first_setup_display(){

    String title_txt          = "Setup New Device";
    String intro_txt          = "";
    String start_btn_txt      = "Start Setup";
    String start_btn_info_txt = "Press ok button to start";
    int title_w      = 0;
    int title_x      = 0;
    int intro_w      = 0;
    int intro_x      = 0;
    int intro_y      = 70;

    int start_btn_w  = 0;
    int start_btn_x  = 0;
    int start_btn_info_w = 0;
    int start_btn_info_x = 0;

    desktop_upper.setFreeFont(&Dialog_bold_22);

    title_w      = desktop_upper.textWidth(title_txt);
    title_x      = ((desktop_upper.width() - title_w) / 2);

    desktop_upper.setTextColor(TFT_WHITE, setup_bgColor);
    desktop_upper.drawString(title_txt, title_x, 30); // Print page title

    if( setup_step == 0 ){

        intro_txt          = "Starting setup your new device, be ready!";

        desktop_upper.setFreeFont(&DejaVu_Sans_Bold_14);

        intro_w      = desktop_upper.textWidth(intro_txt);
        intro_x      = ((desktop_upper.width() - intro_w) / 2);

        desktop_upper.setTextColor(TFT_WHITE, setup_bgColor);
        desktop_upper.drawString(intro_txt, intro_x, intro_y); // intro text

        desktop_upper.setFreeFont(&Dialog_bold_22);

        start_btn_w  = desktop_upper.textWidth(start_btn_txt);
        start_btn_x  = ((desktop_upper.width() - start_btn_w) / 2);

        desktop_upper.setTextColor(TFT_WHITE, TFT_ORANGE);
        desktop_upper.drawRoundRect((start_btn_x - 5), 145, (start_btn_w + 10), 35, 4, TFT_WHITE);
        desktop_upper.fillRoundRect((start_btn_x - 3), 147, (start_btn_w + 6), 31, 4, TFT_ORANGE);
        desktop_upper.drawString(start_btn_txt, start_btn_x, 150); // start button

        desktop_upper.setTextFont(2);
        start_btn_info_w  = desktop_upper.textWidth(start_btn_info_txt);
        start_btn_info_x  = ((desktop_upper.width() - start_btn_info_w) / 2);

        desktop_upper.setTextColor(TFT_WHITE, setup_bgColor);
        desktop_upper.drawString(start_btn_info_txt, start_btn_info_x, 190); // start button info

        app.set_tft_tools_pallet_config.pallet_x          = 190,
        app.set_tft_tools_pallet_config.pallet_y          = 210,
        app.set_tft_tools_pallet_config.targetBgColor     = setup_bgColor;
        app.set_tft_tools_pallet_config.txtFont           = &DejaVu_Sans_Bold_11,
        app.set_tft_tools_pallet_config.use_target_bg     = false,
        app.set_tft_tools_pallet_config.txtColor          = TFT_WHITE,
        app.set_tft_tools_pallet_config.cadrColor         = TFT_WHITE;
        app.set_tft_tools_pallet_config.input_cadreColor  = TFT_EDIT_PALLET_INPUT_CADRE,
        app.set_tft_tools_pallet_config.btnsCadreColor    = TFT_EDIT_BTN_CADRE,
        app.set_tft_tools_pallet_config.bgColor           = TFT_EDIT_PALLET_BG,
        app.set_tft_tools_pallet_config.fillColor         = TFT_EDIT_BTN_FILL;

        //app.createEditPallet(desktop_upper);

        if ( btn_enter.shortPress() ){

            desktop_upper.setFreeFont(&Dialog_bold_22);
            desktop_upper.setTextColor(TFT_WHITE, TFT_DARKGREEN);
            desktop_upper.drawRoundRect((start_btn_x - 5), 145, (start_btn_w + 10), 35, 4, TFT_WHITE);
            desktop_upper.fillRoundRect((start_btn_x - 3), 147, (start_btn_w + 6), 31, 4, TFT_DARKGREEN);
            desktop_upper.drawString(start_btn_txt, start_btn_x, 150); // start button

            desktop_upper.fillRect((intro_x - 10), (intro_y - 10), (intro_w + 10), 30, setup_bgColor);

            intro_txt       = "";
            show_edit_palet = false;
            setup_step      = 1;
        }

        
    }

    String step_txt = "";
    int step_w      = 0;
    int step_x      = 0;
    int step_y      = 0;

    /*if( setup_step == 1 ){ // set location

        step_y = 100;

        desktop_upper.fillRect(0, step_y, desktop_upper.width(), (desktop_upper.height() - step_y), setup_bgColor);

        intro_txt          = "Setting up your location";

        desktop_upper.setFreeFont(&DejaVu_Sans_Bold_11);

        intro_w      = desktop_upper.textWidth(intro_txt);
        intro_x      = ((desktop_upper.width() - intro_w) / 2);

        desktop_upper.setTextColor(TFT_WHITE, setup_bgColor);
        desktop_upper.drawString(intro_txt, intro_x, 70); // intro text

        if( !setup_country_ok ){
            
            step_txt = "Please enter your country.";
            
            desktop_upper.setFreeFont(&DejaVu_Sans_Bold_11);

            step_w = desktop_upper.textWidth(step_txt);
            step_x = ((desktop_upper.width() - step_w) / 2);

            //Serial.println(step_w);

            desktop_upper.setTextColor(TFT_WHITE, setup_bgColor);
            desktop_upper.drawString(step_txt, step_x, step_y); // intro text

            config_edit_pallet.chars_type    = 0,
            config_edit_pallet.x             = step_x,
            config_edit_pallet.y             = step_y + 30,
            config_edit_pallet.w             = 193,
            config_edit_pallet.h             = 100,
            config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
            config_edit_pallet.txtColor      = TFT_WHITE,
            config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
            config_edit_pallet.current_index = 1,
            
            app.setup_edit_pallet(config_edit_pallet);

        }
        else
        {
            setup_step = 2;
        }
    }*/

    
    desktop_upper.pushSprite(0, 0);
    
    return;
}
/**************************************************************************************************************************************************************************************
*                                                                                 VIDEO STREAM
**************************************************************************************************************************************************************************************/

bool create_video_stream_queu(){
    app.xVideoStreamQueue = xQueueCreate(2, sizeof(video_stream_config_t *));

    if( app.xVideoStreamQueue == 0 ){
        Serial.println("[QUEUE] - Failed while creating xVideoStreamQueue");
        return false;
    }

    Serial.println("[QUEUE] - xVideoStreamQueue created succesfully");

    return true;
}

void send_to_video_stream_queu(void *const pxConfig){

    // Send a pointer to a struct AMessage object.  Don't block if the queue is already full.
    if(xQueueSend(app.xVideoStreamQueue, &pxConfig, 0 ) != pdTRUE){
        Serial.println("[Error] - Faild send data to xVideoStreamQueue queue");
    }
}

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap){
  
  if ( y >= desktop_upper.height() ) return 0;

  desktop_upper.pushImage(x, y, w, h, bitmap);

  return 1;
}

void display::video_stream_init(){

    video_stream.setSwapBytes(true);
    TJpgDec.setJpgScale(1);
    TJpgDec.setCallback(tft_output); // "tft_output" is passed to the decoder

    create_video_stream_queu();
    
}

void display::config_video_stream_esperit(video_stream_config_t *config){

    //video_stream.createSprite(config->width, config->height);
    video_stream.fillRect(0, 0, config->width, config->height, TFT_WHITE);
    video_stream.setTextColor(config->txtColor, config->bgColor);
    video_stream.setFreeFont(config->font);

}

void send_video_stream_settings(String var[], String val[], int index){

    setting_link = "";

    setting_link += "http://192.168.4.2/control?";
    setting_link += "var=";
    setting_link += String(var[index]);
    setting_link += "&val=";
    setting_link += String(val[index]);

    Serial.print("Setting to send: ");Serial.println(setting_link);

    WiFiClient client;
    if(client.connect(w_server, 80)) {
        client.print("ECHO\r\n");
        client.flush();
        
        HTTPClient http;
        http.begin(setting_link.c_str()); //HTTP
        
        int httpCode = http.GET();
        Serial.print("Http Code: ");Serial.println(httpCode);

        // httpCode will be negative on error
        if(httpCode > 0) {

            // file found at server
            if(httpCode == HTTP_CODE_OK) {

                String payload = http.getString();
                Serial.print("Setting Response: ");Serial.println(payload);

            }
        }

        client.flush(); client.stop();
    
    }
}

void display::restart_video_stream(){

    app.video_stream_config = TFT_VIDEO_STREAM_CONFIG_DEFAULT();

    video_stream_init();

    config_video_stream_esperit(&app.video_stream_config);

}

void vVideoStreamTask_cb(void * arg){

    Serial.println("\n[TFT VIDEO STREAM TASK] -> Tft video stream task started\n");

    uint32_t f_length;

    for(;;){

        if( app.xVideoStreamQueue != 0 ){

            if(xQueueReceive(app.xVideoStreamQueue, &tft_video_stream_config, 0) == pdTRUE){

                //app.V_STREAM_VAR = tft_video_stream_config->command[0];
                //app.V_STREAM_VAL = tft_video_stream_config->command[1];
                app.VIDEO_X      = tft_video_stream_config->video_x;
                app.VIDEO_X      = tft_video_stream_config->video_y;

                //send_video_stream_settings(app.V_STREAM_VAR, app.V_STREAM_VAL);
            }
        }

        if(app.PAGE_NAME.equals("DESKTOP")){

            if(app.socketServerPoll()){

                app.soc_client = app.soc_server.accept();

            }

            if(app.soc_client.available()){

                v_encode_net_speed_str = "";
                v_encode_speed_str     = "";

                uint32_t t = millis();

                WebsocketsMessage msg = app.soc_client.readBlocking();
                f_length = msg.length() + 16;
                app.soc_client.send(msg.data());

                const uint8_t * buf = reinterpret_cast<const uint8_t *>(msg.c_str());

                v_encode_net_speed_str += "Net: ";
                v_encode_net_speed_str += String(millis() - t);

                TJpgDec.drawJpg(150, 25, buf, f_length); // Runs until complete image decoded

                v_encode_speed_str += "Encod: ";
                v_encode_speed_str += String(millis() - t);

                //video_stream.pushSprite(0, 0);
                video_stream.pushToSprite(&desktop_upper, 150, 25);

                
            }
            
            
        }
        
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

}

/**************************************************************************************************************************************************************************************
*                                                                                 TFT TOOLS
**************************************************************************************************************************************************************************************/

uint8_t display::getCurrentWidth(){
    return _current_width = app.tft.width();
}

uint8_t display::getCurrentHieght(){
    return _current_hieght = app.tft.height();
}

uint8_t display::getCurrentRotation(){
    return _current_rotation = app.tft.getRotation();
}

static uint8_t getTftWidth(){
    return app.getCurrentWidth();
}

static uint8_t getTftHieght() {
    return app.getCurrentHieght();
}

diplay_orientation_t tft_current_ori(){
    
    uint8_t r = app.getCurrentRotation();

    if(r == 0){
        return TFT_ORI_PORTRAT_U;
    }

    if(r == 1){
        return TFT_ORI_LANDSCAPE_L;
    }

    if(r == 2){
        return TFT_ORI_PORTRAT_D;
    }

    if(r == 3){
        return TFT_ORI_LANDSCAPE_R;
    }

    return ORI_ERROR;
}

void init_error_view(int32_t y){

    int32_t w = app.tft.width() - 100;
    int32_t h = 70;
    int32_t x = (app.tft.width() - w) / 2;

    app.tft.setViewport(x, y, w, h, true);
    app.tft.frameViewport(TFT_WHITE, -2);
    //app.tft.fillScreen(TFT_RED);

}

// in include/tft/tft_helper.h
void error_message_s(String msg, int32_t y, bool erase){

    error_msg_active = true;

    int32_t w = app.tft.width() - 100;
    int32_t h = 70;
    int32_t x = (app.tft.width() - w) / 2;

    //error_msg_active = true;

    //s.deleteSprite();

    //if(!s.created()){

        //app.tft.fillScreen(TFT_BLACK);

        app.tft.setViewport(x, y, w, h, true);
        //app.tft.frameViewport(TFT_WHITE, -2);
        //app.tft.fillScreen(TFT_RED);
        app.tft.fillRect(x, y, w, h, TFT_RED);
        app.tft.setTextColor(TFT_WHITE, TFT_RED);

        app.tft.setFreeFont(&FreeSans9pt7b);
        app.tft.drawString("Error", 5, 5);

        app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
        app.tft.drawString(msg, 5, 30);
        app.tft.resetViewport();

        return;
        
   // }

}

void error_message(TFT_eSPI t, int32_t  x, int32_t y){

    error_msg_active = true;

    t.setViewport(x, x, (app.tft.width() - 150), 50, true);
    t.frameViewport(TFT_WHITE, -2);
    t.setTextColor(TFT_WHITE, TFT_BLUE);
    t.drawString("hello", 10, 10);
    //t.resetViewport();


}

void spiffs_errors(filesio_info_t info){

    if(info != FIO_OK){

        app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
        String mount_msg  = "Failed mounting file system";
        String config_msg = "Config file not found";
        
        int32_t err_x, err_y, err_w, err_h, mount_msg_w, config_msg_w;
        
        if(info == FIO_MOUNT_FAIL){

            mount_msg_w = app.tft.textWidth(mount_msg);
            err_w       = (app.tft.width() - mount_msg_w) + 40;
            err_h       = 70;
            err_y       = (app.tft.height() - err_h) / 2;
            err_x       = (app.tft.width() - err_w) / 2;

            app.tft.drawRoundRect(err_x, err_y, err_w, err_h, 4, TFT_WHITE);
            app.tft.fillRoundRect(err_x + 2, err_y + 2, (err_w - 4), (err_h - 4), 4, TFT_RED);
                
            app.tft.setTextColor(TFT_WHITE, TFT_RED);
            app.tft.setFreeFont(&FreeSans9pt7b);
            app.tft.drawString("Error - Spiffs", err_x + 5, err_y + 5);

            app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
            app.tft.drawString(mount_msg, err_x + 5, err_y + 30);
        }

        if(info == FIO_CONFIG_FILE_NOT_EXIST){

            config_msg_w = app.tft.textWidth(config_msg);
            err_w        = (app.tft.width() - config_msg_w) + 40;
            err_h        = 70;
            err_y        = (app.tft.height() - err_h) / 2;
            err_x        = (app.tft.width() - err_w) / 2;

            app.tft.drawRoundRect(err_x, err_y, err_w, err_h, 4, TFT_WHITE);
            app.tft.fillRoundRect(err_x + 2, err_y + 2, (err_w - 4), (err_h - 4), 4, TFT_RED);

            app.tft.setTextColor(TFT_WHITE, TFT_RED);
            app.tft.setFreeFont(&FreeSans9pt7b);
            app.tft.drawString("Error - Spiffs", err_x + 5, err_y + 5);

            app.tft.setFreeFont(&DejaVu_Sans_Bold_11);
            app.tft.drawString(config_msg, err_x + 5, err_y + 30);
        }

        while(app.init_spiffs() != FIO_OK){
            delay(500);
        }

        app.tft.fillScreen(TFT_BLACK);
    }

}

void spiffs_operations_messages(String msg[]){
    // msg[0] -> existe
    // msg[1] -> create
    // msg[2] -> delete
    // msg[2] -> rename


}

void display::configTft(tft_general_config_t *config){ 

    _current_rotation = config->orientation;
    _show_logo        = config->logo_on_startup;
    _logo_delay       = config->logo_delay;
    _enable_animation = config->enable_animation;
    _show_errors      = config->show_errors;
    _show_top_bar     = config->show_top_bar;
    

    app.tft.init();
    app.tft.setSwapBytes(true);

    /* if _current_rotation not set */
    if(_current_rotation != -1){

        /* set rotation to default -> (3) */
        app.tft.setRotation(_default_rotation);
    }
    else
    {
        /* set rotation to _current_rotation */
        app.tft.setRotation(_current_rotation);
    }
    
    app.tft.fillScreen(config->bgColor);

    app.tft.fillRect(0, 0, 480, 13, TFT_RED);

    app.tft.fillRect(0, 305, 480, 320, TFT_GREY);
    app.tft.setTextColor(TFT_WHITE, TFT_RED);
    app.tft.drawCentreString("* Cyrenaica Home Protection *", CENTRE, 3, 1);
    
    app.tft.setTextColor(TFT_YELLOW, TFT_GREY);
    app.tft.drawCentreString("All rights reserved (C) 2024", CENTRE, 309, 1);

    app.tft.drawRect(0, 14, 479, 305-14, TFT_BLUE);

    if(_show_logo){
        logo();
        delay(_logo_delay);
    }

    app.tft.fillScreen(TFT_BLACK); // clear after logo screen

    filesio_info_t spiffs_info = app.init_spiffs(); // check for spiffs errors

    TFT_SPIFFS_ERRORS_CHECKS(spiffs_info); // print error message if there are any errors

    //Serial.print("ROUTER_SSID: "); Serial.println(app.getString("ROUTER_SSID"));

    if(app.SETUP_OK){

        if(config->enable_v_stream){

            app.video_stream_config = TFT_VIDEO_STREAM_CONFIG_DEFAULT();

            video_stream_init();

            config_video_stream_esperit(&app.video_stream_config);

            xTaskCreate( vVideoStreamTask_cb, "V_STREAM_TASK", 10000, NULL, tskIDLE_PRIORITY + 4, &app.V_STREAM_TASK);
        }

        app.tft.fillScreen(config->main_page_config.bgColor);
        app.tft.setTextColor(config->main_page_config.txtColor, config->main_page_config.bgColor);

        create_desktop_config_queu();

        desktop_upper.setSwapBytes(true);

        app.tft_main_page_control = {

            .width                = TFT_W, 
            .hieght               = 0,
            .upper_section_width  = TFT_W_1,
            .upper_section_hieght = 60,
            .upperBgColor         = TFT_BLUE,
            .show_upper_section   = true,
            .font                 = &DejaVu_Sans_Bold_11,
            .bgColor              = TFT_BLUE,
            .txtColor             = TFT_WHITE,
            .show_top_bar         = true,
        };

        send_to_desktop_config_queu(&app.tft_main_page_control);

        if(_show_top_bar){

            create_top_bar_config_queu();

            /* init top_bar espirite */
            top_bar.setSwapBytes(true);

            app.set_top_bar_control = {
                .show         = true,
                .width        = app.tft.width(),
                .hieght       = 40,
                .font         = &DejaVu_Sans_Bold_11,
                .bgColor      = TFT_RED,
                .txtColor     = TFT_WHITE,
                .notification = true,
                .clock        = true,
                .time         = "--:-- --",
                .day          = "--",
                .notif_message = "Empty notif",
            };

            send_to_top_bar_config_queu(&app.set_top_bar_control);
            
        }    

        app.PAGE_NAME = "DESKTOP";

        app.bk_control = {
            .backup_enabled = true, 
            .init_backup    = true, 
            .backup_now     = false, 
            .delay          = 60000
        };

        app.control_backup(&app.bk_control);
    }
    else
    {
        desktop_upper.setSwapBytes(true);
        desktop_upper.createSprite(app.tft.width(), app.tft.height(), 2);
        desktop_upper.fillRect(0, 0, desktop_upper.width(), desktop_upper.height(), setup_bgColor);
        
        app.set_tft_device_setup_screen_config = TFT_DEVICE_SETUP_SCREEN_CONFIG_DEFAULT();

        app.set_tft_tools_button_config    = TFT_TOOLS_BUTTON_CONFIG_DEFAULT();
        app.set_tft_tools_pallet_config    = TFT_TOOLS_PALLET_CONFIG_DEFAULT();
        app.set_tft_tools_wifi_scan_config = TFT_WIFI_SCAN_SCREEN_CONFIG_DEFAULT();
    }
    
}

String display::key_pad_backspace(String live_text){

    Serial.print("length before cut: ");Serial.println(live_text_length);

    //live_text_length = live_text.length() - 1;
    
    KEY_PAD_LIVE_TEXT = KEY_PAD_LIVE_TEXT.substring(0, live_text_length-1);

    live_text_length = KEY_PAD_LIVE_TEXT.length();

    Serial.print("length after cut: ");Serial.println(KEY_PAD_LIVE_TEXT.length());
    
    return KEY_PAD_LIVE_TEXT;
}

void display::onscreen_keypad(){

    uint16_t key_pad_bg_color = 0xA4F3;

    
    int16_t key_pad_w = (TFT_W - 40);
    int16_t key_pad_h = 160;
    int32_t key_pad_x = ((app.tft.width() - key_pad_w) / 2);
    int32_t key_pad_y = (desktop_upper.height() - key_pad_h) ;

    key_pad_y = key_pad_y - 10;

    if(!key_pad.created()){
        key_pad.setSwapBytes(true);
        key_pad.createSprite(key_pad_w, key_pad_h, 2);
        key_pad.fillRect(0, 0, key_pad_w, key_pad_h, key_pad_bg_color);
    }

    /* TODO: While loop */

    key_pad.setTextFont(2);
    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);

    int _x0 = 32;
    int _x1 = 0;
    int _x2 = _x1;
    int _x3 = 80;
    int _y0 = 5;
    int _y1 = 40;
    int _y2 = 70;
    int _y3 = 100;

    int input_w = key_pad_w - 120;
    int back_x  = input_w + 40;
    int back_y  = 8;

    if(key_pad_small){

        key_pad.fillRect(0, 0, key_pad_w, key_pad_h, key_pad_bg_color);

        key_pad_capital = false;
        key_pad_nums    = false;

        int smalls_length = STRING_ARRAY_SIZE(KYEPAD_ARRAY);

        if ( btn_up.shortPress() ){

            key_pad_move = true;

            key_pad_index ++;

            if(key_pad_index == -1){

                key_pad_move = false;
            }

            if(key_pad_index >= smalls_length/*elements_length*/){
                key_pad_index = -1;
            } 
        }

        if ( btn_enter.shortPress() ){

            if(key_pad_index == 1){
                key_pad_backspace(KEY_PAD_LIVE_TEXT); // private function
            }

            if(key_pad_index == 28){ // Caps button

                last_char = KEY_PAD_LIVE_TEXT;

                key_pad_w = (TFT_W - 40);
                key_pad_h = 160;
                key_pad_x = ((app.tft.width() - key_pad_w) / 2);
                key_pad_y = (app.tft.height() - key_pad_h) ;

                _x0     = 32;
                _x1     = 0;
                _x2     = _x1;
                _x3     = 80;
                _y0     = 5;
                _y1     = 40;
                _y2     = 70;
                _y3     = 100;

                input_w = key_pad_w - 120;
                back_x  = input_w + 40;
                back_y  = 8;

                key_pad.fillRect(0, 0, key_pad_w, key_pad_h, key_pad_bg_color);

                key_pad_caps    = true;

                key_pad_small   = false;
                key_pad_nums    = false;
                key_pad_capital = true;
                key_pad_move    = false;
                key_pad_index   = -1;

                
            }

            if(key_pad_index != 0 && key_pad_index != 1 && key_pad_index != 28 && key_pad_index != 29  && key_pad_index != 30  && key_pad_index != 31){

                if(key_pad_index == 32){ // space button
                    KEY_PAD_LIVE_TEXT += " ";
                }
                else
                {
                   
                    if(key_pad_index == 29){ // 123 button

                    }

                    if(key_pad_index == 30){ // OK button

                        if(app.PAGE_NAME == "LOCATION" && elements_index == 1){
                            app.setString("COUNTRY", KEY_PAD_LIVE_TEXT.c_str());

                            app.COUNTRY = app.getString("COUNTRY");
                            show_key_pad = false;
                        }
                    }

                    if(key_pad_index == 31){ // Close button

                        KEY_PAD_LIVE_TEXT  = "";
                        live_text_length   = 0;

                    }

                    if(key_pad_index == 33){ // delete button

                        KEY_PAD_LIVE_TEXT  = "";
                        live_text_length   = 0;
                    }

        
                    KEY_PAD_LIVE_TEXT += KYEPAD_ARRAY[key_pad_index];
                }

                live_text_length = KEY_PAD_LIVE_TEXT.length(); // get new length after deleting last char
            }
        }

        if ( btn_down.shortPress() ){
                
            key_pad_move = true;

            if(key_pad_index == -1){
                key_pad_move = false;
            }

            if(key_pad_index < 0){
                key_pad_move = true;
                key_pad_index = smalls_length/*elements_length*/;
            } 

            key_pad_index --;
        }
   
        for(int i = 0; i < (smalls_length); i++){

            if(key_pad_smals){
                KEY_PAD_LIVE_TEXT = last_char;
                key_pad_smals     = false;
            }

            if(i <= 1){

                if(i == 0){

                    key_pad.drawRoundRect(_x0, _y0, input_w, 25, 4, TFT_WHITE);
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KEY_PAD_LIVE_TEXT, _x0 + 10, 10);
                    

                    if(key_pad_move && key_pad_index == i){

                        key_pad.fillRoundRect(_x0 + 2, _y0 + 2, input_w-4, 21, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KEY_PAD_LIVE_TEXT, _x0 + 10, 10);
                    }
                }

                if(i == 1){

                    key_pad.setTextSize(1);

                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(back_x - 2, back_y, 46, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], back_x+8, back_y+2); //back

                    if(key_pad_move && key_pad_index == i){
                        key_pad.drawRoundRect(back_x - 2, back_y, 46, 20, 4, TFT_WHITE);
                        key_pad.fillRoundRect(back_x + 1, back_y + 2, 41, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], back_x+8, back_y+2); //a
                    }
                }

            }

            /* 2 - 11 -> a -> j */

            if(i > 1 && i <= 11){

                _x1 = _x1 + 39;
            
                key_pad.drawRoundRect(_x1-7, _y1-1, 20, 20, 4, TFT_WHITE);
                
                if(i == 2){

                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 3){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //b

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 4){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //c

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 5){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //d

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 6){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //e

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 7){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1+1, _y1); //f

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 8){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1-1); //g

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 9){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1+1); //h

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 10){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1+1, _y1+1); //i

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }

                if(i == 11){
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1-1); //j

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], _x1, _y1); //a
                    }
                }
            }

            /* 12 - 21 -> k -> t */

            if(i > 11 && i <= 21){

                _x2 = _x2 + 39;

                key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                key_pad.drawRoundRect(_x2-7, _y2-1, 20, 20, 4, TFT_WHITE);
                key_pad.drawString(KYEPAD_ARRAY[i], _x2, _y2);

                if(key_pad_move && key_pad_index == i){
                    key_pad.fillRoundRect(_x2-5, _y2+2, 16, 16, 2, SETTINGS_PAGE_BG);
                    key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x2, _y2); //a
                }
            }

            /* 23 - 27 -> u -> z */

            if(i >= 22 && i <= 27){

                _x3 = _x3 + 39;

                key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                key_pad.drawRoundRect(_x3-7, _y3-1, 20, 20, 4, TFT_WHITE);
                key_pad.drawString(KYEPAD_ARRAY[i], _x3, _y3);

                if(key_pad_move && key_pad_index == i){
                    key_pad.fillRoundRect(_x3-5, _y3+2, 16, 16, 2, SETTINGS_PAGE_BG);
                    key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                    key_pad.drawString(KYEPAD_ARRAY[i], _x3, _y3); //a
                }
            }
        
            /* 84 - 89 -> CONTROLS */

            if(i >= 28 && i <= 33){

                key_pad.setTextSize(1);

                if(i == 28){ // Caps
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(31, _y3-2, 36, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], 35, _y3);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(33, _y3, 32, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], 35, _y3);
                    }
                }

                if(i == 29){ // 123
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(72, _y3-2, 32, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], 76, _y3);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(74, _y3+2, 28, 14, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], 76, _y3);
                    }
                }

                if(i == 30){ // OK
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(334, _y3, 21, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], 338, _y3+2);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(336, _y3+2, 17, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], 338, _y3+2);
                    }
                }

                if(i == 31){ // Close
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(363, _y3, 40, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], 367, _y3+2);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(365, _y3+2, 36, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], 367, _y3+2);
                    }
                }

                if(i == 32){ // Spc
                    key_pad_w = key_pad_w - 145;
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(32, _y3+28, key_pad_w, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], 155, _y3+30);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(34, _y3+30, key_pad_w-4, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], 155, _y3+30);
                    }
                }

                if(i == 33){ // Del
                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(350, _y3+28, 53, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_ARRAY[i], 358, _y3+30);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(352, _y3+30, 49, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_ARRAY[i], 358, _y3+30);
                    }
                }
            }

            
           
        }

        
    }

    if(key_pad_capital){

        int capital_length = STRING_ARRAY_SIZE(KYEPAD_CAPITAL_ARRAY);

        key_pad_small = false;
        key_pad_nums  = false;

        if ( btn_up.shortPress() ){

            key_pad_move = true;

            key_pad_index ++;

            if(key_pad_index == -1){

                key_pad_move = false;
            }

            if(key_pad_index >= capital_length/*elements_length*/){
                key_pad_index = -1;
            } 
        }

        if ( btn_enter.shortPress() ){

            if(key_pad_index == 1){
                key_pad_backspace(KEY_PAD_LIVE_TEXT); // private function
            }

            if(key_pad_index == 28){ // Caps button

                last_char = KEY_PAD_LIVE_TEXT;

                key_pad_w = (TFT_W - 40);
                key_pad_h = 160;
                key_pad_x = ((app.tft.width() - key_pad_w) / 2);
                key_pad_y = (app.tft.height() - key_pad_h) ;

                _x0     = 32;
                _x1     = 0;
                _x2     = _x1;
                _x3     = 80;
                _y0     = 5;
                _y1     = 40;
                _y2     = 70;
                _y3     = 100;

                input_w = key_pad_w - 120;
                back_x  = input_w + 40;
                back_y  = 8;

                key_pad.fillRect(0, 0, key_pad_w, key_pad_h, key_pad_bg_color);

                key_pad_smals   = true;
            
                key_pad_small   = true;
                key_pad_nums    = false;
                key_pad_capital = false;
                key_pad_move    = false;
                key_pad_index   = -1;

            }

            if(key_pad_index != 0 && key_pad_index != 1 && key_pad_index != 28 && key_pad_index != 29  && key_pad_index != 30  && key_pad_index != 31){

                if(key_pad_index == 32){ // space button
                    KEY_PAD_LIVE_TEXT += " ";
                }
                else
                {
                   
                    if(key_pad_index == 29){ // 123 button

                    }

                    if(key_pad_index == 30){ // OK button

                        if(key_pad_smals || key_pad_capital || key_pad_nums){
                                
                            if(app.PAGE_NAME == "LOCATION" && elements_index == 1){
                                app.setString("COUNTRY", KEY_PAD_LIVE_TEXT.c_str());

                                app.COUNTRY = app.getString("COUNTRY");
                                show_key_pad = false;
                            }
                        }

                    }

                    if(key_pad_index == 31){ // Close button

                        KEY_PAD_LIVE_TEXT  = "";
                        live_text_length   = 0;

                    }

                    if(key_pad_index == 33){ // delete button

                        KEY_PAD_LIVE_TEXT  = "";
                        live_text_length   = 0;
                    }

        
                    KEY_PAD_LIVE_TEXT += KYEPAD_CAPITAL_ARRAY[key_pad_index];
                }

                live_text_length = KEY_PAD_LIVE_TEXT.length(); // get new length after deleting last char
            }
        }

        if ( btn_down.shortPress() ){
                
            key_pad_move = true;

            if(key_pad_index == -1){
                key_pad_move = false;
            }

            if(key_pad_index < 0){
                key_pad_move = true;
                key_pad_index = capital_length/*elements_length*/;
            } 

            key_pad_index --;
        }
    

        if(!key_pad_small && !key_pad_nums){

            for(int i = 0; i < (capital_length); i++){

                /* 0 - 1 -> input & back */

                if(key_pad_caps){
                    KEY_PAD_LIVE_TEXT = last_char;
                    key_pad_caps      = false;
                }
                
                if(i <= 1){

                    if(i == 0){

                        key_pad.drawRoundRect(_x0, _y0, input_w, 25, 4, TFT_WHITE);
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KEY_PAD_LIVE_TEXT, _x0 + 10, 10);

                        if(key_pad_move && key_pad_index == i){
 
                            key_pad.fillRoundRect(_x0 + 2, _y0 + 2, input_w-4, 21, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KEY_PAD_LIVE_TEXT, _x0 + 10, 10);
                        }
                    }

                    if(i == 1){

                        key_pad.setTextSize(1);

                        key_pad.drawRoundRect(back_x - 2, back_y, 46, 20, 4, TFT_WHITE);
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], back_x+8, back_y+2); //back

                        if(key_pad_move && key_pad_index == i){
                            key_pad.drawRoundRect(back_x - 2, back_y, 46, 20, 4, TFT_WHITE);
                            key_pad.fillRoundRect(back_x + 1, back_y + 2, 41, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], back_x+8, back_y+2); //back
                        }
                    }
                }
                
                /* 2 - 11 -> a -> j */

                if(i > 1 && i <= 11){

                    _x1 = _x1 + 39;
                
                    key_pad.drawRoundRect(_x1-7, _y1-1, 20, 20, 4, TFT_WHITE);
                    
                    if(i == 2){

                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 3){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //b

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 4){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //c

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 5){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //d

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 6){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //e

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 7){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1+1, _y1); //f

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 8){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1-1); //g

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 9){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1+1); //h

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 10){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1+1, _y1+1); //i

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }

                    if(i == 11){
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1-1); //j

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(_x1-5, _y1+2, 16, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x1, _y1); //a
                        }
                    }
                }

                /* 12 - 21 -> k -> t */

                if(i > 11 && i <= 21){

                    _x2 = _x2 + 39;

                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(_x2-7, _y2-1, 20, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x2, _y2);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x2-5, _y2+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x2, _y2); //a
                    }
                }

                /* 23 - 27 -> u -> z */

                if(i >= 22 && i <= 27){

                    _x3 = _x3 + 39;

                    key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                    key_pad.drawRoundRect(_x3-7, _y3-1, 20, 20, 4, TFT_WHITE);
                    key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x3, _y3);

                    if(key_pad_move && key_pad_index == i){
                        key_pad.fillRoundRect(_x3-5, _y3+2, 16, 16, 2, SETTINGS_PAGE_BG);
                        key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], _x3, _y3); //a
                    }
                }
            
                /* 28 - 33 -> CONTROLS */

                if(i >= 28 && i <= 33){

                    key_pad.setTextSize(1);

                    if(i == 28){ // Caps
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawRoundRect(31, _y3-2, 36, 20, 4, TFT_WHITE);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 35, _y3);

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(33, _y3, 32, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 35, _y3);
                        }
                    }

                    if(i == 29){ // 123
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawRoundRect(72, _y3-2, 32, 20, 4, TFT_WHITE);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 76, _y3);

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(74, _y3+2, 28, 14, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 76, _y3);
                        }
                    }

                    if(i == 30){ // OK
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawRoundRect(334, _y3, 21, 20, 4, TFT_WHITE);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 338, _y3+2);

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(336, _y3+2, 17, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 338, _y3+2);
                        }
                    }

                    if(i == 31){ // Close
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawRoundRect(363, _y3, 40, 20, 4, TFT_WHITE);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 367, _y3+2);

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(365, _y3+2, 36, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 367, _y3+2);
                        }
                    }

                    if(i == 32){ // Spc
                        key_pad_w = key_pad_w - 145;
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawRoundRect(32, _y3+28, key_pad_w, 20, 4, TFT_WHITE);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 155, _y3+30);

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(34, _y3+30, key_pad_w-4, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 155, _y3+30);
                        }
                    }

                    if(i == 33){ // Del
                        key_pad.setTextColor(TFT_WHITE, key_pad_bg_color);
                        key_pad.drawRoundRect(350, _y3+28, 53, 20, 4, TFT_WHITE);
                        key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 358, _y3+30);

                        if(key_pad_move && key_pad_index == i){
                            key_pad.fillRoundRect(352, _y3+30, 49, 16, 2, SETTINGS_PAGE_BG);
                            key_pad.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                            key_pad.drawString(KYEPAD_CAPITAL_ARRAY[i], 358, _y3+30);
                        }
                    }
                }
            }
        }
    }

    if(key_pad_nums){
        key_pad_small   = false;
        key_pad_capital = false;
    }
    
    //key_pad.pushSprite(key_pad_x, key_pad_y);
    key_pad.pushToSprite(&desktop_upper, key_pad_x, key_pad_y);
}

void display::edit_button(edit_pallet_t pallet){

    String btn_txt = "Edit";
    int btn_txt_w  = 0;
    uint16_t edit_color = 0x20E3;

    desktop_upper.fillRoundRect((pallet.x - 3), (pallet.y), (pallet.w - 2), (pallet.h - 2), 4, SETTINGS_PAGE_BG); // to hide the fill color 

    if(!edit_pallet.created()){

        edit_pallet.setSwapBytes(true);
        edit_pallet.createSprite(47, 20, 2);
        //edit_pallet.fillRect(0, 0, 47, 20, pallet.bgColor); 
    }
    else
    {
        if(pallet.edit){
            edit_pallet.deleteSprite();
            edit_pallet.setSwapBytes(true);
            edit_pallet.createSprite(pallet.w, pallet.h, 2);
            edit_pallet.fillRect(0, 0, pallet.w, pallet.h, pallet.bgColor); 
        }
        else
        {
            
            edit_pallet.setSwapBytes(true);
            edit_pallet.createSprite(47, 20, 2);
        }
    }

    edit_pallet.setTextColor(pallet.txtColor, pallet.bgColor);
    edit_pallet.setFreeFont(pallet.font);

    edit_pallet.fillRect(0, 0, edit_pallet.width(), edit_pallet.height(), SETTINGS_PAGE_BG); // to hide the fill color 
    

    if(!pallet.edit){
        btn_txt_w = edit_pallet.textWidth(btn_txt);
        desktop_upper.fillRoundRect((pallet.x - 3), (pallet.y), (pallet.w - 2), (pallet.h - 2), 4, SETTINGS_PAGE_BG);
        edit_pallet.drawRoundRect(1, 1, (btn_txt_w + 8), 18, 4, TFT_WHITE); 
        edit_pallet.drawString(btn_txt, 5, 5);
    }
    else
    {
        desktop_upper.fillRoundRect((pallet.x - 10), (pallet.y), (pallet.w + 10), (pallet.h + 10), 4, SETTINGS_PAGE_BG);
        edit_pallet.fillRoundRect(pallet.x, (pallet.y - 4), (pallet.w - 2), (pallet.h - 2), 4, SETTINGS_PAGE_BG);
        edit_pallet.drawRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, TFT_WHITE);
        edit_pallet.fillRoundRect((pallet.x + 1), (pallet.y-4), (pallet.w - 2), (pallet.h - 2), 4, SETTINGS_PAGE_BG);

        int _x, _y, _w, _h;

        int input_x = 4;
        int input_y = 4;
        int input_w = (pallet.w - 8);
        int input_h = 30;

        String edit_elements[6] = {"input", "<", "OK", "Del", ">", "Save"};

        int edit_elements_length = STRING_ARRAY_SIZE(edit_elements);
        int chars_length         = STRING_ARRAY_SIZE(KYEPAD_ARRAY);

        int l_btn_w   = edit_pallet.textWidth(edit_elements[1]);
        int ok_btn_w  = edit_pallet.textWidth(edit_elements[2]);
        int del_btn_w = edit_pallet.textWidth(edit_elements[3]);
        int r_btn_w   = edit_pallet.textWidth(edit_elements[4]);
        int sv_btn_w  = edit_pallet.textWidth(edit_elements[5]);

        if ( btn_up.shortPress() ){

            edit_move = true;

            edit_index ++;

            if(edit_index >= edit_elements_length/*elements_length*/){
                edit_index = 0;
            } 

        }

        if ( btn_enter.shortPress() ){

            if(edit_index == 0){ // input

            }

            if(edit_index == 1){ // <

                still_edit = true;

                if(pallet.chars_type == 0){
                    last_char = KYEPAD_ARRAY[edit_chars_index];
                }

                if(pallet.chars_type == 1){
                    last_char = KYEPAD_NUM_ARRAY[edit_chars_index];
                }

                

                if(edit_chars_index <= 0){
                    edit_chars_index = chars_length;
                }

                edit_chars_index--;

            }

            if(edit_index == 2){ // OK

                EDIT_LIVE_TEXT   += last_char;
                edit_chars_index = -1;
                last_char        = "";
                still_edit       = false;

            }

            if(edit_index == 3){ // Del

            }

            if(edit_index == 4){ //>

                edit_chars_index++;

                still_edit = true;

                if(pallet.chars_type == 0){
                    last_char = KYEPAD_ARRAY[edit_chars_index];
                }

                if(pallet.chars_type == 1){
                    last_char = KYEPAD_NUM_ARRAY[edit_chars_index];
                }
                
                if(edit_chars_index >= chars_length){
                    edit_chars_index = -1;
                }
            }

            if(edit_index == 5){ // Save

                String _key = pallet.key;
                _key.toUpperCase();

                app.setString(_key.c_str(), EDIT_LIVE_TEXT.c_str());

                if(config_edit_pallet.current_index == 1){
                    app.COUNTRY = app.getString(_key.c_str());
                }

                if(config_edit_pallet.current_index == 2){
                    app.CITY = app.getString(_key.c_str());
                }

                if(config_edit_pallet.current_index == 3){
                    app.TIMEZONE = app.getString(_key.c_str());
                }
                

                still_edit              = false;
                edit_move               = false;
                edit_chars_index        = -1;
                elements_index          = -1;
                config_edit_pallet.edit = false;
                edit_pallet.deleteSprite();
                desktop_upper.fillRect(0, 0, desktop_upper.width(), desktop_upper.height(), SETTINGS_PAGE_BG);

            }

        }

        if ( btn_down.shortPress() ){
                
            edit_move = true;
            

            if(edit_index <= 0){
                edit_index = edit_elements_length/*elements_length*/;
            } 

            edit_index --;
        }

        EDIT_LIVE_TEXT[0] = toupper(EDIT_LIVE_TEXT[0]);

        for(int i = 0; i < edit_elements_length; i++){

            if( i == 0 ){

                edit_pallet.drawRoundRect(input_x, input_y, input_w, input_h, 4, TFT_WHITE); // input
                edit_pallet.fillRoundRect((input_x + 2), (input_y + 2), (input_w - 4), (input_h - 4), 4, SETTINGS_PAGE_BG);
                edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                edit_pallet.drawString(EDIT_LIVE_TEXT, (input_x + 5), (input_y + 5));

                int len = edit_pallet.textWidth(EDIT_LIVE_TEXT);

                if(edit_move && edit_index == i && !still_edit){

                    edit_pallet.fillRoundRect((input_x + 2), (input_y + 2), (input_w - 4), (input_h - 4), 4, edit_color);
                    edit_pallet.setTextColor(TFT_WHITE, edit_color);

                    edit_pallet.drawString(EDIT_LIVE_TEXT, (input_x + 5), (input_y + 8));
        
                }
                else
                {
                    edit_pallet.fillRoundRect((input_x + 2), (input_y + 2), (input_w - 4), (input_h - 4), 4, edit_color);
                    edit_pallet.setTextColor(TFT_WHITE, edit_color);

                    edit_pallet.drawString(EDIT_LIVE_TEXT, (input_x + 5), (input_y + 8));
                    edit_pallet.drawString(last_char, (input_x + len + 7), (input_y + 8));
                }
            }

            if( i == 1 ){

                _x = (input_x + 15);
                _y = (input_h + 10);
                _w = (l_btn_w + 10);
                _h = (input_h - 12);

                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
                edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // <  cadre
                edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <

                if(edit_move && edit_index == i){
                    edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                    edit_pallet.setTextColor(edit_color, TFT_WHITE);
                    edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
                }
            }

            if( i == 2 ){

                _x = (_x + _w + 10);
                _y = (input_h + 10);
                _w = (ok_btn_w + 8);
                _h = (input_h - 12);

                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
                edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // ok cadre
                edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // OK

                if(edit_move && edit_index == i){
                    edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                    edit_pallet.setTextColor(edit_color, TFT_WHITE);
                    edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
                }
            }

            if( i == 3 ){

                _x = (_x + _w + 10);
                _y = (input_h + 10);
                _w = (del_btn_w + 8);
                _h = (input_h - 12);

                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
                edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // Del cadre
                edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // Del

                if(edit_move && edit_index == i){
                    edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                    edit_pallet.setTextColor(edit_color, TFT_WHITE);
                    edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
                }
            }

            if( i == 4 ){

                _x = (_x + _w + 10);
                _y = (input_h + 10);
                _w = (r_btn_w + 10);
                _h = (input_h - 12);

                
                edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // >  cadre
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
                edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // >

                if(edit_move && edit_index == i){
                    edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                    edit_pallet.setTextColor(edit_color, TFT_WHITE);
                    edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
                }
            }

            if( i == 5 ){
                _x = 55;
                _y = (_y + 30);
                _w = (sv_btn_w + 10);
                _h = (input_h - 12);

                
                edit_pallet.drawRoundRect(_x, _y, _w, _h, 4, TFT_WHITE); // Save cadre
                edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, SETTINGS_PAGE_BG);
                edit_pallet.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // Save

                if(edit_move && edit_index == i){
                    edit_pallet.fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, TFT_WHITE);
                    edit_pallet.setTextColor(edit_color, TFT_WHITE);
                    edit_pallet.drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
                }
            }
        }
    }

    edit_pallet.pushToSprite(&desktop_upper, pallet.x, pallet.y);

}

void display::top_bar_clock_screen(top_bar_config_t *config){

    //Serial.print("config->clock: ");Serial.println(config->clock);
    ///Serial.print("Time:          ");Serial.println(app.server_time_info.time_string);

    if(config->clock){
        top_bar.drawString(config->time, app.tft.width() - 80, 10);
        top_bar.drawString(config->day, app.tft.width() - 80, 20);
    }

}

void display::controlTopbar(top_bar_config_t *config) {

    diplay_orientation_t ori = tft_current_ori();

    if(config->show){

        int notif_sector_start = 0;
        int notif_sector_end   = (config->width - 100); //tft_w = 480 -> notif_sector_w = 380 // landscape
        int notif_sector_w     = (notif_sector_end - notif_sector_start); // 380

        int clock_sector_start = (notif_sector_end + 1); // one pixel after notif section = 381
        int clock_sector_end   = (config->width); // 479
        int clock_sector_w     = (clock_sector_end - clock_sector_start);


        /* init top_bar espirite */
        top_bar.setSwapBytes(true);
        
        top_bar.setFreeFont(config->font);
        top_bar.setTextColor(config->txtColor, config->bgColor);

        switch (ori) {
    
            case TFT_ORI_LANDSCAPE_R:
                /* code */
                top_bar.createSprite(config->width, config->hieght);
                top_bar.fillRect(0, 0, config->width, config->hieght, config->bgColor);
            break;

            case TFT_ORI_LANDSCAPE_L:
                /* code */
                top_bar.createSprite(config->width, config->hieght);
                top_bar.fillRect(0, 0, config->width, config->hieght, config->bgColor);
            break;

            case TFT_ORI_PORTRAT_U:
                /* code */
                top_bar.createSprite(320, config->hieght);
                top_bar.fillRect(0, 0, 320, config->hieght, config->bgColor);
            break;

            case TFT_ORI_PORTRAT_D:
                /* code */
                top_bar.createSprite(320, config->hieght);
                top_bar.fillRect(0, 0, 320, config->hieght, config->bgColor);
            break;
            
            default:
            break;
        }

        top_bar.fillRect(0, 0, config->width, config->hieght, config->bgColor);

        if(config->notification){

            if(app.bk_control.backup_enabled){

                if(app.bk_control.init_backup){

                    config->notif_message = app.getBackupTimestamp();

                    top_bar.drawString(config->notif_message, 10, 5);
                }

                if(app.bk_control.backup_now){

                    config->notif_message = "";

                    top_bar.drawString("Backup now", 10, 5);
                }

            }

            //top_bar.drawString(config->notif_message, 10, 5);
        }

        

        if(config->clock){

            int time_w = top_bar.textWidth(app.set_top_bar_control.time);
            int day_w  = top_bar.textWidth(app.set_top_bar_control.time);
            int time_x = (clock_sector_end - time_w) - (time_w/4);
            int day_x  = (clock_sector_end - day_w) - (day_w/4);

           //top_bar.drawRect(clock_sector_start, 0, clock_sector_w, config->hieght, TFT_WHITE);

            top_bar.drawString(app.set_top_bar_control.time, time_x, 10);
            top_bar.drawString(app.set_top_bar_control.day, day_x, 23);
        }

        top_bar.pushSprite(0, 0);

        
    }
    else
    {
        top_bar.deleteSprite();
    }
    
}

int biggerLength(String array[], int length){
    

    int max_v = INT_MIN;
	int max_i = 0;
    
    for ( int i = 0; i < length; i++ ){

        if ( desktop_upper.textWidth(array[i]) > max_v ){

            max_v = desktop_upper.textWidth(array[i]);
            max_i = i;
        }
	}
 
	//printf( "The max value (%d) is at index %d.\n", max_v, max_i);
 
	return max_v;


}

void display::setting_page(main_page_config_t *config){

    if(!show_key_pad && !error_msg_active){

        int _x, _y          = 0;
        uint16_t edit_color = 0x20E3;
        String pageTitle    = "Settings";

        int espiret_h = 0;

        if(!desktop_upper.created()){

            /* init desktop_upper espirite */
            desktop_upper.setSwapBytes(true);
            espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
            desktop_upper.createSprite(config->upper_section_width, espiret_h, 2);
            desktop_upper.fillRect(0, 0, config->upper_section_width, espiret_h, SETTINGS_PAGE_BG);
            
        }

        String setings_element[] = {"Back", "LOCATION", "WIFI", "SENSORS", "MEDIA", "DATETIME", "LANGUAGE"};

        int elements_length = STRING_ARRAY_SIZE(setings_element);

        desktop_upper.setFreeFont(&Dialog_bold_22);

        int page_title_x = ((desktop_upper.width() - desktop_upper.textWidth(pageTitle)) / 2);

        desktop_upper.setTextColor(TFT_BLUE, SETTINGS_PAGE_BG);
        desktop_upper.drawString(pageTitle, page_title_x, 10); // Print page title
        
        desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);

        int elems_w      = biggerLength(setings_element, elements_length);
        int elems_box_w  = elems_w + 20;
        int elems_box_h  = 50;
        int btn_x        = ((desktop_upper.width() - (elems_box_w-2)) / 2); // (elems_box_w-2) -> box left/right pixels

        String text = "";

        bool debug = false;
        
        if(debug){

            desktop_upper.setTextFont(1);
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString("Index :", 10, desktop_upper.height() - 70); desktop_upper.drawString(String(elements_index), 150, desktop_upper.height() - 70);
            desktop_upper.drawString("text  :", 10, desktop_upper.height() - 60); desktop_upper.drawString(debug_elem_name,        150, desktop_upper.height() - 60);
            desktop_upper.drawString("Move  :", 10, desktop_upper.height() - 50); desktop_upper.drawString(String(button_move),    150, desktop_upper.height() - 50);
            desktop_upper.drawString("Edit  :", 10, desktop_upper.height() - 40); desktop_upper.drawString(String(editNow),        150, desktop_upper.height() - 40);

        }

        if ( btn_up.shortPress() ){

            button_move = true;

            elements_index ++;

            if(elements_index == -1){

                button_move = false;
            }

            if(elements_index > 7/*elements_length*/){
                elements_index = -1;
            } 

        }

        if ( btn_enter.shortPress() ){

            if(elements_index == 0){
                //desktop_upper.fillRect(0, 0, desktop_upper.width(), desktop_upper.height(), config->bgColor);

                elements_index = -1;
                button_move    = false;
                editNow        = false;
                app.PAGE_NAME  = "DESKTOP";

                app.restart_video_stream();
                vTaskResume(app.V_STREAM_TASK);

                vTaskResume(app.BACKUP_SPIFFS_TASK);
                desktop_upper.deleteSprite();
            }

            if(elements_index == 1){

                elements_index = 0;
                button_move    = false;
                desktop_upper.deleteSprite();

                app.PAGE_NAME  = "LOCATION";
            }

            if(elements_index == 2){

            }

            if(elements_index == 3){

            }

            if(elements_index == 4){

            }

            if(elements_index == 5){

            }

            if(elements_index == 6){

            }
        }

        if ( btn_down.shortPress() ){
            
            button_move = true;

            if(elements_index == -1){
                button_move = false;
            }

            if(elements_index < 0){
                button_move = true;
                elements_index = 7/*elements_length*/;
            } 

            elements_index --;
        }

        for(int i = 0; i < elements_length; i++){

            if(i == 0){
                
                desktop_upper.setTextFont(2);
                desktop_upper.setTextColor(TFT_BLUE, SETTINGS_PAGE_BG);
                desktop_upper.drawRoundRect(4, 8, 39, 20, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(6, 10, 35, 16, 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], 10, 10);

                if(button_move && elements_index == i){
                    desktop_upper.setTextFont(2);
                    desktop_upper.fillRoundRect(6, 10, 35, 16, 4, TFT_WHITE);
                    desktop_upper.setTextColor(TFT_BLUE, TFT_WHITE);
                    desktop_upper.drawString(setings_element[i], 10, 10);
                }
            }

            desktop_upper.setFreeFont(&Dialog_bold_22);

            if(i == 1){
                _x = btn_x  - (elems_box_w / 2) - 5;
                _y = 40;

                desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                desktop_upper.drawRoundRect(_x, _y, elems_box_w, elems_box_h, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));

                if(button_move && elements_index == i){
                    
                    desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_WHITE, edit_color);
                    desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));
                }
            }

            if(i == 2){
                _x = btn_x  - (elems_box_w / 2) - 5;
                _y = _y + elems_box_h + 5;

                desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);     
                desktop_upper.drawRoundRect(_x, _y, elems_box_w, elems_box_h, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x), (_y + 15));

                if(button_move && elements_index == i){
                    
                    desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_WHITE, edit_color);
                    desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));
                }
            }

            if(i == 3){

                _x = btn_x  - (elems_box_w / 2) - 5;
                _y = _y + elems_box_h + 5;

                desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                desktop_upper.drawRoundRect(_x, _y, elems_box_w, elems_box_h, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x), (_y + 15));

                if(button_move && elements_index == i){
                    
                    desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_WHITE, edit_color);
                    desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));
                } 
            }

            if(i == 4){
                _x = btn_x  + (elems_box_w / 2) + 5;
                _y = 40;
                        
                desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);                    
                desktop_upper.drawRoundRect(_x, _y, elems_box_w, elems_box_h, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));

                if(button_move && elements_index == i){
                    
                    desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_WHITE, edit_color);
                    desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));
                }
            }

            if(i == 5){
                _x = btn_x  + (elems_box_w / 2) + 5;
                _y = _y + elems_box_h + 5;

                desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
                desktop_upper.drawRoundRect(_x, _y, elems_box_w, elems_box_h, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x), (_y + 15));

                if(button_move && elements_index == i){
                    
                    desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_WHITE, edit_color);
                    desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));
                }
            }

            if(i == 6){
                _x = btn_x  + (elems_box_w / 2) + 5;
                _y = _y + elems_box_h + 5;

                desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);      
                desktop_upper.drawRoundRect(_x, _y, elems_box_w, elems_box_h, 4, TFT_WHITE);
                desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, SETTINGS_PAGE_BG);
                desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x), (_y + 15));

                if(button_move && elements_index == i){
                    
                    desktop_upper.fillRoundRect(_x + 2, _y + 2, (elems_box_w - 4), (elems_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_WHITE, edit_color);
                    desktop_upper.drawString(setings_element[i], (((elems_box_w - desktop_upper.textWidth(setings_element[i])) / 2) + _x) , (_y + 15));
                }
            }
           
        } 

        desktop_upper.pushSprite(0, (app.set_top_bar_control.hieght + 1));
    }
}

void display::setting_location_page_(main_page_config_t *config){

    int _x, _y             = 0;
    uint16_t edit_color    = 0x20E3;
    String pageTitle       = "Location";

    int espiret_h          = 0;
    int text_w             = 0;
    int value_w            = 0; // The value comes from spiffs
    int line_w             = 0;
    int title_value_margin = 20;
    int btn_box_h          = 23;
    int save_w             = 0;
    int close_w            = 0;
    int btn_margin         = 50;
    int btn_line_w         = 0;

    if(!desktop_upper.created()){

        /* init desktop_upper espirite */
        desktop_upper.setSwapBytes(true);
        espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
        desktop_upper.createSprite(config->upper_section_width, espiret_h, 2);
        desktop_upper.fillRect(0, 0, config->upper_section_width, espiret_h, SETTINGS_PAGE_BG);
    }

    desktop_upper.fillRect(0, 0, desktop_upper.width(), desktop_upper.height(), SETTINGS_PAGE_BG);
    desktop_upper.setFreeFont(&Dialog_bold_22);
    desktop_upper.setTextColor(TFT_BLUE, SETTINGS_PAGE_BG);

    String setings_location_element[] = {"Back", "Country", "City", "Timezone", "Save", "Close"};
    int elements_length = STRING_ARRAY_SIZE(setings_location_element);

    save_w  = desktop_upper.textWidth(setings_location_element[4]) + 14;  // 14 is the padding R/L of button box
    close_w = desktop_upper.textWidth(setings_location_element[5]) + 14 ; // 14 is the padding R/L of button box

    btn_line_w       = save_w + btn_margin + close_w;

    int elems_w      = biggerLength(setings_location_element, elements_length);
    int page_title_x = ((desktop_upper.width() - desktop_upper.textWidth(pageTitle)) / 2);

    desktop_upper.drawString(pageTitle, page_title_x, 10); // Print page title

    if(!config_edit_pallet.edit){
       
        if ( btn_up.shortPress() ){

            button_move = true;

            config_edit_pallet.edit = false;
            //edit_pallet.drawRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, SETTINGS_PAGE_BG);
            //edit_pallet.fillRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, SETTINGS_PAGE_BG);

            elements_index ++;

            if(elements_index == -1){

                button_move = false;
            }

            if(elements_index > 7/*elements_length*/){
                elements_index = -1;
            } 

        }

        if ( btn_enter.shortPress() ){

            if(elements_index == 0){

                config_edit_pallet.edit = false;
                elements_index          = 0;
                button_move             = false;
                desktop_upper.deleteSprite();
                app.PAGE_NAME           = "SETTINGS";

            }

            if(elements_index == 1){
                button_move             = false;
                config_edit_pallet.edit = true;
            }

            if(elements_index == 2){
                button_move             = false;
                config_edit_pallet.edit = true;
            }

            if(elements_index == 3){
                button_move             = false;
                config_edit_pallet.edit = true;
            }

            if(elements_index == 4){

            }

            if(elements_index == 5){

            }

        }

        if ( btn_down.shortPress() ){
            
            button_move = true;

            config_edit_pallet.edit = false;
            //edit_pallet.drawRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, SETTINGS_PAGE_BG);
            //edit_pallet.fillRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, SETTINGS_PAGE_BG);

            if(elements_index == -1){
                button_move = false;
            }

            if(elements_index < 0){
                button_move = true;
                elements_index = 7/*elements_length*/;
            } 

            elements_index --;
        }
        
    }

    for(int i = 0; i < elements_length; i++){

        if(i == 0){

            desktop_upper.setTextFont(2);
            desktop_upper.setTextColor(TFT_BLUE, SETTINGS_PAGE_BG);
            desktop_upper.drawRoundRect(4, 8, 39, 20, 4, TFT_WHITE);
            desktop_upper.fillRoundRect(6, 10, 35, 16, 4, SETTINGS_PAGE_BG);
            desktop_upper.drawString(setings_location_element[i], 10, 10);

            if(button_move && elements_index == i){
                desktop_upper.setTextFont(2);
                desktop_upper.fillRoundRect(6, 10, 35, 16, 4, TFT_WHITE);
                desktop_upper.setTextColor(TFT_BLUE, TFT_WHITE);
                desktop_upper.drawString(setings_location_element[i], 10, 10);
            }
        }

        desktop_upper.setFreeFont(&DejaVu_Sans_Bold_11);

        if(i == 1){

            text_w  = desktop_upper.textWidth(setings_location_element[i]);
            value_w = desktop_upper.textWidth(app.COUNTRY);
            line_w  = text_w + title_value_margin + value_w;

            _x = ((desktop_upper.width() - line_w) / 2) - 100;
            _y = 60;

            /* element title */
            desktop_upper.fillRect((_x - 10), _y, (text_w + 10), (btn_box_h - 4), SETTINGS_PAGE_BG);
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(setings_location_element[i], _x , _y);

            /* element value */
            desktop_upper.fillRoundRect((_x + elems_w + title_value_margin) - 10, _y, (value_w + 8), (btn_box_h - 4), 4, SETTINGS_PAGE_BG); // clear
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(app.COUNTRY, (_x + elems_w + title_value_margin) , _y);
            
            if(elements_index == i){
                value_w = desktop_upper.textWidth(app.COUNTRY);
                desktop_upper.drawRoundRect((_x + elems_w + title_value_margin) - 4, (_y - 6), (value_w + 10), btn_box_h, 4, TFT_WHITE);    
                desktop_upper.fillRoundRect((_x + elems_w + title_value_margin) - 2, (_y - 4), (value_w + 6), (btn_box_h - 4), 4, edit_color);
                desktop_upper.setTextColor(TFT_WHITE, edit_color);
                desktop_upper.drawString(app.COUNTRY, (_x + elems_w + title_value_margin), _y);

                if(button_move && !config_edit_pallet.edit){
                    
                    config_edit_pallet.chars_type    = 0;
                    config_edit_pallet.x             = ((_x + elems_w + title_value_margin) + value_w + 10),
                    config_edit_pallet.y             = _y - 5,
                    config_edit_pallet.w             = 47,
                    config_edit_pallet.h             = 20,
                    config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
                    config_edit_pallet.txtColor      = TFT_WHITE,
                    config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
                    config_edit_pallet.current_index = i;
                    config_edit_pallet.key           = setings_location_element[i];
                }
                else
                {
                    if(!button_move){

                    }

                    if(config_edit_pallet.edit){

                        config_edit_pallet.chars_type    = 0;
                        config_edit_pallet.x             = ((_x + elems_w + title_value_margin) + value_w + 30),
                        config_edit_pallet.y             = _y - 5,
                        config_edit_pallet.w             = 160,
                        config_edit_pallet.h             = 100,
                        config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
                        config_edit_pallet.txtColor      = TFT_WHITE,
                        config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
                        config_edit_pallet.current_index = i;
                        config_edit_pallet.key           = setings_location_element[i];

                        desktop_upper.fillRoundRect((config_edit_pallet.x - 10), (config_edit_pallet.y), (config_edit_pallet.w + 10), (config_edit_pallet.h + 10), 4, SETTINGS_PAGE_BG);
                    }
                }

                app.edit_button(config_edit_pallet);
            }
        }

        if(i == 2){

            _x = ((desktop_upper.width() - line_w) / 2) - 100;
            _y = _y + 40;
            
            /* element title */
            desktop_upper.fillRect((_x - 10), _y, (text_w + 10), (btn_box_h - 4), SETTINGS_PAGE_BG); // clear
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(setings_location_element[i], _x , _y);

            /* element value */
            desktop_upper.fillRoundRect((_x + elems_w + title_value_margin) - 10, _y, (value_w + 10), (btn_box_h - 4), 4, SETTINGS_PAGE_BG); // clear
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(app.CITY, (_x + elems_w + title_value_margin) , _y);

            if(elements_index == i){

                value_w = desktop_upper.textWidth(app.CITY);

                desktop_upper.drawRoundRect((_x + elems_w + title_value_margin) - 4, (_y - 6), (value_w + 10), btn_box_h, 4, TFT_WHITE);    
                desktop_upper.fillRoundRect((_x + elems_w + title_value_margin) - 2, (_y - 4), (value_w + 6), (btn_box_h - 4), 4, edit_color);
                desktop_upper.setTextColor(TFT_WHITE, edit_color);
                desktop_upper.drawString(app.CITY, (_x + elems_w + title_value_margin) , _y);

                if(button_move && !config_edit_pallet.edit){
                    config_edit_pallet.chars_type    = 0;
                    config_edit_pallet.x             = ((_x + elems_w + title_value_margin) + value_w + 10),
                    config_edit_pallet.y             = _y - 5,
                    config_edit_pallet.w             = 47,
                    config_edit_pallet.h             = 20,
                    config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
                    config_edit_pallet.txtColor      = TFT_WHITE,
                    config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
                    config_edit_pallet.current_index = i;
                    config_edit_pallet.key           = setings_location_element[i];
                }
                else
                {
                    if(!button_move){

                    }

                    if(config_edit_pallet.edit){
                        config_edit_pallet.chars_type    = 0;
                        config_edit_pallet.x             = ((_x + elems_w + title_value_margin) + value_w + 30),
                        config_edit_pallet.y             = _y - 5,
                        config_edit_pallet.w             = 160,
                        config_edit_pallet.h             = 100,
                        config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
                        config_edit_pallet.txtColor      = TFT_WHITE,
                        config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
                        config_edit_pallet.current_index = i;
                        config_edit_pallet.key           = setings_location_element[i];

                        desktop_upper.fillRoundRect((config_edit_pallet.x - 10), (config_edit_pallet.y), (config_edit_pallet.w + 10), (config_edit_pallet.h + 10), 4, SETTINGS_PAGE_BG);
                    }
                }

                app.edit_button(config_edit_pallet);
            }
        }

        if(i == 3){

            _x = ((desktop_upper.width() - line_w) / 2) - 100;
            _y = _y + 40;
            
            /* element title */
            desktop_upper.fillRect((_x - 10), _y, (text_w + 10), (btn_box_h - 4), SETTINGS_PAGE_BG); // clear
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(setings_location_element[i], _x , _y);

            /* element value */
            desktop_upper.fillRoundRect((_x + elems_w + title_value_margin) - 10, _y, (value_w + 10), (btn_box_h - 4), 4, SETTINGS_PAGE_BG); // clear
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(app.TIMEZONE, (_x + elems_w + title_value_margin) , _y);

            if(elements_index == i){
                
                value_w = desktop_upper.textWidth(app.TIMEZONE);
                desktop_upper.drawRoundRect((_x + elems_w + title_value_margin) - 4, (_y - 6), (value_w + 10), btn_box_h, 4, TFT_WHITE);    
                desktop_upper.fillRoundRect((_x + elems_w + title_value_margin) - 2, (_y - 4), (value_w + 6), (btn_box_h - 4), 4, edit_color);
                desktop_upper.setTextColor(TFT_WHITE, edit_color);
                desktop_upper.drawString(app.TIMEZONE, (_x + elems_w + title_value_margin) , _y);

                if(button_move && !config_edit_pallet.edit){
                    config_edit_pallet.chars_type    = 1;
                    config_edit_pallet.x             = ((_x + elems_w + title_value_margin) + value_w + 10),
                    config_edit_pallet.y             = _y - 5,
                    config_edit_pallet.w             = 47,
                    config_edit_pallet.h             = 20,
                    config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
                    config_edit_pallet.txtColor      = TFT_WHITE,
                    config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
                    config_edit_pallet.current_index = i;
                    config_edit_pallet.key           = setings_location_element[i];
                }
                else
                {
                    if(!button_move){

                    }

                    if(config_edit_pallet.edit){
                        config_edit_pallet.chars_type    = 1;
                        config_edit_pallet.x             = ((_x + elems_w + title_value_margin) + value_w + 30),
                        config_edit_pallet.y             = _y - 5,
                        config_edit_pallet.w             = 160,
                        config_edit_pallet.h             = 100,
                        config_edit_pallet.bgColor       = SETTINGS_PAGE_BG, 
                        config_edit_pallet.txtColor      = TFT_WHITE,
                        config_edit_pallet.font          = &DejaVu_Sans_Bold_11,
                        config_edit_pallet.current_index = i;
                        config_edit_pallet.key           = setings_location_element[i];

                        desktop_upper.fillRoundRect((config_edit_pallet.x - 10), (config_edit_pallet.y), (config_edit_pallet.w + 10), (config_edit_pallet.h + 10), 4, SETTINGS_PAGE_BG);

                    }
                }

                app.edit_button(config_edit_pallet);
            }
            
        }

        if(i == 4){

            text_w = desktop_upper.textWidth(setings_location_element[i]);

            _x = ((desktop_upper.width() - line_w) / 2) - 85;
            _y = 200;

            desktop_upper.drawRoundRect((_x - 4), (_y - 6), (text_w + 10), btn_box_h, 4, TFT_WHITE);
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(setings_location_element[i], _x , _y);
        }

        if(i == 5){

            text_w = desktop_upper.textWidth(setings_location_element[i]);

            _x = _x + save_w + btn_margin;
            _y = 200;
            
            desktop_upper.drawRoundRect((_x - 4), (_y - 6), (text_w + 10), btn_box_h, 4, TFT_WHITE);
            desktop_upper.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            desktop_upper.drawString(setings_location_element[i], _x , _y);
        }
    }

    desktop_upper.pushSprite(0, (app.set_top_bar_control.hieght + 1));
}

void display::desktop_page(){

    if(app.PAGE_NAME.equals("DESKTOP")){

        /* init desktop_upper espirite */
        desktop_upper.setSwapBytes(true);

        int espiret_h = 0;

        if(!desktop_upper.created()){
            
            diplay_orientation_t ori = tft_current_ori();

            switch (ori) {
            
                case TFT_ORI_LANDSCAPE_R:
                    espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
                    desktop_upper.createSprite(desktop_page_control.upper_section_width, espiret_h, 2);
                    desktop_upper.fillRect(0, 0, desktop_page_control.upper_section_width, espiret_h, desktop_page_control.bgColor);
                    
                break;

                case TFT_ORI_LANDSCAPE_L:
                    espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
                    desktop_upper.createSprite(desktop_page_control.upper_section_width, espiret_h);
                    desktop_upper.fillRect(0, 0, desktop_page_control.upper_section_width, desktop_page_control.upper_section_hieght, desktop_page_control.bgColor);
                break;

                case TFT_ORI_PORTRAT_U:
                    espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
                    desktop_upper.createSprite(320, espiret_h);
                    desktop_upper.fillRect(0, 0, 320, espiret_h,desktop_page_control.bgColor);
                break;

                case TFT_ORI_PORTRAT_D:
                    espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
                    desktop_upper.createSprite(320, espiret_h);
                    desktop_upper.fillRect(0, 0, 320, espiret_h, desktop_page_control.bgColor);
                break;
                
                default:
                break;
            }
        }

        int lower_section_y = (app.set_top_bar_control.hieght + 10);
        int lower_section_h = (desktop_upper.height() - (app.set_top_bar_control.hieght + desktop_page_control.upper_section_hieght) - 1);

        int elements_length = STRING_ARRAY_SIZE(desktop_page_control.elements);

        int x, y           = 0;
        int W              = desktop_upper.width(); // This espiret width
        int H              = desktop_upper.height(); // This espiret height
        int temp_width     = 0;
        int temp_u_width   = 0;
        String C_str       = "`C";
        String F_str       = "`F";
        
        int press_str_w    = 0;
        int press_width    = 0;
        int press_u_w      = 0;
        String Press_str   = "Pressure:";
        String Press_u_str = "Pa";

        int alt_str_w      = 0;
        int alt_width      = 0;
        int alt_u_w        = 0;
        String Alt_str     = "Altitude:";
        String Alt_u_str   = "M";

        String lower_section_elements [6] = {"SETTING", "CONVERT", "SENSORS", "MEDIA", "WEB", "STREAM"};
        int lower_section_elements_length = STRING_ARRAY_SIZE(lower_section_elements);

        String active_elements[] = {lower_section_elements[0],lower_section_elements[1],lower_section_elements[2],lower_section_elements[3],lower_section_elements[4], app.tft_main_page_control.elements[1],lower_section_elements[5]};
        int active_elements_length = STRING_ARRAY_SIZE(active_elements);

        //desktop_upper.fillRect(148, 0, 300, 19, desktop_page_control.bgColor); // clear the line above the video
        desktop_upper.setTextColor(desktop_page_control.txtColor, desktop_page_control.bgColor);
        desktop_upper.setFreeFont(&DejaVu_Sans_Bold_11);
        desktop_upper.drawString("Temp:", 150, 5);
        desktop_upper.drawString(app.tft_main_page_control.elements[12], 200, 5);

        desktop_upper.drawString(v_encode_net_speed_str, 255, 5);
        desktop_upper.drawString(v_encode_speed_str, 330, 5);
        
        desktop_upper.setFreeFont(&Dialog_bold_22);
        desktop_upper.setTextColor(desktop_page_control.txtColor, desktop_page_control.bgColor);

        int elem_0_w = desktop_upper.textWidth(lower_section_elements[0]);
        int elem_1_w = biggerLength(lower_section_elements, lower_section_elements_length); //desktop_upper.textWidth(lower_section_elements[1]);
        int elem_2_w = desktop_upper.textWidth(lower_section_elements[2]);
        int elem_3_w = desktop_upper.textWidth(lower_section_elements[3]);
        int elem_4_w = desktop_upper.textWidth(lower_section_elements[4]);

        int elements_box_h           = 30;
        int elements_box_w           = (elem_1_w + 11); // because the element[1] is the longer element length
        int elements_box_margin      = 5;
        int elements_box_x_y_padding = 5;
        int elements_box_end_padding = (elem_1_w + 11); // because the element[1] is the longer element length

        desktop_upper.fillRect(0, 0, elements_box_w+15, desktop_upper.height(), desktop_page_control.bgColor); // clear buttons area
        
        int _x, _y = 0;

        uint16_t edit_color = TFT_RED;

        if ( btn_up.shortPress() ){

            button_move = true;

            elements_index ++;

            if(elements_index == -1){

                button_move = false;
            }

            if(elements_index >= active_elements_length){
                elements_index = -1;
            } 
        }

        if ( btn_enter.shortPress() ){

            if(elements_index == 0){
                elements_index = 0;
                button_move    = false;
                
                video_stream.deleteSprite();
                vTaskSuspend(app.V_STREAM_TASK);
                
                vTaskSuspend(app.BACKUP_SPIFFS_TASK);
                
                desktop_upper.deleteSprite();
                app.PAGE_NAME  = "SETTINGS";
                desktop_upper.fillRect(0, app.set_top_bar_control.hieght + 1, desktop_upper.width(), desktop_upper.height(), SETTINGS_PAGE_BG);
            }

            if(elements_index == 1){
                app.PAGE_NAME = "CONVERTERS";    
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
            }

            if(elements_index == 2){
                app.PAGE_NAME = "SENSORS"; 
            }

            if(elements_index == 3){
                app.PAGE_NAME = "MEDIA"; 
            }

            if(elements_index == 4){
                app.PAGE_NAME = "WEB"; 
            }

            if(elements_index == 5){
                app.PAGE_NAME = "D_TEMP"; 
            }
        }

        if ( btn_down.shortPress() ){

            button_move = true;

            if(elements_index == -1){
                button_move = false;
            }

            if(elements_index < 0){
                button_move = true;
                elements_index = active_elements_length;
            } 

            elements_index --;
        }

        for(int i = 0; i < active_elements_length; i++){

            if(i == 0){
                _x = 5;
                _y = lower_section_y + 20;

                desktop_upper.setTextColor(TFT_ORANGE, desktop_page_control.bgColor);
                desktop_upper.drawRoundRect(_x, _y, elements_box_w, elements_box_h, 4, TFT_WHITE);
                desktop_upper.drawString(active_elements[0], (((elements_box_w + 8) - elem_0_w) / 2), _y + elements_box_x_y_padding);
                
                if(button_move && elements_index == i){

                    desktop_upper.fillRoundRect(_x+2, _y+2, (elements_box_w - 4), (elements_box_h - 4), 4, edit_color);
                    desktop_upper.setTextColor(TFT_ORANGE, edit_color);
                    desktop_upper.drawString(active_elements[0], (((elements_box_w + 8) - elem_0_w) / 2), _y + elements_box_x_y_padding);
                }
            }

            if(i == 1){
                _x = 5;
                _y = _y + elements_box_h + elements_box_margin;

                desktop_upper.setTextColor(TFT_ORANGE, desktop_page_control.bgColor);
                desktop_upper.drawRoundRect(_x, _y, elements_box_w, elements_box_h, 4, TFT_WHITE);
                desktop_upper.drawString(active_elements[1], _x + elements_box_x_y_padding, _y + elements_box_x_y_padding);

                if(button_move && elements_index == i){
                    desktop_upper.setTextColor(TFT_ORANGE, edit_color);
                    desktop_upper.fillRoundRect(_x+2, _y+2, (elements_box_w - 4), (elements_box_h - 4), 4, edit_color);
                    desktop_upper.drawString(active_elements[1], _x + elements_box_x_y_padding, _y + elements_box_x_y_padding);
                }
            }

            if(i == 2){
                _x = 5;
                _y = _y + elements_box_h + elements_box_margin;

                desktop_upper.setTextColor(TFT_ORANGE, desktop_page_control.bgColor);
                desktop_upper.drawRoundRect(_x, _y, elements_box_w, elements_box_h, 4, TFT_WHITE);
                desktop_upper.drawString(active_elements[2], _x + elements_box_x_y_padding, _y + elements_box_x_y_padding);
                
                if(button_move && elements_index == i){
                    desktop_upper.setTextColor(TFT_ORANGE, edit_color);
                    desktop_upper.fillRoundRect(_x+2, _y+2, (elements_box_w - 4), (elements_box_h - 4), 4, edit_color);
                    desktop_upper.drawString(active_elements[2], _x + elements_box_x_y_padding, _y + elements_box_x_y_padding);
                }
            }

            if(i == 3){
                _x = 5;
                _y = _y + elements_box_h + elements_box_margin;

                desktop_upper.setTextColor(TFT_ORANGE, desktop_page_control.bgColor);
                desktop_upper.drawRoundRect(_x, _y, elements_box_w, elements_box_h, 4, TFT_WHITE);
                desktop_upper.drawString(active_elements[3], (((elements_box_w + 8) - elem_3_w) / 2), _y + elements_box_x_y_padding);

                if(button_move && elements_index == i){
                    desktop_upper.setTextColor(TFT_ORANGE, edit_color);
                    desktop_upper.fillRoundRect(_x+2, _y+2, (elements_box_w - 4), (elements_box_h - 4), 4, edit_color);
                    desktop_upper.drawString(active_elements[3], (((elements_box_w + 8) - elem_3_w) / 2), _y + elements_box_x_y_padding);
                }
            }
            
            if(i == 4){
                _x = 5;
                _y = _y + elements_box_h + elements_box_margin;

                desktop_upper.setTextColor(TFT_ORANGE, desktop_page_control.bgColor);
                desktop_upper.drawRoundRect(_x, _y, elements_box_w, elements_box_h, 4, TFT_WHITE);
                desktop_upper.drawString(active_elements[4], (((elements_box_w + 8) - elem_4_w) / 2), _y + elements_box_x_y_padding);

                if(button_move && elements_index == i){
                    desktop_upper.setTextColor(TFT_ORANGE, edit_color);
                    desktop_upper.fillRoundRect(_x+2, _y+2, (elements_box_w - 4), (elements_box_h - 4), 4, edit_color);
                    desktop_upper.drawString(active_elements[4], (((elements_box_w + 8) - elem_4_w) / 2), _y + elements_box_x_y_padding);
                }
            }

            if(i == 5){

                _x = 28;
                _y = 4;

                desktop_upper.setTextFont(7);
                desktop_upper.setTextSize(1);

                if(button_move && elements_index == i){
                    desktop_upper.setTextColor(TFT_ORANGE, edit_color);
                    desktop_upper.fillRoundRect(_x, _y, 68, 60, 4, edit_color);
                }
                else
                {
                    desktop_upper.setTextColor(desktop_page_control.txtColor, desktop_page_control.bgColor);
                    desktop_upper.fillRoundRect(_x, _y, 68, 60, 4, desktop_page_control.bgColor);
                }

                temp_width = desktop_upper.textWidth(app.tft_main_page_control.elements[1]);

                x = 30; y = 10;
                
                if(app.tft_main_page_control.temp_unit == 0){
                    desktop_upper.drawString(app.tft_main_page_control.elements[1], x, y);
                }

                if(app.tft_main_page_control.temp_unit == 1){
                    desktop_upper.drawString(app.tft_main_page_control.elements[2], x, y);
                }

                int u_x    = x;
                int unit_x = (u_x + temp_width + 5);
                
                desktop_upper.setTextFont(4);
                desktop_upper.setTextSize(1);
                desktop_upper.setTextColor(desktop_page_control.txtColor, desktop_page_control.bgColor);

                if(app.tft_main_page_control.temp_unit == 0){
                    temp_u_width = desktop_upper.textWidth(C_str);
                    desktop_upper.drawString(C_str, unit_x, y + 28);
                }

                if(app.tft_main_page_control.temp_unit == 1){
                    temp_u_width = desktop_upper.textWidth(F_str);
                    desktop_upper.drawString(F_str, unit_x, y + 5);
                }
            }

            if(i == 6){
                desktop_upper.drawRoundRect(146, 21, 328, 248, 4, TFT_WHITE);/*  */
                desktop_upper.drawRoundRect(147, 22, 326, 246, 4, desktop_page_control.bgColor);/*  */
                desktop_upper.drawRoundRect(148, 23, 324, 244, 4, desktop_page_control.bgColor);/*  */

                if(button_move && elements_index == i){
                    desktop_upper.drawRoundRect(147, 22, 326, 246, 4, TFT_RED);/*  */
                    desktop_upper.drawRoundRect(148, 23, 324, 244, 4, TFT_RED);/*  */

                }
            }
        }

        desktop_upper.pushSprite(0, (app.set_top_bar_control.hieght + 1));
    }
}

void display::controlDesktopPage(main_page_config_t *config){

    desktop_page_control.bgColor              = config->bgColor;

    int elements_length = STRING_ARRAY_SIZE(config->elements);

    for(int i = 0; i < elements_length; i++){
        desktop_page_control.elements[i]      = config->elements[i];
    }

    desktop_page_control.font                 = config->font;
    desktop_page_control.width                = config->width;
    desktop_page_control.hieght               = config->hieght;
    desktop_page_control.show_top_bar         = config->show_top_bar;
    desktop_page_control.show_upper_section   = config->show_upper_section;
    desktop_page_control.upper_section_width  = config->upper_section_width;
    desktop_page_control.upper_section_hieght = config->upper_section_hieght;
    desktop_page_control.upperBgColor         = config->upperBgColor;
    desktop_page_control.temp_unit            = config->temp_unit;
    desktop_page_control.txtColor             = config->txtColor;
    
    
}



