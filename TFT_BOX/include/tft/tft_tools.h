#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "InputButton.h"
#include "files/spiffsio_helper.h"

#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

int16_t txtColor;
int16_t bgColor;

void setTxtColor(int16_t color){txtColor = color;};

void setBgColor(int16_t color){bgColor = color;};

/* To creat an edite pallet */
void createEditPallet(int32_t x, int32_t y, int16_t w, int16_t h);









#ifdef __cplusplus
}
#endif /**< _cplusplus */