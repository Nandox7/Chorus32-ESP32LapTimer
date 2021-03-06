#ifdef OLED

#include <Wire.h>
#include "SSD1306.h"
#include "Font.h"
#include "Timer.h"
#include "Screensaver.h"

//displayScreenNumber = 0;
uint8_t  numberOfOledScreens = 2; // Increment if a new screen is added to cycle through.

uint8_t oledRefreshTime = 50;
Timer oledTimer = Timer(oledRefreshTime);

SSD1306  display(0x3c, 21, 22);  // 21 and 22 are default pins

const TickType_t xDelay = 500 / portTICK_PERIOD_MS;

void oledSetup(void) {
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.drawFastImage(0, 0, 128, 64, ChorusLaptimerLogo_Screensaver);
  display.display();
  display.setFont(Dialog_plain_9);
}

void OLED_CheckIfUpdateReq() {
  if (oledTimer.hasTicked()) {
    oledUpdate();
    oledTimer.reset();
  }
  //Serial.println(VbatReadingRaw);

  if (ADCVBATmode != INA219) {
    VbatReadingFloat = fmap(VbatReadingSmooth * VBATcalibration, 0, 4096, 0, 4.4);
  }
  //Serial.println(VbatReading);
}

void oledUpdate(void)
{
  display.clear();

  if (displayScreenNumber % numberOfOledScreens == 0) {
    // Display on time
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    // Hours
    if (millis() / 3600000 < 10) {
      display.drawString(0, 0, "0" + String(millis() / 3600000) + ":");
    } else {
      display.drawString(0, 0, String(millis() / 3600000) + ":");
    }
    // Mins
    if (millis() % 3600000 / 60000 < 10) {
      display.drawString(18, 0, "0" + String(millis() % 3600000 / 60000) + ":");
    } else {
      display.drawString(18, 0, String(millis() % 3600000 / 60000) + ":");
    }
    // Seconds
    if (millis() % 60000 / 1000 < 10) {
      display.drawString(36, 0, "0" + String(millis() % 60000 / 1000));
    } else {
      display.drawString(36, 0, String(millis() % 60000 / 1000));
    }

    // Voltage
    if (ADCVBATmode != 0) {
      display.setTextAlignment(TEXT_ALIGN_RIGHT);
      display.drawString(127, 0, String(VbatReadingFloat, 2) + "V");
    }

    if (ADCVBATmode == INA219) {
      display.drawString(90, 0, String(mAReadingFloat/1000, 2) + "A");
    }


    // Rx modules
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    for (int i = 0; i < NumRecievers; i++) {
      display.drawString(0, 9 + i * 9, getBandLabel(RXBand[i]) + String(RXChannel[i] + 1) + ", " + String(ADCvalues[i] / 12));
      if (ADCvalues[i] < 600) {
        display.drawProgressBar(40, 10 + i * 9, 127 - 42, 8, map(600, 600, 3500, 0, 85));
      } else {
        display.drawProgressBar(40, 10 + i * 9, 127 - 42, 8, map(ADCvalues[i], 600, 3500, 0, 85));
      }
      display.drawVerticalLine(40 + map(RSSIthresholds[i], 600, 3500, 0, 85),  10 + i * 9, 8); // line to show the RSSIthresholds
    }
  } else if (displayScreenNumber % numberOfOledScreens == 1) {
    if (ADCVBATmode != 0) {
      display.setTextAlignment(TEXT_ALIGN_LEFT);

      display.drawString(0, 0, "ADC loop " + String(adcLoopCounter * (1000.0 / oledRefreshTime)) + " Hz");
      adcLoopCounter = 0;

      display.drawString(0, 9, String(mAReadingFloat) + " mA");
    }
  }

  display.display();
}

#endif
