#include "tft_tools.h"
#include "Application.h"
//#include "tft/tft_helper.h"

InputButton edit_btn_up(BUTTON_UP, true, ACTIVE_HIGH);
InputButton edit_btn_enter(BUTTON_ENTER, true, ACTIVE_HIGH);
InputButton edit_btn_down(BUTTON_DOWN, true, ACTIVE_HIGH);
InputButton edit_btn_edit(BUTTON_EDIT, true, ACTIVE_HIGH);

String EDIT_PALLET_SMALL_ARRAY[]   = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
String EDIT_PALLET_CAPITAL_ARRAY[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
String EDIT_PALLET_NUM_ARRAY[]     = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "-", "_", "*", "/", "@", "?", ":", "!"};

String SSID_ARRY[]                               = {};
int wifi_scan_first_line_y                       = 20;
int wifi_scan_network_count                      = 0;
int wifi_scan_network_index                      = 0;
int wifi_scan_scanning_wait                      = 7;
bool wifi_scan_btn_move                          = false;
bool scan_results_started                        = false;
bool scan_results_ready                          = false;



const int buttons_espirets_index                 = 20;
bool button_created                              = false;
bool pallet_created                              = false;
bool wifi_scan_created                           = false;

TFT_eSprite *edit_pallet_view                    = nullptr; 
TFT_eSprite *button_view[buttons_espirets_index] = {0};
TFT_eSprite *wifi_scan_view                      = nullptr;


tft_tools::tft_tools(/* args */){

    _button_x                  = 0;
    _button_y                  = 0;
    _button_w                  = 0; 
    _button_h                  = 20;
    _btn_txt_w                 = 0;
    _btn_txt_h                 = 0;
    _button_fill_x             = 3;
    _button_fill_y             = 3;
    _button_fill_w             = 0;
    _button_fill_h             = 0;
    _btnsCadreColor            = TFT_WHITE;
    _show_edit_palet           = false;
    _remined_espirets_index    = buttons_espirets_index;
    _current_espirets_index    = 0;
    _pallet_x                  = 0;
    _pallet_y                  = 0;
    _pallet_w                  = 193; 
    _pallet_h                  = 100;
    _pallet_input_x            = 4;
    _pallet_input_y            = 4;
    _pallet_input_w            = 0;
    _pallet_input_h            = 30;
    _edit_pallet_input_bgColor = TFT_EDIT_PALLET_INPUT_BG;
    _input_cadreColor          = TFT_WHITE;
    
    _pallet_edit_move          = false;
    _still_edit                = false;
    _use_caps                  = false;
    _use_nums                  = false;
    _edit_index                = 0;
    _edit_chars_index          = -1;
    _elements_index            = -1;
    _edit_error                = false;
    _edit_error_msg            = "";
    _last_char                 = "";
    EDIT_PALLET_LIVE_TEXT      = "";
    _last_char_length          = 0;
    _pallet_live_text_length   = 0;

    _gif_view_created          = false;
    _gif_filename              = "";

    _wifi_init                 = false;
    _use_wifi_scan_target_bg   = false;
    _start_wifi_scan           = false;
    _wifi_scan_done            = false;
}
tft_tools::~tft_tools(){}


/******************************************************************************************************************
*                                                BUTTON                                                           *
******************************************************************************************************************/

