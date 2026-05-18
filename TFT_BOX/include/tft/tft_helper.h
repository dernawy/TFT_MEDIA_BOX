
#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "InputButton.h"
#include "files/spiffsio_helper.h"


#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

/**************************************************************************************************************************************************************************
*                                                                        TFT BUTTONS DEFINITIONS
**************************************************************************************************************************************************************************/

#define BUTTON_UP    27 // Pulled down
#define BUTTON_ENTER 32 // Pulled down
#define BUTTON_DOWN  33 // Pulled down
#define BUTTON_EDIT  34 // Pulled down



typedef enum {
    BTN_VOLLONG,          BTN_VOLSHORT,
    BTN_SEEKLONG,         BTN_SEEKSHORT, 
    BTN_B1LONG,           BTN_B1SHORT, 
    BTN_B2LONG,           BTN_B2SHORT, 
    BTN_B3LONG,           BTN_B3SHORT,
    BTN_B4LONG,           BTN_B4SHORT,
    ERROR,
} ControlButton;

typedef enum {
    I_BUTTON,
    I_KEY,
    I_SEEK1, 
    I_SEEK2,
} PlayerInputType;


/**************************************************************************************************************************************************************************
*                                                                        TFT GENERAL DEFINITIONS
**************************************************************************************************************************************************************************/
#define TFT_GREY 0x7BEF

/************** BACK LIGHT **************/
#define BACK_LIGHT_PIN        12
#define BACK_LIGHT_CHANNEL    4
#define BACK_LIGHT_FREQUENCY  12000
#define BACK_LIGHT_RESOLUTION 8

#define STRING_ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

/**************************************************************************************************************************************************************************
*                                                                           TFT CONSTRUCTERS
**************************************************************************************************************************************************************************/

/************** TFT GENERAL DECLARATION **************/
#define TFT_W 480
#define TFT_W_1 480
#define TFT_H 320
#define TOP_BAR_W TFT_W
#define TOP_BAR_H 40

#define KYEPAD_CAPITAL_SMAL_LENGTH 27 
#define KYEPAD_NUM_SIGN_LENGTH     33 


/************** TFT GENERAL CONFIG **************/

typedef enum {
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240X240,  // 240x240
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_ERROR,
} video_stream_framesize_t;

typedef struct {
    uint8_t orientation;
    uint16_t bgColor;
    bool enable_animation;
    bool logo_on_startup;
    uint32_t logo_delay;
    bool show_errors;
    bool show_top_bar;
    bool enable_v_stream;;
    
    struct {
        bool show;
        int16_t width;
        int16_t hieght; 
        const GFXfont *font;
        uint16_t bgColor;
        uint16_t txtColor;
        bool notification;
        bool clock;
        String time;
        String day;
        String notif_message;

    } top_bar_config;

    struct {
        int16_t width;
        int16_t hieght;
        int16_t upper_section_width;
        int16_t upper_section_hieght;
        uint16_t upperBgColor;
        bool show_upper_section;
        const GFXfont *font;
        uint16_t bgColor;
        uint16_t txtColor;
        bool show_top_bar;
        int temp_unit;
    } main_page_config;
} tft_general_config_t;

#define TFT_DESKTOP_CONFIG_DEFAULT() { \
    .orientation             = 3, \
    .bgColor                 = TFT_BLACK, \
    .enable_animation        = false, \
    .logo_on_startup         = true, \
    .logo_delay              = 3000, \
    .show_errors             = true, \
    .show_top_bar            = true, \
    .enable_v_stream         = true, \
    .top_bar_config          = { \
        .show          = true, \
        .width         = TOP_BAR_W, \
        .hieght        = TOP_BAR_H, \
        .font          = &DejaVu_Sans_Bold_11, \
        .bgColor       = TFT_RED, \
        .txtColor      = TFT_WHITE, \
        .notification  = false, \
        .clock         = true, \
        .time          = "--:--: --", \
        .day           = "---------", \
        .notif_message = "", \
    }, \
    .main_page_config = { \
        .width                = TFT_W, \
        .hieght               = 0, \
        .upper_section_width  = TFT_W_1, \
        .upper_section_hieght = 100, \
        .upperBgColor         = TFT_BLUE, \
        .show_upper_section   = true, \
        .font                 = &DejaVu_Sans_Bold_11, \
        .bgColor              = TFT_BLUE, \
        .txtColor             = TFT_WHITE, \
        .show_top_bar         = true, \
        .temp_unit            = 0, \
    }, \
}

typedef struct {
    bool show;
    int16_t width;
    int16_t hieght; 
    const GFXfont *font;
    uint16_t bgColor;
    uint16_t txtColor;
    bool notification;
    bool clock;
    String time;
    String day;
    String notif_message;

} top_bar_config_t;

