#include "setup_screen.h"

#include <Adafruit_GFX.h>
#include "Application.h"
#include "Fonts/RalmoneRegular10.h"
#include "Fonts/PlayfairDisplay10.h"

int location_set_type_index  = -1; // to choose location setting type
int setup_elements_index     = -1;
int setup_router_setup_index = 0;
bool setup_button_move       = false;

int _location_set_type       = 0; // 0 = Automaticly, 1 = Manualy

edit_pallet_t setup_edit_pallet;


InputButton start_setup_btn_edit(BUTTON_EDIT, true, ACTIVE_HIGH);
InputButton setup_btn_up(BUTTON_UP, true, ACTIVE_HIGH);
InputButton setup_btn_enter(BUTTON_ENTER, true, ACTIVE_HIGH);
InputButton setup_btn_down(BUTTON_DOWN, true, ACTIVE_HIGH);


TFT_eSprite setup_view        = TFT_eSprite(&app.tft); // Sprite for desktop top bar

setup_screen::setup_screen(){

    _main_title_txt     = "Setup Your New Device";
    _intro_txt          = "";
    _start_btn_txt      = "Start Setup";
    _start_btn_info_txt = "Press ok button to start";
    _device_name        = DEF_DEVICE_NAME;
    _device_model       = DEF_DEVICE_MODEL;
    _firmware_ver       = DEF_DEVICE_FIRMWARE_VER;
    _firmware_name      = DEF_DEVICE_FIRMWARE_NAME;
    _has_psram          = DEF_DEVICE_HAS_PSRAM;
    _psram_size         = DEF_DEVICE_PSRAM_SIZE;
    _flash_size         = DEF_DEVICE_MEM;
    _ram_size           = DEF_DEVICE_RAM_SIZE;
    _has_wifi           = DEF_DEVICE_HAS_WIFI;
    _has_ble            = DEF_DEVICE_HAS_BLE;
    _title_x            = 0;
    _title_y            = 30;
    _title_w            = 0;
    _title_h            = 0;
    _intro_x            = 0;
    _intro_y            = 70;
    _intro_w            = 0;
    _intro_h            = 0;
    _start_btn_x        = 0;
    _start_btn_y        = 0;
    _start_btn_w        = 0;
    _start_btn_h        = 0;
    _start_btn_info_x   = 0;
    _start_btn_info_y   = 0;
    _start_btn_info_w   = 0;
    _start_btn_info_h   = 0;

    _setup_step         = -1;
    
    _setup_bgColor      = TFT_SETUP_BG;
    _show_edit_palet    = false;

    _setup_country_ok   = false;
}
setup_screen::~setup_screen(){}