void tft_tools::createEditButton(TFT_eSprite &target){

    _button_x      = app.set_tft_tools_button_config.button_x;
    _button_y      = app.set_tft_tools_button_config.button_y;
    _button_fill_x = 3;
    _button_fill_y = 3;
    int32_t center_x;
    int32_t center_y;

    if(_remined_espirets_index > 0){

        if(!button_created){
            
            button_view[_current_espirets_index] = new TFT_eSprite(&app.tft); // Sprite for buttons

            button_view[_current_espirets_index]->setFreeFont(app.set_tft_tools_button_config.txtFont);

            _btn_txt_w = button_view[_current_espirets_index]->textWidth(app.set_tft_tools_button_config.btn_txt);
            _btn_txt_h = button_view[_current_espirets_index]->fontHeight();

            app.set_tft_tools_button_config.btn_txt_w = _btn_txt_w;
            app.set_tft_tools_button_config.btn_txt_h = _btn_txt_h;

            _button_w = (_btn_txt_w + 11);
            _button_h = (_btn_txt_h + 9);

            app.set_tft_tools_button_config.button_w  = _button_w;
            app.set_tft_tools_button_config.button_h  = _button_h;

            button_view[_current_espirets_index]->setSwapBytes(true);
            button_view[_current_espirets_index]->createSprite(_button_w, _button_h, 2);

            button_created = true;
            _remined_espirets_index = (_remined_espirets_index - 1);
        }

        button_view[_current_espirets_index]->fillSprite(app.set_tft_tools_button_config.targetBgColor);

        //button_view[_current_espirets_index]->fillRect(0, 0, _btn_txt_w, _btn_txt_h, app.set_tft_tools_button_config.targetBgColor);
        

        center_x = (_button_w - _btn_txt_w) / 2;
        center_y = ((_button_h - _btn_txt_h) / 2) + 3;


        button_view[0]->setFreeFont(app.set_tft_tools_button_config.txtFont);

        _btn_txt_w                         = button_view[_current_espirets_index]->textWidth(app.set_tft_tools_button_config.btn_txt);
        _btn_txt_h                         = button_view[_current_espirets_index]->fontHeight();

        app.set_tft_tools_button_config.btn_txt_w = _btn_txt_w;
        app.set_tft_tools_button_config.btn_txt_h = _btn_txt_h;

        _button_w                          = (_btn_txt_w + 11);
        _button_h                          = (_btn_txt_h + 9);

        app.set_tft_tools_button_config.button_w  = _button_w;
        app.set_tft_tools_button_config.button_h  = _button_h;

        if(app.set_tft_tools_button_config.static_button){
            
            button_view[_current_espirets_index]->drawRoundRect(0, 0, _button_w, _button_h, 4, app.set_tft_tools_button_config.cadrColor); // draw button cadre
            button_view[_current_espirets_index]->fillRoundRect(3, 3, (_button_w - 6), (_button_h - 6), 4, app.set_tft_tools_button_config.fillColor);
            button_view[_current_espirets_index]->setTextColor(app.set_tft_tools_button_config.txtEditColor, app.set_tft_tools_button_config.fillColor);
            button_view[_current_espirets_index]->drawString(app.set_tft_tools_button_config.btn_txt, center_x, center_y);
        }
        else
        {
            button_view[_current_espirets_index]->drawRoundRect(0, 0, _button_w, _button_h, 4, app.set_tft_tools_button_config.targetBgColor); // draw button cadre with targetBgColor
            button_view[_current_espirets_index]->drawRoundRect(0, 0, _button_w, _button_h, 4, app.set_tft_tools_button_config.cadrColor); // draw button cadre
            button_view[_current_espirets_index]->fillRoundRect(3, 3, (_button_w - 6), (_button_h - 6), 4, app.set_tft_tools_button_config.bgColor);
            button_view[_current_espirets_index]->setTextColor(app.set_tft_tools_button_config.txtColor, app.set_tft_tools_button_config.bgColor);
            button_view[_current_espirets_index]->drawString(app.set_tft_tools_button_config.btn_txt, center_x, center_y);
        }

        //String tx_h = "Text H: "; tx_h += String(_btn_txt_h);

        //target.drawString(tx_h, 300, 220);

        button_view[_current_espirets_index]->pushToSprite(&target, _button_x, _button_y);
    }  
    else
    {
        target.drawString("Can not create more buttons", 50, 200);
    }
}

/******************************************************************************************************************
*                                                PALLET                                                           *
******************************************************************************************************************/ 

void tft_tools::setButtonTxtColor(uint16_t color){app.set_tft_tools_button_config.txtColor = color;};
void tft_tools::setButtonTxtEditColor(uint16_t color){app.set_tft_tools_button_config.txtEditColor = color;};
void tft_tools::setButtonCadreColor(uint16_t color){app.set_tft_tools_button_config.cadrColor = color;};
void tft_tools::setButtonFillColor(uint16_t color){app.set_tft_tools_button_config.fillColor = color;};
void tft_tools::setButtonBgColor(uint16_t color){app.set_tft_tools_button_config.bgColor = color;};
void tft_tools::setButtonTargetBgColor(uint16_t color){app.set_tft_tools_button_config.targetBgColor = color;};
void tft_tools::setButtonTxtFont(const GFXfont *font){app.set_tft_tools_button_config.txtFont = font;};

