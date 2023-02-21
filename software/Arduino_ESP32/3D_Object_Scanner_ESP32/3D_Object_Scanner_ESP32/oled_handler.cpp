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

void draw_menu(Adafruit_SSD1306* OLED_DISPLAY, bool selected) {
  //? selected : false (RIGHT Selected), true (LEFT Selected)  
  OLED_DISPLAY->clearDisplay();

  //Always Render:
  OLED_DISPLAY->setTextSize(1);             // Normal 1:1 pixel scale
  OLED_DISPLAY->setTextColor(SSD1306_WHITE);        // Draw white text
  OLED_DISPLAY->setCursor(0,0);             // Start at top-left corner
  OLED_DISPLAY->println(F("3D OBJ-SCAN MENU:"));

  OLED_DISPLAY->setTextSize(2);
  
  //Left component
  OLED_DISPLAY->setCursor(5,35); 
  OLED_DISPLAY->println(F("AUTO"));
  //Right component
    OLED_DISPLAY->setTextSize(1.5);
    OLED_DISPLAY->setCursor(80,35); 
    OLED_DISPLAY->println(F("MANUAL"));

  //Conditional Rendering:
  if(selected == false) {
      OLED_DISPLAY->fillRoundRect(0, 16, 55, 48, 20, SSD1306_INVERSE);
      OLED_DISPLAY->drawRoundRect(70, 16, 55, 48, 20, SSD1306_INVERSE);
  } else {
      OLED_DISPLAY->drawRoundRect(0, 16, 55, 48, 20, SSD1306_INVERSE);
      OLED_DISPLAY->fillRoundRect(70, 16, 55, 48, 20, SSD1306_INVERSE);
  }
  OLED_DISPLAY->display();
}