void setup_screen::main_setup_display(){

    _title_y       = app.set_tft_device_setup_screen_config.title_y;
    _setup_bgColor = app.set_tft_device_setup_screen_config.setup_bgColor;
    _setup_step    = app.set_tft_device_setup_screen_config.setup_step;

    if(!setup_view.created()){

        setup_view.setSwapBytes(true);
        setup_view.createSprite(app.tft.width(), app.tft.height(), 2);
    }
    
    setup_view.fillRect(0, 0, setup_view.width(), setup_view.height(), _setup_bgColor);

    int cadre_x = 10;
    int cadre_y = 20;
    int cadre_w = (app.tft.width() - 20);
    int cadre_h = (app.tft.height() - 60);

    int box_w; // 400
    int box_h;
    int box_x;
    int box_y;
    int box_margin; 
    int first_block_x;     

    int _x, _y, _w1, _h1, _w2, _h2;

    if( _setup_step == -1 ){ // setup intro page

        String _name = "Cyrenaica MEDIA BOX";

        setup_view.setFreeFont(&Dialog_bold_22);
        setup_view.setTextColor(TFT_WHITE, _setup_bgColor);

        _title_w      = setup_view.textWidth(_name);
        _title_h      = setup_view.fontHeight();
        _title_x      = (setup_view.width() - _title_w) / 2;

        setup_view.drawString(_name, (_title_x - 1), _title_y); // Print page title

        int box_w                    = (setup_view.width() - 80); // 400
        int box_h                    = (setup_view.height() - 210);
        int box_x                    = (setup_view.width() - box_w) / 2;
        int box_y                    = (_title_h + 40);
        int box_margin               = box_x; 
        int first_block_x            = (box_x + 20);   

        int img_lin_y = (box_y + box_h + 2);

        setup_view.setFreeFont(&DejaVu_Sans_Bold_11);

        String labels_array[10] = {"NAME", "MODEL", "WIFI", "BLE", "FLASH", "RAM S", "PS S", "PSRAM S", "FIRMW", "FW VER"};
        String values_array[10] = {_device_name, _device_model, _has_wifi, _has_ble, _flash_size, _ram_size, _has_psram, _psram_size, _firmware_name, _firmware_ver};
        String units_array [2]  = {"KB", "MB"};
        
        int elements_length = STRING_ARRAY_SIZE(labels_array);

        int label_longest_w      = biggerLength(labels_array, elements_length);
        int value_longest_w      = biggerLength(values_array, elements_length);
        int txt_start_x          = box_x;
        int txt_end_x            = box_w;
        int label_margin         = (label_longest_w + 10);
        int value_margin         = (value_longest_w);
        int scnd_block_w; 
        int scnd_block_x;

        for(int i = 0; i < elements_length; i++){

            for(int x = 0; x < elements_length; x++){

                if(i == 0 && x == 0){

                    _x = first_block_x;
                    _y = box_y + 10;

                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);

                }

                if(i == 1 && x == 1){
                    
                    _w1 = setup_view.textWidth(labels_array[i]);
                    _w2 = setup_view.textWidth(values_array[x]);

                    scnd_block_w = (_w1 + _w2 + label_margin);

                    _x = (box_w + (box_margin - 20)) - scnd_block_w + 30;

                    scnd_block_x = _x;

                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 
                    
                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                }

                if(i == 2 && x == 2){

                    _x = first_block_x;
                    _y = _y + 20;
                
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                }

                if(i == 3 && x == 3){

                    _x = scnd_block_x; 
                    
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                }

                if(i == 4 && x == 4){

                    _x = first_block_x;
                    _y = _y + 20;
                
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                    
                    _w2 = setup_view.textWidth(values_array[x]);
                    _x = (_x + _w2 + 5);
                    setup_view.drawString(units_array[1], _x-1, _y);

                    
                }

                if(i == 5 && x == 5){

                    _x = scnd_block_x; 
                    
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);

                    _w2 = setup_view.textWidth(values_array[x]);
                    _x = (_x + _w2 + 5);
                    setup_view.drawString(units_array[0], _x-1, _y);
                }

                if(i == 6 && x == 6){

                    _x = first_block_x;
                    _y = _y + 20;
                
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                }

                if(i == 7 && x == 7){

                    _x = scnd_block_x; 
                    
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);

                    _w2 = setup_view.textWidth(values_array[x]);
                    _x = (_x + _w2 + 5);
                    setup_view.drawString(units_array[1], _x, _y);
                }

                if(i == 8 && x == 8){

                    _x = first_block_x;
                    _y = _y + 20;
                
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                }

                if(i == 9 && x == 9){

                    _x = scnd_block_x; 
                    
                    setup_view.setTextColor(TFT_EDIT_BTN_FILL, _setup_bgColor);
                    setup_view.drawString(labels_array[i], _x, _y);

                    _x = (_x + label_margin + 20); 

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    setup_view.drawString(values_array[x], _x, _y);
                }
            }
        }
    
        setup_view.drawRoundRect(cadre_x, cadre_y, cadre_w, cadre_h, 4, TFT_EDIT_BTN_CADRE); // screen cadre
        setup_view.drawRoundRect(box_x, box_y, box_w, box_h, 4, TFT_EDIT_BTN_CADRE); // device specifc cadre

        int protocols_box_x = box_x;
        int protocols_box_y = img_lin_y;
        int protocols_box_w = box_w;
        int protocols_box_h = 48;

        int img_x = (protocols_box_x + 10);
        int img_w = 48;

        setup_view.fillRect(protocols_box_x, protocols_box_y, protocols_box_w, protocols_box_h, TFT_BLACK);

        setup_view.pushImage(img_x, protocols_box_y, 48, 48, wifi_sign);

        img_x = (app.tft.width() - 100) / 2;

        setup_view.pushImage(img_x, protocols_box_y+8, 100, 32, espnow_logo);

        img_x = (protocols_box_w); // 42 = 32 -> img w + 10 -> margin

        setup_view.pushImage(img_x, protocols_box_y+8, 32, 32, bluetooth_sign);

        setup_view.setTextFont(2);
        String start_str = "CLIK OK BUTTON TO START";

        _x = (app.tft.width() - setup_view.textWidth(start_str)) / 2;
        _y = protocols_box_y + protocols_box_h + 20;

        setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
        setup_view.drawString(start_str, _x, _y, 2);

        if ( start_setup_btn_edit.shortPress() ){

            setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
            setup_view.fillRect(0, 0, setup_view.width(), setup_view.height(), _setup_bgColor);
            _intro_txt       = "";
            _setup_step      = 0;
            app.set_tft_device_setup_screen_config.setup_step = _setup_step;
        }
    }
    
    if( _setup_step == 0 ){ // setup home page

        setup_view.fillRect(0, 0, setup_view.width(), setup_view.height(), _setup_bgColor);

        setup_view.setFreeFont(&Dialog_bold_22);

        _title_w      = setup_view.textWidth(_main_title_txt);
        _title_h      = setup_view.fontHeight();
        _title_x      = ((setup_view.width() - _title_w) / 2);

        setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
        setup_view.drawString(_main_title_txt, _title_x, _title_y); // Print page title

        setup_view.setFreeFont(&DejaVu_Sans_Bold_14);

        _intro_txt          = "Starting setup your new device, be ready!";

        _intro_w      = setup_view.textWidth(_intro_txt);
        _intro_x      = ((setup_view.width() - _intro_w) / 2);
        _intro_y      = _title_y + 30;

        setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
        setup_view.drawString(_intro_txt, (_intro_x - 1), _intro_y); // intro text / _intro_x - 1 -> to hide the black line at start

        setup_view.setFreeFont(&Dialog_bold_22);

        _start_btn_w  = setup_view.textWidth(_start_btn_txt);
        _start_btn_x  = ((setup_view.width() - _start_btn_w) / 2);

        setup_view.setTextColor(TFT_WHITE, TFT_ORANGE);
        setup_view.drawRoundRect((_start_btn_x - 5), 145, (_start_btn_w + 10), 35, 4, TFT_WHITE);
        setup_view.fillRoundRect((_start_btn_x - 3), 147, (_start_btn_w + 6), 31, 4, TFT_ORANGE);
        setup_view.drawString(_start_btn_txt, _start_btn_x, 150); // start button

        setup_view.setTextFont(2);
        _start_btn_info_w  = setup_view.textWidth(_start_btn_info_txt);
        _start_btn_info_x  = ((setup_view.width() - _start_btn_info_w) / 2);

        setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
        setup_view.drawString(_start_btn_info_txt, _start_btn_info_x, 190); 

        if ( start_setup_btn_edit.shortPress() ){

            setup_view.setFreeFont(&Dialog_bold_22);
            setup_view.setTextColor(TFT_WHITE, TFT_DARKGREEN);
            setup_view.drawRoundRect((_start_btn_x - 5), 145, (_start_btn_w + 10), 35, 4, TFT_WHITE);
            setup_view.fillRoundRect((_start_btn_x - 3), 147, (_start_btn_w + 6), 31, 4, TFT_DARKGREEN);
            setup_view.drawString(_start_btn_txt, _start_btn_x, 150); // start button

            _intro_txt       = "";
            _setup_step      = 1;
            app.set_tft_device_setup_screen_config.setup_step = _setup_step;
        }

    }

    String step_txt = "";
    int step_w      = 0;
    int step_x      = 0;
    int step_y      = 0;

    if( _setup_step == 1 ){ // choose setting time automaticly or manualy

        setup_view.fillRect(0, 0, setup_view.width(), setup_view.height(), _setup_bgColor);

        _main_title_txt = "Setting location & Timezone";
        _intro_txt      = "Choose the way setting up your Timezone";

        setup_view.setFreeFont(&Dialog_bold_22);

        _x  = (app.tft.width() - setup_view.textWidth(_main_title_txt)) / 2;
        _y  = 10;

        setup_view.setTextColor(TFT_BLUE, _setup_bgColor);
        setup_view.drawString(_main_title_txt, (_x - 1), _y); // Print page title

        setup_view.setFreeFont(&DejaVu_Sans_Bold_16);

        _x  = (app.tft.width() - setup_view.textWidth(_intro_txt)) / 2;
        _y  = _y + 30;

        setup_view.setTextColor(TFT_BROWN, _setup_bgColor);
        setup_view.drawString(_intro_txt, (_x - 1), _y); // Print page intro

        String location_setings_type_elements[] = {"Manualy", "Auto", "Exit", "Back"};
        int elements_length                     = STRING_ARRAY_SIZE(location_setings_type_elements);

        setup_view.setFreeFont(&Dialog_bold_22);

        int elems_w  = app.str_array_bigger_length(setup_view, location_setings_type_elements, elements_length);

        if ( setup_btn_up.shortPress() ){

            location_set_type_index ++;

            if(location_set_type_index > 3/*elements_length*/){
                location_set_type_index = 0;
            } 
        }

        if ( setup_btn_down.shortPress() ){
            
            location_set_type_index --;

            if(location_set_type_index < 0){
                location_set_type_index = 3/*elements_length*/;
            } 

            
        }

        int lin_w   = 0;
        int cadre_x = 0;
        int cadre_w = 0;

        int upper_cadre_x = 0, upper_cadre_y = (_y + 30), upper_cadre_w = 0;
        
        int lower_cadre_w = 0;
        int lower_cadre_x = 0;//(setup_view.width() - lower_cadre_w) / 2;
        int lower_cadre_y = 0;

        int elems_mid_margin = 80;

        for(int i = 0; i < elements_length; i++){

            if( i == 0 ){ // Manualy

                setup_view.setFreeFont(&Dialog_bold_22);

                lin_w   = (elems_w * 2) + elems_mid_margin;
                _x      = (setup_view.width() - lin_w) / 2;
                _x      = (_x - 1);
                cadre_x = (_x - 5);
                cadre_w = (elems_w + 10);
                _y      = (_y + 100);

                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(location_setings_type_elements[i], (_x - 1), _y); // Print manualy btn
                setup_view.drawRoundRect(cadre_x, (_y - 6), cadre_w, 35, 4, TFT_WHITE); 

                if(location_set_type_index == i){

                    setup_view.fillRoundRect((cadre_x + 3), (_y - 4), (cadre_w - 4), 31, 4, TFT_BLACK); 
                    setup_view.setTextColor(TFT_WHITE, TFT_BLACK);
                    setup_view.drawString(location_setings_type_elements[i], _x, _y); // Print manualy btn

                    setup_view.setFreeFont(&DejaVu_Sans_Bold_12);

                    String str    = "Not need internet connection";
                    upper_cadre_w = setup_view.textWidth(str);

                    upper_cadre_x = (app.tft.width() - upper_cadre_w) / 2;

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    
                    setup_view.drawString(str, upper_cadre_x, (upper_cadre_y + 10)); // Print manualy btn

                    lower_cadre_x = cadre_x;
                    lower_cadre_y = (_y + 40);
                    lower_cadre_w = (lin_w + 14);

                    setup_view.drawRoundRect(lower_cadre_x, lower_cadre_y, lower_cadre_w, 80, 2, 0x9492); 
                    setup_view.fillRoundRect((lower_cadre_x + 3), (lower_cadre_y + 3), (lower_cadre_w - 5), 74, 2, 0x2945);

                    setup_view.setTextColor(TFT_ORANGE, 0x2945);
                    setup_view.setFreeFont(&DejaVu_Sans_Bold_12);

                    setup_view.drawString("* Must provide time zone.", (lower_cadre_x + 10), (lower_cadre_y + 10)); // Print manualy btn
                    setup_view.drawString("* Must set time manualy.", (lower_cadre_x + 10), (lower_cadre_y + 30));  // Print manualy btn

                    if ( setup_btn_enter.shortPress() ){
                        location_set_type_index                           = -1;
                        _location_set_type                                = 1;
                        app.set_tft_device_setup_screen_config.setup_step = 2;

                    }
                } 
            }

            if( i == 1 ){  // Auto

                setup_view.setFreeFont(&Dialog_bold_22);

                lower_cadre_x = cadre_x;
                
                _x      = (cadre_x + cadre_w + elems_mid_margin);
                _x      = (_x - 1);
                cadre_x = (_x - 5);
                cadre_w = (elems_w + 10);
                
                _x      = (_x + 24); 

                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(location_setings_type_elements[i], (_x - 1), _y); // Print auto btn
                setup_view.drawRoundRect(cadre_x, (_y - 6), cadre_w, 35, 4, TFT_WHITE); 

                if(location_set_type_index == i){
                    setup_view.fillRoundRect((cadre_x + 3), (_y - 4), (cadre_w - 4), 31, 4, TFT_BLACK); 
                    setup_view.setTextColor(TFT_WHITE, TFT_BLACK);
                    setup_view.drawString(location_setings_type_elements[i], _x, _y); // Print auto btn

                    setup_view.setFreeFont(&DejaVu_Sans_Bold_12);

                    String str    = "Will pass router setting before";
                    upper_cadre_w = setup_view.textWidth(str);

                    upper_cadre_x = (app.tft.width() - upper_cadre_w) / 2;

                    setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                    
                    setup_view.drawString(str, upper_cadre_x, (upper_cadre_y + 10)); // Print manualy btn

                    lower_cadre_y = (_y + 40);
                    lower_cadre_w = (lin_w + 14);

                    setup_view.drawRoundRect(lower_cadre_x, lower_cadre_y, lower_cadre_w, 80, 2, 0x9492); 
                    setup_view.fillRoundRect((lower_cadre_x + 3), (lower_cadre_y + 3), (lower_cadre_w - 5), 74, 2, 0x2945);

                    setup_view.setTextColor(TFT_ORANGE, 0x2945);
                    setup_view.setFreeFont(&DejaVu_Sans_Bold_12);

                    setup_view.drawString("* Timezone will be set automaticly.", (lower_cadre_x + 10), (lower_cadre_y + 10)); // Print manualy btn
                    setup_view.drawString("* Time will be set automaticly.", (lower_cadre_x + 10), (lower_cadre_y + 30)); // Print manualy btn

                    if ( setup_btn_enter.shortPress() ){
                        location_set_type_index                           = -1;
                        _location_set_type                                = 0;
                        app.set_tft_device_setup_screen_config.setup_step = 2;
                    }
                }
            }

            if( i == 2 ){  // Exit

                setup_view.setFreeFont(&DejaVu_Sans_Bold_14);

                _x      = 20;
                
                cadre_x = (_x - 5);
                cadre_w = (setup_view.textWidth(location_setings_type_elements[i]) + 8);
                _y      = (setup_view.height() - 30);

                _x      = (_x - 1);

                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(location_setings_type_elements[i], _x-1, _y); // Print Exit btn
                setup_view.drawRoundRect(cadre_x, _y-3, cadre_w, 20, 4, TFT_WHITE); 

                if(location_set_type_index == i){
                    setup_view.fillRoundRect(cadre_x+2, _y-2, cadre_w-3, 18, 4, TFT_BLACK); 
                    setup_view.setTextColor(TFT_WHITE, TFT_BLACK);
                    setup_view.drawString(location_setings_type_elements[i], _x, _y); // Print Exit btn

                    if ( setup_btn_enter.shortPress() ){ // Exit button
                        location_set_type_index                           = -1;
                        app.set_tft_device_setup_screen_config.setup_step = -1;

                    }


                }
            }

            if( i == 3 ){  // Back

                setup_view.setFreeFont(&DejaVu_Sans_Bold_14);

                _x      = (cadre_x + cadre_w + 30);
                
                cadre_x = (_x - 5);
                cadre_w = (setup_view.textWidth(location_setings_type_elements[i]) + 8);
                _y      = (setup_view.height() - 30);

                _x      = (_x - 1);

                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(location_setings_type_elements[i], _x, _y); // Print Exit btn
                setup_view.drawRoundRect(cadre_x, _y-3, cadre_w, 20, 4, TFT_WHITE); 

                if(location_set_type_index == i){
                    setup_view.fillRoundRect(cadre_x+2, _y-2, cadre_w-3, 18, 4, TFT_BLACK); 
                    setup_view.setTextColor(TFT_WHITE, TFT_BLACK);
                    setup_view.drawString(location_setings_type_elements[i], _x, _y); // Print Exit btn

                    if ( setup_btn_enter.shortPress() ){ // Back button
                        location_set_type_index                           = -14;
                        app.set_tft_device_setup_screen_config.setup_step = 0;

                    }
                }
            }
        }
    }

    app.setString("ROUTER_PASS", "EMPTY");

    if( _setup_step == 2 ){ // set timezone type screen

        if( _location_set_type == 0 ){ // Automaticly

            _main_title_txt = "";
            _intro_txt      = "";
            _x = 0;
            _y = 0;

            _main_title_txt = "Setting timezone automaticly";
            _intro_txt      = "Setup internet connection (Router)";
            String header   = "Router SSID: After scan done";

            setup_view.setFreeFont(&Dialog_bold_22);

            _x  = (app.tft.width() - setup_view.textWidth(_main_title_txt)) / 2;
            _y  = 10;

            setup_view.setTextColor(TFT_BLUE, _setup_bgColor);
            setup_view.drawString(_main_title_txt, (_x - 1), _y); // Print page title

            setup_view.setFreeFont(&DejaVu_Sans_Bold_16);

            _x  = (app.tft.width() - setup_view.textWidth(_intro_txt)) / 2;
            _y  = _y + 30;

            setup_view.setTextColor(TFT_BROWN, _setup_bgColor);
            setup_view.drawString(_intro_txt, (_x - 1), _y); // Print page intro

            _y  = _y + 20;

            app.set_tft_tools_wifi_scan_config.targetBgColor = _setup_bgColor;

            app.set_tft_tools_wifi_scan_config.x = 0;
            app.set_tft_tools_wifi_scan_config.y = _y;
            app.set_tft_tools_wifi_scan_config.w = app.tft.width();
            app.set_tft_tools_wifi_scan_config.h = (app.tft.height() - _y);
            
            int header_w = 0;
            int ssid_w   = 0;

            if(!app.scanResultsReady()){

                setup_view.setFreeFont(&DejaVu_Sans_Bold_16);
                header_w = setup_view.textWidth(header);

                if ( setup_btn_enter.shortPress()  && !app.get_wifi_scan_done()){
                    app.set_start_wifi_scan(true);
                    app.setScanResultsStarted(true);
                }

                _x  = (app.tft.width() - header_w) / 2;
                _y  = _y + 20;

                setup_view.setFreeFont(&DejaVu_Sans_Bold_16);
                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(header, _x, _y); // Print page title

                app.tft_wifi_scan(setup_view);
            }

            if(app.scanResultsReady()){

                _x = (_x - 3);
                _y = (_y - 3);

                setup_view.fillRect(_x, _y, app.tft.width(), 30, _setup_bgColor);

                app.ROUTER_SSID    = app.getString("ROUTER_SSID");

                setup_view.setFreeFont(&Dialog_bold_22);
                ssid_w = setup_view.textWidth(app.ROUTER_SSID);

                header = "Router SSID";

                setup_view.setFreeFont(&DejaVu_Sans_Bold_16);

                header_w = setup_view.textWidth(header);

                _x  = 100;
                _y = (_y + 50);

                setup_view.setFreeFont(&DejaVu_Sans_Bold_16);
                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(header, (_x - 2), _y); // Router SSID

                header_w = setup_view.textWidth(header);

                _x  = 100;
                _y = (_y + 20);

                setup_view.setFreeFont(&Dialog_bold_22);
                setup_view.setTextColor(TFT_BLUE, _setup_bgColor);
                setup_view.drawString(app.ROUTER_SSID, (_x - 2), _y); // app.ROUTER_SSID


                header = "Router PASS";

                setup_view.setFreeFont(&DejaVu_Sans_Bold_16);

                header_w = setup_view.textWidth(header);

                header_w = header_w + ssid_w;

                _x  = 100;
                _y = (_y + 40);

                setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
                setup_view.drawString(header, (_x - 2), _y); // Router PASS

                app.ROUTER_PASS = app.getString("ROUTER_PASS");

                _x  = 100;
                _y  = (_y + 40);

                setup_view.setFreeFont(&DejaVu_Sans_Bold_16);

                String router_setup_elements[] = {"Assign", "Edit", "Show", "Back"};
                int router_setup_elements_length                     = STRING_ARRAY_SIZE(router_setup_elements);

                if(app.ROUTER_PASS.equals("") || app.ROUTER_PASS.equals("EMPTY")){

                    if ( setup_btn_up.shortPress() ){

                        if(setup_router_setup_index > 1/*elements_length*/){
                            setup_router_setup_index = 0;
                        } 

                        setup_router_setup_index++;

                    }

                    if ( setup_btn_enter.shortPress() ){


                    }

                    if ( setup_btn_down.shortPress() ){

                        if(setup_router_setup_index < 0/*elements_length*/){
                            setup_router_setup_index = 1;
                        }

                        setup_router_setup_index--;

                    }

                    header = "Press to assign";

                    header_w = setup_view.textWidth(header);

                    setup_view.setTextColor(TFT_BLUE, _setup_bgColor);
                    setup_view.drawString(header, (_x - 2), _y); // app.ROUTER_SSID

                    for(int i = 0; i < router_setup_elements_length; i++){

                        if(i == 0){

                        }

                    }

                    _x = (_x + header_w + 15);
                    _y = (_y - 6);

                    app.set_tft_tools_button_config.button_x             = (_x - 1);
                    app.set_tft_tools_button_config.button_y             = _y;
                    app.set_tft_tools_button_config.btn_txt              = "Assign";
                    app.set_tft_tools_button_config.bgColor              = _setup_bgColor;
                    app.set_tft_tools_button_config.targetBgColor        = _setup_bgColor;

                    app.createEditButton(setup_view);

                    if(setup_router_setup_index == 0){

                    }

                    


                }
                else
                {

                    if ( setup_btn_up.shortPress() ){

                        if(setup_router_setup_index > 1/*elements_length*/){
                            setup_router_setup_index = 0;
                        } 

                        setup_router_setup_index++;

                    }

                    if ( setup_btn_enter.shortPress() ){


                    }

                    if ( setup_btn_down.shortPress() ){

                        if(setup_router_setup_index < 0/*elements_length*/){
                            setup_router_setup_index = 1;
                        }

                        setup_router_setup_index--;

                    }

                    header = app.paswwordToStarts(app.ROUTER_PASS);

                    header_w = setup_view.textWidth(header);

                    setup_view.setTextColor(TFT_BLUE, _setup_bgColor);
                    setup_view.drawString(header, (_x - 2), _y); // app.ROUTER_SSID

                    _x = (_x + header_w + 15);
                    _y = (_y - 6);

                    app.set_tft_tools_button_config.button_x             = (_x - 1);
                    app.set_tft_tools_button_config.button_y             = _y;
                    app.set_tft_tools_button_config.btn_txt              = "Show";
                    app.set_tft_tools_button_config.bgColor              = _setup_bgColor;
                    app.set_tft_tools_button_config.targetBgColor        = _setup_bgColor;

                    app.createEditButton(setup_view);

                    app.set_tft_tools_button_config.button_x             = (_x + 60);
                    app.set_tft_tools_button_config.button_y             = _y;
                    app.set_tft_tools_button_config.btn_txt              = "Edit";
                    app.set_tft_tools_button_config.targetBgColor        = _setup_bgColor;

                    app.createEditButton(setup_view);

                    if(setup_router_setup_index == 0){

                        app.set_tft_tools_button_config.bgColor              = TFT_BLACK;

                    }

                    if(setup_router_setup_index == 1){

                    }
                }
            }


            //Serial.print("SSID_SETUP:       ");Serial.println(app.ROUTER_SSID);

        }

        if( _location_set_type == 1 ){ // Manualy
            
        }
    }

    if( _setup_step == 3 ){ // set location page

        step_y = 100;

        _intro_txt          = "Setting up your location";

        setup_view.setFreeFont(&DejaVu_Sans_Bold_11);

        _intro_w      = setup_view.textWidth(_intro_txt);
        _intro_x      = ((setup_view.width() - _intro_w) / 2);

        setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
        setup_view.drawString(_intro_txt, _intro_x, 70); // intro text

        if( !_setup_country_ok ){

            step_txt = "Please enter your country.";
            
            setup_view.setFreeFont(&DejaVu_Sans_Bold_11);

            step_w = setup_view.textWidth(step_txt);
            step_x = ((setup_view.width() - step_w) / 2);

          
            setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
            setup_view.drawString(step_txt, step_x, step_y); // intro text

            _intro_txt       = "";
            _setup_step      = 1;
            app.set_tft_device_setup_screen_config.setup_step = _setup_step;

            app.setting_location_page();
        }
        else
        {
            _setup_step = 3;
        }
    }
    
    if( _setup_step == 3 ){ // -------------

        if( !_setup_country_ok ){

            step_txt = "Please enter your country.";
            
            setup_view.setFreeFont(&DejaVu_Sans_Bold_11);

            step_w = setup_view.textWidth(step_txt);
            step_x = ((setup_view.width() - step_w) / 2);

        
            setup_view.setTextColor(TFT_WHITE, _setup_bgColor);
            setup_view.drawString(step_txt, step_x, step_y); // intro text

            //app.setting_location_page();
        }
        else
        {
            _setup_step = 4;
        }

    }

    if( _setup_step == -1 || _setup_step == 0){ // show footer only on intro page and home page

        setup_view.fillRoundRect(cadre_x, (cadre_y + cadre_h + 2), cadre_w, setup_view.height() - (cadre_y + cadre_h) - 3, 4, TFT_FOOTER_BG); // footer area
            
        setup_view.setFreeFont(&DejaVu_Sans_Bold_11); 
        String footer  = "EMAIL "; footer += SUPPORT_EMAIL; footer += "      "; footer += "MOBILE "; footer += SUPPORT_MOBILE;
        
        int footer_w  = app.tft.textWidth(footer);
        
        _x = (app.tft.width() - footer_w) / 2;
        _x = _x - (cadre_x*2);
        _y = (cadre_h + 25);

        setup_view.setTextColor(TFT_WHITE, TFT_FOOTER_BG);
        setup_view.drawString(footer, _x, _y, 2);
    }
    else
    {
        // delete footer espirit to free mem
    }

    setup_view.pushSprite(0, 0);
}