void tft_tools::createEditPallet(TFT_eSprite &target){

    if(!pallet_created){

        edit_pallet_view = new TFT_eSprite(&app.tft); // Sprite for edit pallet

        edit_pallet_view->setSwapBytes(true);
        _pallet_w = app.set_tft_tools_pallet_config.pallet_w;
        _pallet_h = app.set_tft_tools_pallet_config.pallet_h;
        edit_pallet_view->createSprite(_pallet_w, _pallet_h, 2);

        pallet_created = true;
    }

    _edit_pallet_input_bgColor = app.set_tft_tools_pallet_config.input_bgColor;
    _input_cadreColor          = app.set_tft_tools_pallet_config.input_cadreColor;
    _pallet_w                  = app.set_tft_tools_pallet_config.pallet_w;
    _pallet_h                  = app.set_tft_tools_pallet_config.pallet_h;

    _pallet_input_w = (_pallet_w - 8);

    edit_pallet_view->fillRect(0, 0, app.set_tft_tools_pallet_config.pallet_w, app.set_tft_tools_pallet_config.pallet_h, app.set_tft_tools_pallet_config.targetBgColor);
    edit_pallet_view->drawRoundRect(0, 0, app.set_tft_tools_pallet_config.pallet_w, app.set_tft_tools_pallet_config.pallet_h, 4, app.set_tft_tools_pallet_config.cadrColor);

    // If true the edit pallet will appear with the target background
    if(app.set_tft_tools_pallet_config.use_target_bg){
        edit_pallet_view->fillRoundRect(3, 3, (app.set_tft_tools_pallet_config.pallet_w - 4), (app.set_tft_tools_pallet_config.pallet_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
    }
    else
    {
        edit_pallet_view->fillRoundRect(3, 3, (app.set_tft_tools_pallet_config.pallet_w - 4), (app.set_tft_tools_pallet_config.pallet_h - 4), 4, app.set_tft_tools_pallet_config.bgColor);
    }

    _pallet_x = app.set_tft_tools_pallet_config.pallet_x;
    _pallet_y = app.set_tft_tools_pallet_config.pallet_y;

    String edit_elements[9] = {"input", "<", "OK", "Del", ">", "Cap", "Save", "Num", "X"};

    int edit_elements_length = STRING_ARRAY_SIZE(edit_elements);
    int chars_length         = STRING_ARRAY_SIZE(EDIT_PALLET_SMALL_ARRAY);

    int l_btn_w              = edit_pallet_view->textWidth(edit_elements[1]);
    int ok_btn_w             = edit_pallet_view->textWidth(edit_elements[2]);
    int del_btn_w            = edit_pallet_view->textWidth(edit_elements[3]);
    int r_btn_w              = edit_pallet_view->textWidth(edit_elements[4]);
    int ca_btn_w             = edit_pallet_view->textWidth(edit_elements[5]);
    int sv_btn_w             = edit_pallet_view->textWidth(edit_elements[6]);
    int nu_btn_w             = edit_pallet_view->textWidth(edit_elements[7]);
    int x_btn_w              = edit_pallet_view->textWidth(edit_elements[8]);

    int _x, _y, _w, _h;

    _btnsCadreColor = app.set_tft_tools_pallet_config.btnsCadreColor;

    if(_edit_error){

        _x = _pallet_input_x;
        _y = (_pallet_input_h + 54);
        _w = (edit_pallet_view->width() - (x_btn_w + 20));
        _h = 14;

        if(app.set_tft_tools_pallet_config.use_target_bg){
            edit_pallet_view->fillRect(_x, _y, _w, _h, app.set_tft_tools_pallet_config.targetBgColor); //clean errors area
            edit_pallet_view->setTextColor(TFT_RED, app.set_tft_tools_pallet_config.targetBgColor);
        }
        else
        {
            edit_pallet_view->fillRect(_x, _y, _w, _h, app.set_tft_tools_pallet_config.bgColor); //clean errors area
            edit_pallet_view->setTextColor(TFT_RED, app.set_tft_tools_pallet_config.bgColor);
        }

        edit_pallet_view->drawString(_edit_error_msg, (_x + 1), (_y + 1), 1); // error string
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /*                                              MOVE BUTTONS                                           */
    /*-----------------------------------------------------------------------------------------------------*/

    if ( edit_btn_up.shortPress() ){
        
        _pallet_edit_move = true;

        _edit_index ++;

        if(_edit_index >= edit_elements_length/*elements_length*/){
            _edit_index = 0;
        } 

    }

    if ( edit_btn_enter.longPress() ){

        if(_edit_index == 3){ // Del

            //_show_edit_palet        = false;

            _last_char              = "";
            EDIT_PALLET_LIVE_TEXT   = "";
            _edit_chars_index       = -1;
            _elements_index         = 4;
            _still_edit             = false;
            _pallet_edit_move       = false;
            _use_caps               = false;
        }
    }
    
    if ( edit_btn_enter.shortPress() ){

        if(_edit_index == 0){ // input

        }

        if(_edit_index == 1){ // input

            _still_edit = true;

            if(app.set_tft_tools_pallet_config.chars_type == 0){

                if(_use_caps){

                    _last_char = EDIT_PALLET_CAPITAL_ARRAY[_edit_chars_index];
                    _last_char_length       = _last_char.length(); // get new length after deleting last char
                }
                else
                {
                    if(_use_nums){
                        _last_char = EDIT_PALLET_NUM_ARRAY[_edit_chars_index];
                        _last_char_length       = _last_char.length(); // get new length after deleting last char
                    }
                    else
                    {
                        _last_char = EDIT_PALLET_SMALL_ARRAY[_edit_chars_index];
                        _last_char_length       = _last_char.length(); // get new length after deleting last char
                    }
                }
            }

            if(app.set_tft_tools_pallet_config.chars_type == 1){
                _last_char = EDIT_PALLET_NUM_ARRAY[_edit_chars_index];
            }

            if(_edit_chars_index <= 0){
                _edit_chars_index = chars_length;
            }

            _edit_chars_index--;

        }

        if(_edit_index == 2){ // OK

            EDIT_PALLET_LIVE_TEXT   += _last_char;
            _edit_chars_index        = -1;
            _last_char               = "";
            _still_edit              = false;

        }

        if(_edit_index == 3){ // Del
            pallet_backspace();
        }

        if(_edit_index == 4){ //>

            _edit_chars_index++;

            _still_edit = true;

            if(app.set_tft_tools_pallet_config.chars_type == 0){

                if(_use_caps){

                    _last_char = EDIT_PALLET_CAPITAL_ARRAY[_edit_chars_index];
                    _last_char_length       = _last_char.length(); // get new length after deleting last char
                }
                else
                {
                    if(_use_nums){
                        _last_char = EDIT_PALLET_NUM_ARRAY[_edit_chars_index];
                        _last_char_length       = _last_char.length(); // get new length after deleting last char
                    }
                    else
                    {
                        _last_char = EDIT_PALLET_SMALL_ARRAY[_edit_chars_index];
                        _last_char_length       = _last_char.length(); // get new length after deleting last char
                    }
                }
            }

            if(app.set_tft_tools_pallet_config.chars_type == 1){
                _last_char = EDIT_PALLET_NUM_ARRAY[_edit_chars_index];
            }
            
            if(_edit_chars_index >= chars_length){
                _edit_chars_index = -1;
            }
        }

        if(_edit_index == 5){ // Caps

            if(_use_caps){
                _use_caps = false;
            }
            else
            {
                _use_caps = true;
                _use_nums = false;
            }
        }

        if(_edit_index == 6){ // Save

            String _key = app.set_tft_tools_pallet_config.key;

            _key.toUpperCase();

            app.setString(_key.c_str(), EDIT_PALLET_LIVE_TEXT.c_str());

            if(app.set_tft_tools_pallet_config.current_index == 1){
                app.COUNTRY = app.getString(_key.c_str());
            }

            if(app.set_tft_tools_pallet_config.current_index == 2){
                app.CITY = app.getString(_key.c_str());
            }

            if(app.set_tft_tools_pallet_config.current_index == 3){
                app.TIMEZONE = app.getString(_key.c_str());
            }
            

            _still_edit                          = false;
            _pallet_edit_move                    = false;
            _use_caps                            = false;
            _edit_chars_index                    = -1;
            _elements_index                      = -1;
            _show_edit_palet                     = false;
            app.set_tft_tools_pallet_config.edit = false;
            _key                                 = "";
            edit_pallet_view->deleteSprite();
            pallet_created                       = false;
            target.fillRect(0, 0, target.width(), target.height(), SETTINGS_PAGE_BG);

        }

        if(_edit_index == 7){ // nums

            if(_use_nums){
                _use_nums = false;
            }
            else
            {
                _use_nums = true;
                _use_caps = false;
            }
        }

        if(_edit_index == 8){ // close (X)

            
            _still_edit                          = false;
            _pallet_edit_move                    = false;
            _use_caps                            = false;
            _edit_chars_index                    = -1;
            _elements_index                      = -1;
            _show_edit_palet                     = false;
            app.set_tft_tools_pallet_config.edit = false;
            edit_pallet_view->deleteSprite();

        }

    }

    if ( edit_btn_down.shortPress() ){

        _pallet_edit_move = true;
        
        if(_edit_index <= 0){
            _edit_index = edit_elements_length/*elements_length*/;
        } 

        _edit_index --;

    }

    /*-----------------------------------------------------------------------------------------------------*/
    /*                                        END BUTTONS                                                  */
    /*-----------------------------------------------------------------------------------------------------*/

    EDIT_PALLET_LIVE_TEXT[0] = toupper(EDIT_PALLET_LIVE_TEXT[0]); // Make first letter capital

    /*-----------------------------------------------------------------------------------------------------*/
    /*                                 PLOT EDIT ELEMENTS IN SPRIT                                         */
    /*-----------------------------------------------------------------------------------------------------*/

    for(int i = 0; i < edit_elements_length; i++){

        if( i == 0 ){ // Plot the input element

            edit_pallet_view->drawRoundRect(_pallet_input_x, _pallet_input_y, _pallet_input_w, _pallet_input_h, 4, _input_cadreColor); // input
            edit_pallet_view->fillRoundRect((_pallet_input_x + 2), (_pallet_input_y + 2), (_pallet_input_w - 4), (_pallet_input_h - 4), 4, _edit_pallet_input_bgColor);
            edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, _edit_pallet_input_bgColor);
            edit_pallet_view->drawString(EDIT_PALLET_LIVE_TEXT, (_pallet_input_x + 5), (_pallet_input_y + 5));

            int len = edit_pallet_view->textWidth(EDIT_PALLET_LIVE_TEXT);

            if(_pallet_edit_move && _edit_index == i && !_still_edit){

                edit_pallet_view->fillRoundRect((_pallet_input_x + 2), (_pallet_input_y + 2), (_pallet_input_w - 4), (_pallet_input_h - 4), 4, _edit_pallet_input_bgColor);
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, _edit_pallet_input_bgColor);
                edit_pallet_view->drawString(EDIT_PALLET_LIVE_TEXT, (_pallet_input_x + 5), (_pallet_input_y + 8));
            }
            else
            {
                edit_pallet_view->fillRoundRect((_pallet_input_x + 2), (_pallet_input_y + 2), (_pallet_input_w - 4), (_pallet_input_h - 4), 4, _edit_pallet_input_bgColor);
                edit_pallet_view->setTextColor(TFT_WHITE, _edit_pallet_input_bgColor);

                edit_pallet_view->drawString(EDIT_PALLET_LIVE_TEXT, (_pallet_input_x + 5), (_pallet_input_y + 8));
                edit_pallet_view->drawString(_last_char, (_pallet_input_x + len + 7), (_pallet_input_y + 8));
            }
        }

        if( i == 1 ){ // Plot < sign

            _x = (_pallet_input_x + 30);
            _y = (_pallet_input_h + 10);
            _w = (l_btn_w + 10);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); // <  cadre

            // If true the edit pallet will appear with the target background
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }
            
            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // <

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }

        }

        if( i == 2 ){

            _x = (_x + _w + 10);
            _y = (_pallet_input_h + 10);
            _w = (ok_btn_w + 8);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); // ok cadre
            
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // OK

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); //OK 
            }
        }

        if( i == 3 ){

            _x = (_x + _w + 10);
            _y = (_pallet_input_h + 10);
            _w = (del_btn_w + 8);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); // Del cadre
            
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // Del

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // Del
            }
        }
    
        if( i == 4 ){

            _x = (_x + _w + 10);
            _y = (_pallet_input_h + 10);
            _w = (r_btn_w + 10);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); // >  cadre
            
            // If true the edit pallet will appear with the target background
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // >

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // <
            }
        }

        if( i == 5 ){
            _x = (_pallet_input_x + 22);
            _y = (_pallet_input_h + 33);
            _w = (ca_btn_w + 10);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor);  //cap cadre
            
            // If true the edit pallet will appear with the target background
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // Cap

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // Cap
            }
        }

        if( i == 6 ){
            _x = (_x + _w + 10);
            _y = (_pallet_input_h + 33);
            _w = (sv_btn_w + 10);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); //save cadre
            
            // If true the edit pallet will appear with the target background
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // Save

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // Save
            }
        }

        if( i == 7 ){
            _x = (_x + _w + 10);
            _y = (_pallet_input_h + 33);
            _w = (nu_btn_w + 10);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); //num cadre
            
            // If true the edit pallet will appear with the target background
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // num

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 4), (_y + 4)); // num
            }
        }

        if( i == 8 ){
            _x = (_x + _w + 7);
            _y = (_pallet_input_h + 48);
            _w = (x_btn_w + 10);
            _h = (_pallet_input_h - 12);

            edit_pallet_view->setFreeFont(app.set_tft_tools_pallet_config.txtFont);

            edit_pallet_view->drawRoundRect(_x, _y, _w, _h, 4, app.set_tft_tools_pallet_config.btnsCadreColor); //close (X) cadre
            
            // If true the edit pallet will appear with the target background
            if(app.set_tft_tools_pallet_config.use_target_bg){
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.targetBgColor);
            }
            else
            {
                edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.bgColor);
            }

            edit_pallet_view->drawString(edit_elements[i], (_x + 5), (_y + 4)); // close (X)

            if(_pallet_edit_move && _edit_index == i){
                
                if(app.set_tft_tools_pallet_config.use_target_bg){
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.targetBgColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtEditColor, app.set_tft_tools_pallet_config.targetBgColor);
                }
                else
                {
                    edit_pallet_view->fillRoundRect((_x + 2), (_y + 2), (_w - 4), (_h - 4), 4, app.set_tft_tools_pallet_config.fillColor);
                    edit_pallet_view->setTextColor(app.set_tft_tools_pallet_config.txtColor, app.set_tft_tools_pallet_config.fillColor);
                }

                edit_pallet_view->drawString(edit_elements[i], (_x + 5), (_y + 4)); // close (X)
            }
        }
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /*                               END PLOT EDIT ELEMENTS IN SPRIT                                       */
    /*-----------------------------------------------------------------------------------------------------*/

    edit_pallet_view->pushToSprite(&target, app.set_tft_tools_pallet_config.pallet_x, app.set_tft_tools_pallet_config.pallet_y);

}

