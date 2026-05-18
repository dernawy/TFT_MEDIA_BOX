#ifndef TFT_TOOLS_H
#define TFT_TOOLS_H

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include "tft_media.h"
#include "tft/tft_helper.h"


class tft_tools {
    private:
        /* data */
        int32_t _button_x;
        int32_t _button_y;
        int16_t _button_w; 
        int16_t _button_h;
        int16_t _btn_txt_w;
        int16_t _btn_txt_h;
        int32_t _button_fill_x;
        int32_t _button_fill_y;
        int32_t _button_fill_w;
        int32_t _button_fill_h;
        uint16_t _btnsCadreColor;
        int16_t _pallet_x;
        int16_t _pallet_y;
        int16_t _pallet_w; 
        int16_t _pallet_h;
        uint16_t _edit_pallet_input_bgColor;
        uint16_t _input_cadreColor;
        bool _show_edit_palet;
        int _remined_espirets_index;
        int _current_espirets_index;
        int _pallet_input_x;
        int _pallet_input_y;
        int _pallet_input_w;
        int _pallet_input_h;
        
        bool _pallet_edit_move;
        bool _still_edit;
        bool _use_caps;
        bool _use_nums;
        int _edit_index;
        int _edit_chars_index;
        int _elements_index;
        bool _edit_error;
        String _edit_error_msg;
        String _last_char;
        String EDIT_PALLET_LIVE_TEXT;
        int _last_char_length;
        int _pallet_live_text_length;
        
        bool _gif_view_created;
        const char *_gif_filename;

        bool _wifi_init;
        bool _use_wifi_scan_target_bg;
        bool _start_wifi_scan;
        bool _wifi_scan_done;
        
        

    public:

        tft_tools(/* args */);
        ~tft_tools();

        /* To creat an inegrated edite button */
        void createEditButton(TFT_eSprite &target);

        void setButtonTxtColor(uint16_t color);
        void setButtonTxtEditColor(uint16_t color);
        void setButtonCadreColor(uint16_t color);
        void setButtonFillColor(uint16_t color);
        void setButtonBgColor(uint16_t color);
        void setButtonTargetBgColor(uint16_t color);
        void setButtonTxtFont(const GFXfont *font);

        /* To creat an inegrated edite pallet */
        void createEditPallet(TFT_eSprite &target);

        String pallet_backspace();
        void setPalletTxtColor(uint16_t color);
        void setPalletTxtEditColor(uint16_t color);
        void setPalletCadreColor(uint16_t color);
        void setPalletFillColor(uint16_t color);
        void setPalletBgColor(uint16_t color);
        void setPalletTargetBgColor(uint16_t color);
        void setPalletTxtFont(const GFXfont *font);

        int str_array_bigger_length(TFT_eSprite &target, String array[], int length);
        int str_array_bigger_length(TFT_eSprite *target, String array[], int length);

        void tft_wifi_scan(TFT_eSprite &target);
        void set_start_wifi_scan(bool scan);
        void setScanResultsStarted(bool started);
        bool scanResultsReady();

        bool get_wifi_scan_done();

        String paswwordToStarts(String password);

       
        

};

    




















#endif /* TFT_TOOLS_H */