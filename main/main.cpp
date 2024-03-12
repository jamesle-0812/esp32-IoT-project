#include <stdio.h>
#include <string.h> //for handling strings
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h" //esp_init funtions esp_err_t 
#include "esp_wifi.h" //esp_wifi_init functions and wifi operations
#include "esp_log.h" //for showing logs
#include "esp_event.h" //for wifi event
#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps

#include "wifi.h"
#include "streaming.h"

/* const char *ssid = CONFIG_ESP_WIFI_SSID;
const char *pass = CONFIG_ESP_WIFI_PASSWORD; */

extern "C" void app_main(void);


void app_main(void)
{
    
    Wifi wifiESP32;
    streaming::streaming ov7670;

    // wifiESP32.set_credentials(ssid, pass);
    wifiESP32.init();
    
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    if (ov7670.camera_init() != ESP_OK) {
        ESP_LOGE("CAM", "Camera Init Failed");
        // fflush(stdout);
        // esp_restart();
    }

    ov7670.server_setup();
    
    /* int i = 0;
    while (1) {
        ESP_LOGI("Main", "[%d] Hello world!", i);
        // printf("[%d] Hello world!\n", i);
        i++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } */
}
