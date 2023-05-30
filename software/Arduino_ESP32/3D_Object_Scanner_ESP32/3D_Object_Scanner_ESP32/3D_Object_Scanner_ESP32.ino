
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

//ESP WIFI
#include <WiFi.h>
#include <WiFiMulti.h>

//Local Dependencies:

#include "pins.h"
#include "handler.h"

  //Camera Related Dependencies:

#include "esp_camera.h"  
#include "camera_handle.cpp"

WiFiMulti WiFiMulti;
camera_config_t config;

void config_init();

//? WIFI

const char *ssid_Router     = "wifina";  //input your wifi name
const char *password_Router = "1326Gabi@";  //input your wifi passwords


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
  delay(10);

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

  //Initializing Camera

  config_init(); //This function sets the `camera_config_t config` global var to the pins needed (defined in `pins.h`)

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, 0);        //1-Upside down, 0-No operation
  s->set_hmirror(s, 0);      //1-Reverse left and right, 0-No operation
  s->set_brightness(s, 1);   //up the blightness just a bit
  s->set_saturation(s, -1);  //lower the saturation

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid_Router, password_Router);

    Serial.println();
    Serial.println();
    Serial.print("Waiting for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
  }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void loop() {

    const uint16_t port = 8888;
    const char * host = "10.0.0.88"; // IPv4 internal (of my Laptop)

  //WIFI Client     
    WiFiClient client;
    int TCP_Connect = client.connect(host,port);
    delay(20);

    if (!TCP_Connect) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 5 seconds before retrying...");
        delay(5000);
        return;
    }

    Serial.println("Connected to server ! ( OK )");
    camera_fb_t *fb = NULL;
    fb = esp_camera_fb_get();


    if(!fb) {
      Serial.println("Camera capture failed...");
      return;
    } else {
      Serial.println("Camera Captured ( OK )");
    }

//Get Camera Sensor and push to Client

   const char *data = (const char*)fb->buf; //Cast

  Serial.print("Size of image:");
  Serial.println(fb->len);
  Serial.print("Shape->width:");
  Serial.print(fb->width);
  Serial.print("height:");
  Serial.println(fb->height);

  //client.write((char*)&(fb->width),sizeof(fb->width)); //Character Buffer for Width (bytes)
  //client.write((char*)&(fb->height),sizeof(fb->height)); //Character Buffer for Height (bytes)
  
  //Main State Assertion:


  // Give the server a chance to receive the information before sending an acknowledgement.
  delay(1000);
  //getResponse(client);
  Serial.print(data);
  client.write(data, fb->len);
  
  Serial.println("Disconnecting...");
  //client.stop();

  // put your main code here, to run repeatedly:

     int microDelay = 1000;
     for(int x = 0; x < 200; x++) {
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(microDelay); 
      digitalWrite(stepPin,LOW); 
     }

  delay(25); // One second delay
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

  esp_camera_fb_return(fb); //Return cam at the end?

  delay(200);

}


void getResponse(WiFiClient client) {
  byte buffer[8] = { NULL };
  while (client.available() > 0 || buffer[0] == NULL) {
    int len = client.available();
    Serial.println("Len" + len);
    if (len > 8) len = 8;
    client.read(buffer, len);
  }
}

//Config Init function:

void config_init() {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;
}