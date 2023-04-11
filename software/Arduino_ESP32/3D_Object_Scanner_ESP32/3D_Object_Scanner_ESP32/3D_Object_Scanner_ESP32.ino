
//Stepper Driver Pins (for Step-Motor IO)
//! const int stepPin = 12; 
//! const int dirPin = 14; 

//Library Dependencies:

  //For Joystick:
#include <ezButton.h>

  //For OLED Display:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Local Dependencies:

#include "pins.h"
#include "handler.h"

//? Joystick Definitions:

  ezButton JOYSTICK_BUTTON(SW_PIN);

  int valueX = 0; // to store the X-axis value
  int valueY = 0; // to store the Y-axis value
  int buttonValue = 0;

  //Joystick Decoding:
  const int JS_HIGH = 4095;
  const int JS_LOW = 0; 

  int joystick_analog_decode(int analog) {
      if(JS_HIGH == analog){ return 1; }
      if(JS_LOW == analog){return -1;}
      if(JS_LOW < analog < JS_HIGH){return 0;}
  }

//? OLED Definitions:
  Adafruit_SSD1306 OLED_DISPLAY(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//? NEMA 17 Stepper Motor Definitions:
const int stepPin = 32; 
const int dirPin = 33; 


//* State Variable ,Enum & Structure Definitions

//global 
enum  MACHINE_state {MENU, AUTO, MANUAL, IDLE};

//when MACHINE is set to AUTO_PROGRESS:
enum AUTO_PROGRESS_state {OBJECT_SELECT, SCANNING, AUTO_COMPLETE};

//when MACHINE is set to MANUAL_PROGRESS:
enum MANUAL_PROGRESS_state{PROGRESS, MANUAL_COMPLETE};

//* State definitions:

MACHINE_state GLOBAL;

AUTO_PROGRESS_state AUTO_S;
MANUAL_PROGRESS_state MANUAL_S;


//TODO: fix this design pattern?

struct {             
  bool selected_component; //8 bit integer that holds the position from Left X (Most left X component = 0)
} menu;       


void setup() {
  //ESP32 - Wrover Cam Board runs on 115.2k baud
  Serial.begin(115200);

  //Activating Pins for Stepper Motor:
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

  //for joystick
   JOYSTICK_BUTTON.setDebounceTime(50); // set debounce time to 50 milliseconds

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  //It's like an enum for a memory address in Adafruit's library
  if(!OLED_DISPLAY.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    //!Maybe remove this loop that blocks the thread forever?
    for(;;); // Don't proceed, loop forever
  }


  draw_start_up_logo(OLED_DISPLAY);
  GLOBAL = MENU;
  //! pinMode(stepPin,OUTPUT); 
  //! pinMode(dirPin,OUTPUT);

  delay(1000);

}

void loop() {

  //Main State Assertion:


  // put your main code here, to run repeatedly:

     int microDelay = 1000;
     for(int x = 0; x < 200; x++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(microDelay); 
      digitalWrite(stepPin,LOW); 
     }

  delay(100); // One second delay
  Serial.println("Rotated");

  JOYSTICK_BUTTON.loop();

  // read X and Y analog values
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);
  buttonValue = JOYSTICK_BUTTON.getState();

  int decodedX = joystick_analog_decode(valueX);


  if(GLOBAL == MENU){

    if(menu.selected_component > 0 && decodedX == -1){
      menu.selected_component = false;
    }

    if(menu.selected_component == 0 && decodedX == 1){
      menu.selected_component = true;
    }

    //Check if the user ends up selecting an option:
      if (JOYSTICK_BUTTON.isPressed()) {
      
        if(menu.selected_component == true){ //implies right
          GLOBAL = MANUAL;
        } else {
          GLOBAL = AUTO;
        }

      }


    draw_menu(&OLED_DISPLAY, menu.selected_component);
  
  }

  if(GLOBAL == MANUAL) {
    
    int microDelay = 1000;

      for(int x = 0; x < 200; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(microDelay); 
    digitalWrite(stepPin,LOW); 
      }
    delay(100);
  
    draw_manual(&OLED_DISPLAY);
  }

  if(GLOBAL == AUTO) {
    draw_manual(&OLED_DISPLAY);
  }



  if (JOYSTICK_BUTTON.isReleased()) {
    Serial.println("The button is released");
    // TODO do something here
  }

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(menu.selected_component);
  Serial.print("BV: ");
  Serial.println(buttonValue);
  delay(200);

}
