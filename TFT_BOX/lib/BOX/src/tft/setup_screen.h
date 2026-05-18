#ifndef SETUP_SCREEN_H
#define SETUP_SCREEN_H

#include "Arduino.h"
#include <TFT_eSPI.h>
#include "tft_tools.h"


class setup_screen {
    
    private:
        /* data */
        String _main_title_txt;
        String _intro_txt;
        String _start_btn_txt;
        String _start_btn_info_txt;
        String _device_name;
        String _device_model;
        String _firmware_ver;
        String _firmware_name;
        String _has_psram;
        String _psram_size;
        String _flash_size;
        String _ram_size;
        String _has_wifi;
        String _has_ble;

        int _title_x;
        int _title_y;
        int _title_w;
        int _title_h;
        int _intro_x;
        int _intro_y;
        int _intro_w;
        int _intro_h;
        int _start_btn_x;
        int _start_btn_y;
        int _start_btn_w;
        int _start_btn_h;
        int _start_btn_info_x;
        int _start_btn_info_y;
        int _start_btn_info_w;
        int _start_btn_info_h;

        int _setup_step;
        
        uint16_t _setup_bgColor;
        bool _show_edit_palet;

        bool _setup_country_ok;


    public:
        setup_screen();
        ~setup_screen();

        void main_setup_display();
        void setting_location_page();
};




#endif /* SETUP_SCREEN_H */