typedef struct {
    int16_t width;
    int16_t hieght;
    int16_t upper_section_width;
    int16_t upper_section_hieght;
    uint16_t upperBgColor;
    bool show_upper_section;
    const GFXfont *font;
    uint16_t bgColor;
    uint16_t txtColor;
    bool show_top_bar;
    int temp_unit;
    String elements[20];
} main_page_config_t;

typedef struct {
    int16_t video_x;
    int16_t video_y;
    int16_t width;
    int16_t height; 
    int16_t bgColor;
    const GFXfont *font;
    uint16_t txtColor;
    bool controlled;
    bool show_controls;
    String vars[10];
    String vals[10];

} video_stream_config_t;

#define TFT_VIDEO_STREAM_CONFIG_DEFAULT() { \
    .video_x       = 180, \
    .video_y       = 170, \
    .width         = 160, \
    .height        = 120, \
    .bgColor       = TFT_BLACK, \
    .font          = &DejaVu_Sans_Bold_11, \
    .txtColor      = TFT_WHITE, \
    .controlled    = true, \
    .show_controls = true, \
}


typedef struct {
    bool edit;
    int chars_type; // 0 = letters | 1 = numbers
    int x;
    int y;
    int w;
    int h;
    int bgColor;
    uint16_t txtColor;
    const GFXfont *font;
    int current_index;
    int loop_index;
    int16_t elem_x;
    int16_t elem_y;
    int16_t elem_w;
    int16_t elem_h;
    String key;

} edit_pallet_t;
/************** TFT ORENTATION **************/
typedef enum {
    TFT_ORI_PORTRAT_U,
    TFT_ORI_PORTRAT_D,
    TFT_ORI_LANDSCAPE_L,
    TFT_ORI_LANDSCAPE_R,
    ORI_ERROR           = -1
} diplay_orientation_t;

diplay_orientation_t tft_current_ori();

/******************* TFT TOOLS *****************/
typedef struct {
    int16_t button_x;
    int16_t button_y;
    int16_t button_w; 
    int16_t button_h;
    String btn_txt;
    int16_t btn_txt_w;
    int16_t btn_txt_h;
    bool static_button;
    const GFXfont *txtFont;
    uint16_t txtColor;
    uint16_t txtEditColor;
    uint16_t cadrColor;
    uint16_t fillColor;
    uint16_t bgColor;
    uint16_t targetBgColor;
    
} tft_tools_button_config_t;

#define TFT_TOOLS_BUTTON_CONFIG_DEFAULT() { \
    .button_x      = 0, \
    .button_y      = 0, \
    .button_w      = 0, \
    .button_h      = 0, \
    .btn_txt       = "EDIT", \
    .btn_txt_w     = 0, \
    .btn_txt_h     = 0, \
    .static_button = false, \
    .txtFont       = &DejaVu_Sans_Bold_12, \
    .txtColor      = TFT_WHITE, \
    .txtEditColor  = TFT_BLACK, \
    .cadrColor     = TFT_WHITE, \
    .fillColor     = TFT_ORANGE, \
    .bgColor       = TFT_BLACK, \
    .targetBgColor = TFT_BLACK, \
}

typedef struct {
    int chars_type;
    int current_index;
    int loop_index;
    String key;
    bool show_edit_palet;
    bool edit;
    bool use_target_bg;
    int16_t pallet_x;
    int16_t pallet_y;
    int16_t pallet_w; 
    int16_t pallet_h;
    const GFXfont *txtFont;
    uint16_t input_bgColor;
    uint16_t input_cadreColor;
    uint16_t txtColor;       // btn text color while no foucs on this btn
    uint16_t btnsCadreColor; // btn cadre color if want usnig deferent color of pallet cadre color
    uint16_t txtEditColor;   // btn text color while foucs on this btn
    uint16_t cadrColor;      // pallet cadre color
    uint16_t fillColor;      // fill color for buttons while foucs on this button
    uint16_t bgColor;        // pallet bg color
    uint16_t targetBgColor;  // target sprit bg color
    
} tft_tools_pallet_config_t;

#define TFT_TOOLS_PALLET_CONFIG_DEFAULT() { \
    .chars_type       = 0, \
    .current_index    = 0, \
    .loop_index       = 0, \
    .key              = "", \
    .show_edit_palet  = false, \
    .edit             = false, \
    .use_target_bg    = false, \
    .pallet_x         = 0, \
    .pallet_y         = 0, \
    .pallet_w         = 193, \
    .pallet_h         = 100, \
    .txtFont          = &DejaVu_Sans_Bold_12, \
    .input_bgColor    = TFT_EDIT_PALLET_INPUT_BG, \
    .input_cadreColor = TFT_EDIT_PALLET_INPUT_CADRE, \
    .txtColor         = TFT_WHITE, \
    .btnsCadreColor   = TFT_WHITE, \
    .txtEditColor     = TFT_EDIT_BTN_FILL, \
    .cadrColor        = TFT_WHITE, \
    .fillColor        = TFT_EDIT_PALLET_BG, \
    .bgColor          = TFT_EDIT_PALLET_BG, \
    .targetBgColor    = TFT_BLACK, \
}