void setup_screen::setting_location_page(){

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
    int btn_margin         = 20;//
    int btn_line_w         = 0;

    if(!setup_view.created()){

        /* init setup_view espirite */
        setup_view.setSwapBytes(true);
        espiret_h = (app.tft.height() - app.set_top_bar_control.hieght);
        setup_view.createSprite(app.tft.width(), app.tft.height(), 2);
        setup_view.fillRect(0, 0, app.tft.width(), app.tft.height(), SETTINGS_PAGE_BG);
    }

    app.set_tft_tools_button_config.targetBgColor = SETTINGS_PAGE_BG;

    setup_view.fillRect(0, 0, setup_view.width(), setup_view.height(), SETTINGS_PAGE_BG);
    setup_view.setFreeFont(&Dialog_bold_22);
    setup_view.setTextColor(TFT_BLUE, SETTINGS_PAGE_BG);

    String setings_location_element[] = {"Back", "Country", "City", "Timezone", "Close", "Next"};
    int elements_length = STRING_ARRAY_SIZE(setings_location_element);

    save_w  = setup_view.textWidth(setings_location_element[4]) + 14;  // 14 is the padding R/L of button box
    close_w = setup_view.textWidth(setings_location_element[5]) + 14 ; // 14 is the padding R/L of button box

    btn_line_w       = save_w + btn_margin + close_w;

    int elems_w      = biggerLength(setings_location_element, elements_length);
    int page_title_x = ((app.tft.width() - setup_view.textWidth(pageTitle)) / 2);

    setup_view.drawString(pageTitle, page_title_x, 10); // Print page title

    app.set_tft_tools_pallet_config.targetBgColor     = _setup_bgColor;

    if(!app.set_tft_tools_pallet_config.edit){
       
        if ( setup_btn_up.shortPress() ){

            setup_button_move = true;

            app.set_tft_tools_pallet_config.edit = false;

            setup_elements_index ++;

            if(setup_elements_index == -1){

                setup_button_move = false;
            }

            if(setup_elements_index > 7/*elements_length*/){
                setup_elements_index = -1;
            } 

        }

        if ( setup_btn_enter.shortPress() ){

            if(setup_elements_index == 0){

                app.set_tft_tools_pallet_config.edit = false;
                setup_elements_index          = 0;
                setup_button_move             = false;
                setup_view.deleteSprite();
                app.PAGE_NAME           = "SETTINGS";

            }

            if(setup_elements_index == 1){
                setup_button_move                    = false;
                app.set_tft_tools_pallet_config.edit = true;
            }

            if(setup_elements_index == 2){
                setup_button_move                    = false;
                app.set_tft_tools_pallet_config.edit = true;
            }

            if(setup_elements_index == 3){
                setup_button_move                    = false;
                app.set_tft_tools_pallet_config.edit = true;
            }

            if(setup_elements_index == 4){

            }

            if(setup_elements_index == 5){

            }

        }

        if ( setup_btn_down.shortPress() ){
            
            setup_button_move = true;

            setup_edit_pallet.edit = false;
            //edit_pallet.drawRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, SETTINGS_PAGE_BG);
            //edit_pallet.fillRoundRect(0, 0, edit_pallet.width(), edit_pallet.height(), 4, SETTINGS_PAGE_BG);

            if(setup_elements_index == -1){
                setup_button_move = false;
            }

            if(setup_elements_index < 0){
                setup_button_move = true;
                setup_elements_index = 7/*elements_length*/;
            } 

            setup_elements_index --;
        }
        
    }

    for(int i = 0; i < elements_length; i++){

        if(i == 0){

            setup_view.setTextFont(2);
            setup_view.setTextColor(TFT_BLUE, SETTINGS_PAGE_BG);
            setup_view.drawRoundRect(4, 8, 39, 20, 4, TFT_WHITE);
            setup_view.fillRoundRect(6, 10, 35, 16, 4, SETTINGS_PAGE_BG);
            setup_view.drawString(setings_location_element[i], 10, 10);

            if(setup_button_move && setup_elements_index == i){
                setup_view.setTextFont(2);
                setup_view.fillRoundRect(6, 10, 35, 16, 4, TFT_WHITE);
                setup_view.setTextColor(TFT_BLUE, TFT_WHITE);
                setup_view.drawString(setings_location_element[i], 10, 10);
            }
        }

        setup_view.setFreeFont(&DejaVu_Sans_Bold_11);

        if(i == 1){ // Country

            text_w  = setup_view.textWidth(setings_location_element[i]);
            value_w = setup_view.textWidth(app.COUNTRY);
            line_w  = text_w + title_value_margin + value_w;

            _x = ((setup_view.width() - line_w) / 2) - 100;
            _y = 60;

            /* element title */
            setup_view.fillRect((_x - 10), _y, (text_w + 10), (btn_box_h - 4), SETTINGS_PAGE_BG);
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(setings_location_element[i], _x , _y);

            /* element value */
            setup_view.fillRoundRect((_x + elems_w + title_value_margin) - 10, _y, (value_w + 8), (btn_box_h - 4), 4, SETTINGS_PAGE_BG); // clear
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(app.COUNTRY, (_x + elems_w + title_value_margin) , _y);
            
            if(setup_elements_index == i){

                value_w = setup_view.textWidth(app.COUNTRY);

                setup_view.drawRoundRect((_x + elems_w + title_value_margin) - 4, (_y - 6), (value_w + 10), btn_box_h, 4, TFT_WHITE);    
                setup_view.fillRoundRect((_x + elems_w + title_value_margin) - 2, (_y - 4), (value_w + 6), (btn_box_h - 4), 4, edit_color);
                setup_view.setTextColor(TFT_WHITE, edit_color);
                setup_view.drawString(app.COUNTRY, (_x + elems_w + title_value_margin), _y);


                if(setup_button_move && !app.set_tft_tools_pallet_config.edit){
                    
                    app.set_tft_tools_button_config.button_x             = ((_x + elems_w + title_value_margin) + value_w + 15),
                    app.set_tft_tools_button_config.button_y             = _y - 6;
                    app.set_tft_tools_pallet_config.key                  = setings_location_element[i];

                    app.createEditButton(setup_view);
                }
                else
                {
                    if(!setup_button_move){

                    }

                    if(app.set_tft_tools_pallet_config.edit){

                        app.set_tft_tools_pallet_config.pallet_x          = ((_x + elems_w + title_value_margin) + value_w + 15),
                        app.set_tft_tools_pallet_config.pallet_y          = _y - 5,
                        app.set_tft_tools_pallet_config.targetBgColor     = _setup_bgColor;
                        app.set_tft_tools_pallet_config.txtFont           = &DejaVu_Sans_Bold_11,
                        app.set_tft_tools_pallet_config.use_target_bg     = false,
                        app.set_tft_tools_pallet_config.txtColor          = TFT_WHITE,
                        app.set_tft_tools_pallet_config.cadrColor         = TFT_WHITE;
                        app.set_tft_tools_pallet_config.input_cadreColor  = TFT_EDIT_PALLET_INPUT_CADRE,
                        app.set_tft_tools_pallet_config.btnsCadreColor    = TFT_EDIT_BTN_CADRE,
                        
                        app.set_tft_tools_pallet_config.bgColor           = TFT_EDIT_PALLET_BG,
                        app.set_tft_tools_pallet_config.fillColor         = TFT_EDIT_BTN_FILL,
                        app.set_tft_tools_pallet_config.current_index     = i;
                        app.set_tft_tools_pallet_config.key               = setings_location_element[i];
                        
                        app.createEditPallet(setup_view);
                    }
                }
                
                
            }
        }

        if(i == 2){ // City

            _x = ((setup_view.width() - line_w) / 2) - 100;
            _y = _y + 40;
            
            /* element title */
            setup_view.fillRect((_x - 10), _y, (text_w + 10), (btn_box_h - 4), SETTINGS_PAGE_BG); // clear
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(setings_location_element[i], _x , _y);

            /* element value */
            setup_view.fillRoundRect((_x + elems_w + title_value_margin) - 10, _y, (value_w + 10), (btn_box_h - 4), 4, SETTINGS_PAGE_BG); // clear
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(app.CITY, (_x + elems_w + title_value_margin) , _y);

            if(setup_elements_index == i){

                value_w = setup_view.textWidth(app.CITY);

                setup_view.drawRoundRect((_x + elems_w + title_value_margin) - 4, (_y - 6), (value_w + 10), btn_box_h, 4, TFT_WHITE);    
                setup_view.fillRoundRect((_x + elems_w + title_value_margin) - 2, (_y - 4), (value_w + 6), (btn_box_h - 4), 4, edit_color);
                setup_view.setTextColor(TFT_WHITE, edit_color);
                setup_view.drawString(app.CITY, (_x + elems_w + title_value_margin) , _y);

                if(setup_button_move && !app.set_tft_tools_pallet_config.edit){
                    app.set_tft_tools_button_config.button_x             = ((_x + elems_w + title_value_margin) + value_w + 15),
                    app.set_tft_tools_button_config.button_y             = _y - 6;
                    app.set_tft_tools_pallet_config.key               = setings_location_element[i];
                    app.createEditButton(setup_view);
                }
                else
                {
                    if(!setup_button_move){

                    }

                    if(app.set_tft_tools_pallet_config.edit){
                        app.set_tft_tools_pallet_config.pallet_x          = ((_x + elems_w + title_value_margin) + value_w + 15),
                        app.set_tft_tools_pallet_config.pallet_y          = _y - 5,
                        app.set_tft_tools_pallet_config.targetBgColor     = _setup_bgColor;
                        app.set_tft_tools_pallet_config.txtFont           = &DejaVu_Sans_Bold_11,
                        app.set_tft_tools_pallet_config.use_target_bg     = false,
                        app.set_tft_tools_pallet_config.txtColor          = TFT_WHITE,
                        app.set_tft_tools_pallet_config.cadrColor         = TFT_WHITE;
                        app.set_tft_tools_pallet_config.input_cadreColor  = TFT_EDIT_PALLET_INPUT_CADRE,
                        app.set_tft_tools_pallet_config.btnsCadreColor    = TFT_EDIT_BTN_CADRE,
                        
                        app.set_tft_tools_pallet_config.bgColor           = TFT_EDIT_PALLET_BG,
                        app.set_tft_tools_pallet_config.fillColor         = TFT_EDIT_BTN_FILL;
                        app.set_tft_tools_pallet_config.current_index     = i;
                        app.set_tft_tools_pallet_config.key               = setings_location_element[i];
                        
                        app.createEditPallet(setup_view);
                    }
                }

                
            }
        }

        if(i == 3){ // Timezone

            _x = ((setup_view.width() - line_w) / 2) - 100;
            _y = _y + 40;
            
            /* element title */
            setup_view.fillRect((_x - 10), _y, (text_w + 10), (btn_box_h - 4), SETTINGS_PAGE_BG); // clear
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(setings_location_element[i], _x , _y);

            /* element value */
            setup_view.fillRoundRect((_x + elems_w + title_value_margin) - 10, _y, (value_w + 10), (btn_box_h - 4), 4, SETTINGS_PAGE_BG); // clear
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(app.TIMEZONE, (_x + elems_w + title_value_margin) , _y);

            if(setup_elements_index == i){
                
                value_w = setup_view.textWidth(app.TIMEZONE);
                setup_view.drawRoundRect((_x + elems_w + title_value_margin) - 4, (_y - 6), (value_w + 10), btn_box_h, 4, TFT_WHITE);    
                setup_view.fillRoundRect((_x + elems_w + title_value_margin) - 2, (_y - 4), (value_w + 6), (btn_box_h - 4), 4, edit_color);
                setup_view.setTextColor(TFT_WHITE, edit_color);
                setup_view.drawString(app.TIMEZONE, (_x + elems_w + title_value_margin) , _y);

                if(setup_button_move && !app.set_tft_tools_pallet_config.edit){
                    app.set_tft_tools_button_config.button_x             = ((_x + elems_w + title_value_margin) + value_w + 15),
                    app.set_tft_tools_button_config.button_y             = _y - 6;
                    app.set_tft_tools_pallet_config.key                  = setings_location_element[i];

                    app.createEditButton(setup_view);
                }
                else
                {
                    if(!setup_button_move){

                    }

                    if(app.set_tft_tools_pallet_config.edit){
                        app.set_tft_tools_pallet_config.pallet_x          = ((_x + elems_w + title_value_margin) + value_w + 15),
                        app.set_tft_tools_pallet_config.pallet_y          = _y - 5,
                        app.set_tft_tools_pallet_config.targetBgColor     = _setup_bgColor;
                        app.set_tft_tools_pallet_config.txtFont           = &DejaVu_Sans_Bold_11,
                        app.set_tft_tools_pallet_config.use_target_bg     = false,
                        app.set_tft_tools_pallet_config.txtColor          = TFT_WHITE,
                        app.set_tft_tools_pallet_config.cadrColor         = TFT_WHITE;
                        app.set_tft_tools_pallet_config.input_cadreColor  = TFT_EDIT_PALLET_INPUT_CADRE,
                        app.set_tft_tools_pallet_config.btnsCadreColor    = TFT_EDIT_BTN_CADRE,
                        
                        app.set_tft_tools_pallet_config.bgColor           = TFT_EDIT_PALLET_BG,
                        app.set_tft_tools_pallet_config.fillColor         = TFT_EDIT_BTN_FILL;
                        app.set_tft_tools_pallet_config.current_index     = i;
                        app.set_tft_tools_pallet_config.key               = setings_location_element[i];
                        
                        app.createEditPallet(setup_view);

                    }
                }
            }
            
        }

        if(i == 4){

            text_w = setup_view.textWidth(setings_location_element[i]);

            _x = 20;
            _y = (setup_view.height() - 30);

            setup_view.drawRoundRect((_x - 4), (_y - 6), (text_w + 10), btn_box_h, 4, TFT_WHITE);
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(setings_location_element[i], _x , _y);
        }

        if(i == 5){

            text_w = setup_view.textWidth(setings_location_element[i]);

            _x = (setup_view.width() - 50);
            _y = (setup_view.height() - 30);
            
            setup_view.drawRoundRect((_x - 4), (_y - 6), (text_w + 10), btn_box_h, 4, TFT_WHITE);
            setup_view.setTextColor(TFT_WHITE, SETTINGS_PAGE_BG);
            setup_view.drawString(setings_location_element[i], _x , _y);
        }
    }

    setup_view.pushSprite(0, 0);
}