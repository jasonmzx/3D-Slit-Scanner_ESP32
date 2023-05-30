#include "esp_timer.h"
#include "esp_camera.h"


static camera_fb_t get_img() {
    
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;

    //Enable LED ~150 ms before call to esp_camera_fb_get()
    // enable_led(true);
    // vTaskDelay(150 / portTICK_PERIOD_MS);
    
    fb = esp_camera_fb_get();

    // enable_led(false); //turn off LED after pic is taken

    // if(!fb) {
    //    return NULL;
    // }
    return *fb;

}