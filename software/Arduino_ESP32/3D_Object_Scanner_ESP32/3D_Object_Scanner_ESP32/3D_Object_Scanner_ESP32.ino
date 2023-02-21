
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

//? OLED Definitions:
  Adafruit_SSD1306 OLED_DISPLAY(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//* State Variable ,Enum & Structure Definitions

//global 
enum  MACHINE_state {IN_MENU, IN_PROGRESS, IDLE};

//when MACHINE is set to IN_PROGRESS
enum IN_PROGRESS_state {OBJECT_SELECT, SCANNING, COMPLETED};



//In Menu Structure
struct {
  int idle_tick; //Amount of time since last user input (When user goes A.F.K)

} IN_MENU_struct;

//In Progress Structure
struct {
  IN_PROGRESS_state state;
  int current_step; //Step the motor is at
  //TODO: Fill this more
} IN_PROGRESS_struct;


void setup() {
  //ESP32 - Wrover Cam Board runs on 115.2k baud
  Serial.begin(115200);

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

  //! pinMode(stepPin,OUTPUT); 
  //! pinMode(dirPin,OUTPUT);

  delay(2000);

}

void loop() {
  // put your main code here, to run repeatedly:

    //! int microDelay = 1000;
    //! for(int x = 0; x < 200; x++) {
    //!  digitalWrite(stepPin,HIGH); 
    //!  delayMicroseconds(microDelay); 
    //!  digitalWrite(stepPin,LOW); 
    //!  delayMicroseconds(microDelay); 
    //! }

  delay(100); // One second delay
  Serial.println("Rotated");

  JOYSTICK_BUTTON.loop();

  // read X and Y analog values
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);
  buttonValue = JOYSTICK_BUTTON.getState();


  if (JOYSTICK_BUTTON.isPressed()) {
    Serial.println("The button is pressed");
    // TODO do something here
  }

  if (JOYSTICK_BUTTON.isReleased()) {
    Serial.println("The button is released");
    // TODO do something here
  }

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(valueX);
  Serial.print(", y = ");
  Serial.println(valueY);
  Serial.print("BV: ");
  Serial.println(buttonValue);
  delay(200);

}