String tft_tools::pallet_backspace(){

    if(_last_char.length() > 0){
        _last_char      = "";
    }
    else
    {

        EDIT_PALLET_LIVE_TEXT = EDIT_PALLET_LIVE_TEXT.substring(0, _pallet_live_text_length-1);

        _pallet_live_text_length = EDIT_PALLET_LIVE_TEXT.length();
    }
    
    return EDIT_PALLET_LIVE_TEXT;
}

void tft_tools::setPalletTxtColor(uint16_t color){app.set_tft_tools_pallet_config.txtColor = color;};
void tft_tools::setPalletTxtEditColor(uint16_t color){app.set_tft_tools_pallet_config.txtEditColor = color;};
void tft_tools::setPalletCadreColor(uint16_t color){app.set_tft_tools_pallet_config.cadrColor = color;};
void tft_tools::setPalletFillColor(uint16_t color){app.set_tft_tools_pallet_config.fillColor = color;};
void tft_tools::setPalletBgColor(uint16_t color){app.set_tft_tools_pallet_config.bgColor = color;};
void tft_tools::setPalletTargetBgColor(uint16_t color){app.set_tft_tools_pallet_config.targetBgColor = color;};
void tft_tools::setPalletTxtFont(const GFXfont *font){app.set_tft_tools_pallet_config.txtFont = font;};


