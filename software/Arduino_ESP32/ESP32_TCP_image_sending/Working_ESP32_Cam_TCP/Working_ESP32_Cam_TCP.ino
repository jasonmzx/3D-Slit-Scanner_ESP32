/*
 *  This sketch sends a message to a TCP server
 * WORKING
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiMulti.h>

//Pin definitions from header file `pins.h`
#include "pins.h"
#include "camera_handle.cpp"


WiFiMulti WiFiMulti;
camera_config_t config;

void config_init();
//static camera_fb_t get_img();

const char *ssid_Router     = "wifina";  //input your wifi name
const char *password_Router = "";  //input your wifi passwords

void setup()
{
    Serial.begin(115200);
    delay(10);

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
}


void loop()
{ 
//    const uint16_t port = 80;
//    const char * host = "192.168.1.1"; // ip or
    const uint16_t port = 8888;
    const char * host = "10.0.0.88"; // IPv4 internal (of my Laptop)

    Serial.print("Connecting to :");
    Serial.println(host);

    // // Use WiFiClient class to create TCP connections
     WiFiClient client;
    int TCP_Connect = client.connect(host,port);
    delay(20);

    if (!TCP_Connect) {
        Serial.println("Connection failed.");
        Serial.println("Waiting 0.5 seconds before retrying...");
        delay(500);
        return;
    }

    Serial.println("Connected to server ! ( OK )");

    // This will send a request to the server
    //uncomment this line to send an arbitrary string to the server
    // for(int i = 0; i < 10; i++){
    //   client.print("Send this data to the server ");
    // }
    
    
    //camera_fb_t x = get_img();
    camera_fb_t *fb = NULL;
    fb = esp_camera_fb_get();

    if(!fb) {
      Serial.println("Camera capture failed...");
      return;
    } else {
      Serial.println("Camera Captured ( OK )");
    }

    const char *data = (const char*)fb->buf; // ??? Lol
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
  esp_camera_fb_return(fb);

  Serial.println("bottom of loop...");
  // client.stop();

  delay(1000);
}

void getResponse(WiFiClient client) {
  byte buffer[8] = { };
  while (client.available() > 0 || buffer[0] == NULL) {
    int len = client.available();
    Serial.println("Len" + len);
    if (len > 8) len = 8;
    client.read(buffer, len);
  }
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