/******************* TFT SETUP *****************/
typedef struct {
    String main_title_txt;
    String intro_txt;
    String start_btn_txt;
    String start_btn_info_txt;
    int title_x;
    int title_y;
    int title_w;
    int title_h;
    int intro_x;
    int intro_y;
    int intro_w;
    int intro_h;
    int start_btn_x;
    int start_btn_y;
    int start_btn_w;
    int start_btn_h;
    int start_btn_info_x;
    int start_btn_info_y;
    int start_btn_info_w;
    int start_btn_info_h;
    uint16_t setup_bgColor;
    int setup_step;
    bool show_edit_palet;
} tft_device_setup_screen_config_t;

#define TFT_DEVICE_SETUP_SCREEN_CONFIG_DEFAULT() { \
    .main_title_txt     = "", \
    .intro_txt          = "", \
    .start_btn_txt      = "", \
    .start_btn_info_txt = "", \
    .title_x            = 0, \
    .title_y            = 30, \
    .title_w            = 0, \
    .title_h            = 0, \
    .intro_x            = 0, \
    .intro_y            = 0, \
    .intro_w            = 0, \
    .intro_h            = 0, \
    .start_btn_x        = 0, \
    .start_btn_y        = 0, \
    .start_btn_w        = 0, \
    .start_btn_h        = 0, \
    .start_btn_info_x   = 0, \
    .start_btn_info_y   = 0, \
    .start_btn_info_w   = 0, \
    .start_btn_info_h   = 0, \
    .setup_bgColor      = TFT_SETUP_BG, \
    .setup_step         = -1, \
    .show_edit_palet    = false, \
}

/********************* TFT WIFI *******************************/
typedef struct {
    int16_t x;
    int16_t y;
    int16_t w; 
    int16_t h;
    const GFXfont *txtFont;
    uint16_t txtColor;
    uint16_t cadrColor;
    uint16_t fillColor;
    uint16_t bgColor;
    uint16_t targetBgColor;
    bool delete_spiret;
    
} tft_tools_wifi_scan_config_t;

#define TFT_WIFI_SCAN_SCREEN_CONFIG_DEFAULT() { \
    .x             = 0, \
    .y             = 0, \
    .w             = 0, \
    .h             = 0, \
    .txtFont       = &DejaVu_Sans_Bold_12, \
    .txtColor      = TFT_WHITE, \
    .cadrColor     = TFT_WHITE, \
    .fillColor     = TFT_EDIT_PALLET_BG, \
    .bgColor       = TFT_EDIT_PALLET_BG, \
    .targetBgColor = TFT_BLACK, \
    .delete_spiret = false, \
}


/**************************************************************************************************************************************************************************
*                                                                   PUBLIC FUNCTIONS FOR TFT SCREEN
**************************************************************************************************************************************************************************/
void init_error_view(int32_t y);

int biggerLength(String array[], int length);

void init_display_backlight();

static uint8_t getTftWidth();

static uint8_t getTftHieght();

bool create_tft_config_queu();

void send_to_tft_config_queu(void *const pxConfig);

bool create_top_bar_config_queu();

void send_to_top_bar_config_queu(void *const pxConfig);

bool create_desktop_config_queu();

void send_to_desktop_config_queu(void *const pxConfig);

bool create_video_stream_queu();

void send_to_video_stream_queu(void *const pxConfig);

void send_video_stream_settings(String var[], String val[], int index);

/**************************************************************************************************************************************************************************
*                                                                            ERRORS SCREENS
**************************************************************************************************************************************************************************/

void error_message(TFT_eSPI t, int32_t  x, int32_t  y);

void error_message_s(String msg, int32_t y, bool erase=false);

void spiffs_errors(filesio_info_t info);

#define TFT_SPIFFS_ERRORS_CHECKS(info)  { \
    spiffs_errors(info);\
}

void spiffs_operations_messages(String msg[]);


#define TFT_FLASH_MESSAGET_S(msg, y, erase)  { \
    error_message_s(msg, y, erase);\
}

/**************************************************************************************************************************************************************************
*                                                                   ANIMATIONS GENERAL DEFINITIONS
**************************************************************************************************************************************************************************/


#ifdef __cplusplus
}
#endif /**< _cplusplus */