int tft_tools::str_array_bigger_length(TFT_eSprite &target, String array[], int length){
    

    int max_v = INT_MIN;
	int max_i = 0;
    
    for ( int i = 0; i < length; i++ ){

        if ( target.textWidth(array[i]) > max_v ){

            max_v = target.textWidth(array[i]);
            max_i = i;
        }
	}
 
	//printf( "The max value (%d) is at index %d.\n", max_v, max_i);
 
	return max_v;


}

int tft_tools::str_array_bigger_length(TFT_eSprite *target, String array[], int length){
    

    int max_v = INT_MIN;
	int max_i = 0;
    
    for ( int i = 0; i < length; i++ ){

        if ( target->textWidth(array[i]) > max_v ){

            max_v = target->textWidth(array[i]);
            max_i = i;
        }
	}
 
	//printf( "The max value (%d) is at index %d.\n", max_v, max_i);
 
	return max_v;


}

/********************** WIFI *********************************/
void tft_tools::set_start_wifi_scan(bool scan){
    _start_wifi_scan = scan;
    _wifi_scan_done  = false;
}

bool tft_tools::scanResultsReady(){
    return scan_results_ready;
}

bool tft_tools::get_wifi_scan_done(){
    return _wifi_scan_done;
}

void tft_tools::setScanResultsStarted(bool started){
    scan_results_started = started;
}

