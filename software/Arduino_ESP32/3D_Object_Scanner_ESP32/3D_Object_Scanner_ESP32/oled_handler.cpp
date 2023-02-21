#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Header File containing the `oled_handler` functions
#include "handler.h"

void draw_start_up_logo(Adafruit_SSD1306 OLED_DISPLAY) {
  OLED_DISPLAY.clearDisplay();

  //LOGO

  OLED_DISPLAY.setTextSize(1);             // Normal 1:1 pixel scale
  OLED_DISPLAY.setTextColor(SSD1306_WHITE);        // Draw white text
  OLED_DISPLAY.setCursor(0,0);             // Start at top-left corner
  OLED_DISPLAY.println(F("By: jasonmzx"));

  OLED_DISPLAY.display();
  return;
}

