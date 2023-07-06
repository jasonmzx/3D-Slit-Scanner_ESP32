/*
 *  This sketch sends a message to a TCP server
 * WORKING
 */
 
//For Joystick:
#include <ezButton.h>

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiMulti.h>

//Pin definitions from header file `pins.h`
#include "pins.h"
#include "camera_handle.cpp"

//Threading Task
TaskHandle_t Task1;

WiFiMulti WiFiMulti;
camera_config_t config;

void config_init();
void captureImage(WiFiClient client);

//static camera_fb_t get_img();

const char *ssid_Router     = "wifina";  //input your wifi name
const char *password_Router = "1326Gabi@";  //input your wifi passwords

//? NEMA 17 Stepper Motor Definitions:
const int stepPin = 32; 
const int dirPin = 33; 

// //? Small Button Pin
// const int buttonPin = 15;

//? Line Lazer Pin
const int lazerPin = 2;



void setup()
{
    Serial.begin(115200);
    delay(10);

  config_init(); //This function sets the `camera_config_t config` global var to the pins needed (defined in `pins.h`)

    Serial.println(xPortGetCoreID());
    //Activating Pins for Stepper Motor:
    pinMode(stepPin,OUTPUT); 
    pinMode(dirPin,OUTPUT);
    pinMode(lazerPin, OUTPUT);

    // pinMode(buttonPin, INPUT);s

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, 0);        //1-Upside down, 0-No operation
  s->set_hmirror(s, 0);      //1-Reverse left and right, 0-No operation
  s->set_brightness(s, 1);   //up the brightness just a bit
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

      //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0

}



int machineState = 0;
int tick = 0;
float angle = 0;

void loop() {
  
  delay(15);

  if (tick > 100 && machineState == 0) {
    machineState = 1;
    tick = 0;
  }
  
  if(machineState == 1){
   delay(1000);

    const uint16_t port = 8888;
    const char * host = "10.0.0.88"; // IPv4 internal (of my Laptop)

    Serial.print("Connecting to :");
    Serial.println(host);

    digitalWrite(lazerPin, HIGH);
    delay(20);
    // Use WiFiClient class to create TCP connections
    WiFiClient client;


  //! Capture with Lazer
    bool lazerCapture = captureImage(client,port,host);

    while(!lazerCapture){
      delay(500);
      Serial.println("Re-trying Camera Capture (With Lazer)");
      lazerCapture = captureImage(client,port,host);
    }

    delay(500);    

  //! Capture without Lazer

    digitalWrite(lazerPin, LOW);
    bool refCapture = captureImage(client,port,host);

    while(!refCapture){
      delay(500);
      Serial.println("Re-trying Camera Capture (With Lazer)");
      refCapture = captureImage(client,port,host);
    }

  //motor rotation
    
    if(angle < 360) {
     int microDelay = 4250;

      // 90 DEG = ~2475 optimal ?



      for(int x = 0; x < 308; x++) {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(microDelay); 
        digitalWrite(stepPin,LOW); 
      }

      
      Serial.println("current angle:");
      Serial.println(angle);
      Serial.println("\n");
    }

    bool ackStatus = sendAcknowledgement(client, port, host, angle);

    while(!ackStatus){
      delay(500);
      Serial.println("Re-trying Acknowledgement");
      ackStatus = sendAcknowledgement(client, port, host, angle);
    }

     angle += 11.25; //Update Angle after Sending ack
    machineState = 0; 

  }
  tick++;
    
}

bool sendAcknowledgement(WiFiClient client, u_int16_t port, const char* host, float angle) {
    int TCP_Connect = client.connect(host, port);
    if (!TCP_Connect) {
        Serial.println("[ACK] Connection failed, No TCP Stream Available");
        delay(500);
        return false;
    }

    char angleStr[10]; // increased the size to accommodate the float and precision
    snprintf(angleStr, sizeof(angleStr), "%.4f", angle); // formatted print to get 4 decimal places

    // Replace the dot with a dash
    for (int i = 0; angleStr[i]; i++) {
        if (angleStr[i] == '.')
            angleStr[i] = '-';
    }

    client.write(angleStr);
    return true;
    }

bool captureImage(WiFiClient client, u_int16_t port, const char* host) {


  Serial.println("Running captureImage() ");
    int TCP_Connect = client.connect(host,port);
    delay(20);

    if (!TCP_Connect) {
        Serial.println("Connection failed, No TCP Stream Available");
        delay(500);
        return false;
    }

    Serial.println("Connected to server ! ( OK )");

    //camera_fb_t x = get_img();
    camera_fb_t *fb = NULL;

//! Band-aid Fix; Getting, Returning & Getting ESP Camera again for end of repeat fb issue 

    fb = esp_camera_fb_get();
    
    xTaskCreatePinnedToCore(
                    ESP_Return_on_Thread,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    fb,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */  
    
    fb = esp_camera_fb_get();
  
//! ENDOF Band-aid fix

    if(!fb) {
      Serial.println("Camera capture failed...");
      return false;
    } else {
      Serial.println("Camera Captured ( OK )");
    }

    const char *data = (const char*)fb->buf; //? Casts dereference of data to an array of characters 
                                            //? from the fb's buf attribute

// Image metadata.  Yes it should be cleaned up to use printf if the function is available
  Serial.print("Size of image:");
  Serial.println(fb->len);
  Serial.print("Shape->width:");
  Serial.print(fb->width);
  Serial.print("height:");
  Serial.println(fb->height);

  //client.write((char*)&(fb->width),sizeof(fb->width)); //Character Buffer for Width (bytes)
  //client.write((char*)&(fb->height),sizeof(fb->height)); //Character Buffer for Height (bytes)
  
  // Give the server a chance to receive the information before sending an acknowledgement.
  delay(1000);
  //getResponse(client);
  Serial.print(data);
  client.write(data, fb->len);

  //esp_camera_fb_return(fb);
    xTaskCreatePinnedToCore(
                    ESP_Return_on_Thread,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    fb,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */  



  delay(100);
  return true;
}


void ESP_Return_on_Thread( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  camera_fb_t* fb = (camera_fb_t*)pvParameters;

  esp_camera_fb_return(fb);
  vTaskDelete(NULL);
}

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