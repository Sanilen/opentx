/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

const char * warningText = NULL;
const char * warningInfoText;
uint8_t         warningInfoLength;
uint8_t         warningType;
uint8_t         warningResult = 0;

uint8_t         warningInfoFlags = ZCHAR;
int16_t         warningInputValue;
int16_t         warningInputValueMin;
int16_t         warningInputValueMax;

void drawMessageBox()
{
  lcdDrawFilledRect(10, 16, LCD_W-20, 40, SOLID, ERASE);
  lcdDrawRect(10, 16, LCD_W-20, 40);
  lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y, warningText, WARNING_LINE_LEN);
  // could be a place for a warningInfoText
}

void showMessageBox(const char * str)
{
  warningText = str;
  drawMessageBox();
  warningText = NULL;
  lcdRefresh();
}

#if !defined(PCBI6) || !defined(DEBUG)
const unsigned char ASTERISK_BITMAP[]  = {
#include "asterisk.lbm"
};
#endif

void drawAlertBox(const char * title, const char * text, const char * action)
{
  lcdClear();
#if !defined(PCBI6) || !defined(DEBUG)
  lcdDraw1bitBitmap(2, 0, ASTERISK_BITMAP, 0, 0);
#else
  lcdDrawRect(2, 2, 32 - 4, 32 - 4);
  lcdDrawText(11, 6, "x", DBLSIZE); 
#endif

#define MESSAGE_LCD_OFFSET   6*FW

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcdDrawText(MESSAGE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcdDrawText(MESSAGE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcdDrawText(MESSAGE_LCD_OFFSET, 0, title, DBLSIZE);
  lcdDrawText(MESSAGE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif
  
  lcdDrawSolidFilledRect(0, 0, LCD_W, 32);
  if (text) {
    lcdDrawTextAlignedLeft(5*FH, text);
  }
  if (action) {
    lcdDrawTextAlignedLeft(7*FH, action);
  }
  
#undef MESSAGE_LCD_OFFSET
}

void showAlertBox(const char * title, const char * text, const char * action , uint8_t sound)
{
  drawAlertBox(title, text, action);
  
  AUDIO_ERROR_MESSAGE(sound);
  
  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
  backlightOn();
  checkBacklight();
}

void runPopupWarning(event_t event)
{
  warningResult = false;
  drawMessageBox();
  if (warningInfoText) {
    lcdDrawSizedText(WARNING_LINE_X, WARNING_LINE_Y+FH, warningInfoText, warningInfoLength, WARNING_INFO_FLAGS);
  }
  lcdDrawText(WARNING_LINE_X, WARNING_LINE_Y+2*FH, warningType == WARNING_TYPE_ASTERISK ? STR_EXIT : STR_POPUPS);
  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (warningType == WARNING_TYPE_ASTERISK)
        break;
      warningResult = true;
      // no break
    case EVT_KEY_BREAK(KEY_EXIT):
      warningText = NULL;
      warningType = WARNING_TYPE_ASTERISK;
      break;
    default:
      if (warningType != WARNING_TYPE_INPUT) break;
      s_editMode = EDIT_MODIFY_FIELD;
      warningInputValue = checkIncDec(event, warningInputValue, warningInputValueMin, warningInputValueMax);
      s_editMode = EDIT_SELECT_FIELD;
      break;
  }
}
