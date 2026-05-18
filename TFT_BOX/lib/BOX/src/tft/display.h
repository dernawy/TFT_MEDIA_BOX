#ifndef TFTDISPLAY_H
#define TFTDISPLAY_H

#include "Arduino.h"

#include "setup_screen.h"


class display : public TFT_eSPI, public setup_screen {

    private:
        /* data */
        bool _show_logo;
        uint32_t _logo_delay;
        bool _enable_animation;
        bool _show_errors;
        int16_t _current_width;
        int16_t _current_hieght;
        uint8_t _default_rotation;
        uint8_t _current_rotation;
        bool _show_top_bar;

        void setPrvShowLogo(bool show_logo);
        void setPrvDefaultRotation(uint8_t rotation);
        void setPrvCurrentRotation(uint8_t rotation);
        
        String key_pad_backspace(String live_text);
        String setup_key_pad_backspace(String live_text);
        void createEditBtn(int step, TFT_eSprite target, int32_t x, int32_t y, int16_t txtColor, int16_t cadreColor, int16_t fillColor, const GFXfont *font);

        

    public:
        display(/* args */);
        ~display();

        void configTft(tft_general_config_t *config);
        void controlTopbar(top_bar_config_t *config);
        void controlDesktopPage(main_page_config_t *config);
        void desktop_page();
        void video_stream_init();
        void config_video_stream_esperit(video_stream_config_t *config);
        void control_video_stream();
        void restart_video_stream();
        void setting_page(main_page_config_t *config);
        void setting_location_page_(main_page_config_t *config);
        void init_display();
        void init_display_backlight();
        void setup_edit_pallet(edit_pallet_t pallet);
        void first_setup_display();
        void logo();
        bool getShowLogo();
        uint8_t getCurrentWidth();
        uint8_t getCurrentHieght();
        uint8_t getCurrentRotation();

        void onscreen_keypad();
        void edit_button(edit_pallet_t pallet);

        void top_bar_clock_screen(top_bar_config_t *config);



};





#endif /* #ifndef TFTDISPLAY_H */