String tft_tools::paswwordToStarts(String password){
    
    int length = password.length();
    String stars_password = "";

    for(int i = 0; i < length; i++){
        stars_password += "*";
    }

    return stars_password;
}

void tft_tools::tft_wifi_scan(TFT_eSprite &target){

    if(!wifi_scan_created){
            
        wifi_scan_view = new TFT_eSprite(&app.tft); // Sprite for buttons

        wifi_scan_view->setFreeFont(app.set_tft_tools_wifi_scan_config.txtFont);

        wifi_scan_view->setSwapBytes(true);
        wifi_scan_view->createSprite(app.set_tft_tools_wifi_scan_config.w, app.set_tft_tools_wifi_scan_config.h, 2);
        wifi_scan_view->fillSprite(app.set_tft_tools_wifi_scan_config.targetBgColor);

        wifi_scan_created = true;
    }

    int n = 0;

    int x            = 0;
    int y            = 0;
    int w            = 0;
    int no_x         = 0;
    int no_w         = 0;
    int ssid_w       = 0;
    int fill_x       = 0;
    int fill_y       = 0;
    
    String info_txt = "";
    String scan_buttn = "";

    Serial.print("_start_wifi_scan: ");Serial.println(_start_wifi_scan);

    if(!_start_wifi_scan){ // from where we use the api when set to true the scan will start

        info_txt   = "Press OK button to start scan";
        scan_buttn = "Scan networks";

        wifi_scan_view->setFreeFont(&DejaVu_Sans_Bold_16);
            
        x  = (app.tft.width() - wifi_scan_view->textWidth(info_txt)) / 2;
        y  = ((wifi_scan_view->height()/2) - 50);

        wifi_scan_view->setTextColor(TFT_RED, app.set_tft_tools_wifi_scan_config.targetBgColor);
        wifi_scan_view->drawString(info_txt, (x - 1), y); // Press OK button to start scan

        x  = (app.tft.width() - wifi_scan_view->textWidth(scan_buttn)) / 2;
        y  = (y + 30);
        w  = (wifi_scan_view->textWidth(scan_buttn) + 10);

        wifi_scan_view->drawRoundRect(x, y, w, 30, 4, TFT_ORANGE); // Scan button cadre

        x  = (x + 2);
        y  = (y + 2);
        w  = (w - 4);

        wifi_scan_view->fillRoundRect(x, y, w, 26, 4, TFT_WHITE); // Scan button fill

        x  = ((app.tft.width() - wifi_scan_view->textWidth(scan_buttn)) / 2) + 5;
        y  = (y + 5);

        wifi_scan_view->setTextColor(TFT_ORANGE, TFT_WHITE);
        wifi_scan_view->drawString(scan_buttn, x, y); //  // Scan button text

       // wifi_scan_view->pushToSprite(&target, app.set_tft_tools_wifi_scan_config.x, app.set_tft_tools_wifi_scan_config.y);
    }
    
    if(_start_wifi_scan){ // from where we use the api when set to true the scan will start

        Serial.print("wifi_scan_scanning_wait: ");Serial.println(wifi_scan_scanning_wait);

        if(wifi_scan_scanning_wait < 6){

            info_txt = "Scanning ...";

            wifi_scan_view->fillSprite(app.set_tft_tools_wifi_scan_config.targetBgColor);

            wifi_scan_view->setFreeFont(&Dialog_bold_22);

            x  = (app.tft.width() - wifi_scan_view->textWidth(info_txt)) / 2;
            y  = ((wifi_scan_view->height()/2) - 50);

            wifi_scan_view->setTextColor(TFT_WHITE, app.set_tft_tools_wifi_scan_config.targetBgColor);
            wifi_scan_view->drawString(info_txt, (x - 1), y); // Scanning ...
        }

        if(wifi_scan_scanning_wait == 0){
            
            
            info_txt = "Scan done";

            wifi_scan_view->fillSprite(app.set_tft_tools_wifi_scan_config.targetBgColor);

            wifi_scan_view->setFreeFont(&Dialog_bold_22);

            x  = (app.tft.width() - wifi_scan_view->textWidth(info_txt)) / 2;
            y  = ((wifi_scan_view->height()/2) - 50);

            wifi_scan_view->setTextColor(TFT_WHITE, app.set_tft_tools_wifi_scan_config.targetBgColor);
            wifi_scan_view->drawString(info_txt, (x - 1), y); // Scanning ...

            if(!_wifi_init){ // we check if wifi was not inited before

                WiFi.mode(WIFI_STA);
                WiFi.disconnect();
                delay(100);

                _wifi_init = true; // after int wifi we set this to true;
            }
            
            if(!_wifi_scan_done){ // if not true that is mean we did a scan before

                n = WiFi.scanNetworks();
            }

            scan_results_started = true;
            _start_wifi_scan     = false;
        }
        

        wifi_scan_scanning_wait--;

        wifi_scan_view->pushToSprite(&target, app.set_tft_tools_wifi_scan_config.x, app.set_tft_tools_wifi_scan_config.y);

        Serial.println("Scan done");
    }

    if(scan_results_started){ // start getting resaults

        if (n == 0) { // if no networks found

            //wifi_scan_view->fillSprite(app.set_tft_tools_wifi_scan_config.targetBgColor);


        }
        else
        {
            // if we found networks
            if(!_wifi_scan_done){ // if there is no scan done so will start (n = WiFi.scanNetworks()) then get the networks

                wifi_scan_network_count = n; // get the networks count out of this loop

                for (int i = 0; i < n; i++) {

                    
                    SSID_ARRY[i] = WiFi.SSID(i);
                }

                // Delete the scan result to free memory for code below.
                WiFi.scanDelete();
                _wifi_scan_done = true;
            }
        }
    }
    
    if(_wifi_scan_done){

        wifi_scan_view->fillSprite(app.set_tft_tools_wifi_scan_config.targetBgColor);

        if ( edit_btn_up.shortPress() ){

            wifi_scan_btn_move = true;

            wifi_scan_network_index ++;

            if(wifi_scan_network_index >= wifi_scan_network_count/*networks_length*/){
                wifi_scan_network_index = -1;
            } 

        }

        if ( edit_btn_enter.longPress() ){
            
            app.setString("ROUTER_SSID", app.WIFI_SCAN_LIVE_SSID.c_str());
            
            wifi_scan_btn_move = false;

            wifi_scan_view->deleteSprite();

            scan_results_ready = true;
            
        }

        if ( edit_btn_down.shortPress() ){

            wifi_scan_btn_move = true;

            wifi_scan_network_index --;

            if(wifi_scan_network_index <= -1){
                wifi_scan_network_index = wifi_scan_network_count/*elements_length*/;
            } 
        }

        wifi_scan_view->fillSprite(app.set_tft_tools_wifi_scan_config.targetBgColor);

        String net_count = String(wifi_scan_network_count) + " networks found";

        wifi_scan_view->setFreeFont(&DejaVu_Sans_Bold_11);
        wifi_scan_view->setTextColor(TFT_BLUE, app.set_tft_tools_wifi_scan_config.targetBgColor);
        wifi_scan_view->drawString(net_count, 20, 5); // networks found

        wifi_scan_view->setFreeFont(&DejaVu_Sans_Bold_11);
        wifi_scan_view->setTextColor(TFT_WHITE, app.set_tft_tools_wifi_scan_config.targetBgColor);

        //Serial.print("LINE_1: "); Serial.println(wifi_scan_first_line_y);

        for (int t = 0; t < wifi_scan_network_count; t++) {
            
            int _no_count = t + 1;
            
            if(t <= 11){

                no_x         = 20;
                x            = (no_x + 30);

                wifi_scan_view->setTextColor(TFT_WHITE, app.set_tft_tools_wifi_scan_config.targetBgColor);
                wifi_scan_view->drawNumber(_no_count, no_x, wifi_scan_first_line_y); // plot the number of network
                wifi_scan_view->drawString(SSID_ARRY[t], x, wifi_scan_first_line_y); // plot the network ssid

                if(t == wifi_scan_network_index){

                    app.WIFI_SCAN_LIVE_SSID = SSID_ARRY[wifi_scan_network_index];

                    fill_x = (x - 5);
                    fill_y = (wifi_scan_first_line_y - 5);
                    ssid_w = (190);

                    wifi_scan_view->fillRoundRect(fill_x, fill_y, ssid_w, 20, 4, TFT_BLACK);
                    wifi_scan_view->setTextColor(TFT_WHITE, TFT_BLACK);
                    wifi_scan_view->drawString(SSID_ARRY[wifi_scan_network_index], (fill_x + 5), (fill_y + 5)); // plot the network ssid

                    Serial.print("LINE_2: "); Serial.println(wifi_scan_first_line_y);
                    
                }
                
                wifi_scan_first_line_y = wifi_scan_first_line_y + 20;

                if(t == 11){
                    wifi_scan_first_line_y = 20;
                }                
            }
            else
            {
                
                no_x         = 260;
                x            = (no_x + 30);

                wifi_scan_view->setTextColor(TFT_WHITE, app.set_tft_tools_wifi_scan_config.targetBgColor);
                wifi_scan_view->drawNumber(_no_count, no_x, wifi_scan_first_line_y); // plot the number of network
                wifi_scan_view->drawString(SSID_ARRY[t], x, wifi_scan_first_line_y); // plot the network ssid
                
                if(t == wifi_scan_network_index){

                    app.WIFI_SCAN_LIVE_SSID = SSID_ARRY[wifi_scan_network_index];

                    fill_x = (x - 5);
                    fill_y = (wifi_scan_first_line_y - 5);
                    ssid_w = (190);

                    wifi_scan_view->fillRoundRect(fill_x, fill_y, ssid_w, 20, 4, TFT_BLACK);
                    wifi_scan_view->setTextColor(TFT_WHITE, TFT_BLACK);
                    wifi_scan_view->drawString(SSID_ARRY[wifi_scan_network_index], (fill_x + 5), (fill_y + 5)); // plot the network ssid

                    //Serial.print("LINE_2: "); Serial.println(wifi_scan_first_line_y);
                }

                wifi_scan_first_line_y = wifi_scan_first_line_y + 20;
            }
        
        
        }

        wifi_scan_first_line_y = 20;
    }

    wifi_scan_view->pushToSprite(&target, app.set_tft_tools_wifi_scan_config.x, app.set_tft_tools_wifi_scan_config.y);
}