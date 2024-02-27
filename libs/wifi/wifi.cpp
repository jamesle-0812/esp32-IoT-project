#include <stdio.h>                  //for basic printf commands
#include <string.h>                 //for handling strings
#include "freertos/FreeRTOS.h"      //for delay,mutexs,semphrs rtos operations
#include "freertos/event_groups.h"
#include "esp_system.h"             //esp_init funtions esp_err_t 
#include "esp_log.h"                //for showing logs
#include "esp_event.h"              //for wifi event
#include "lwip/err.h"               //light weight ip packets error handling
#include "lwip/sys.h"               //system applications for light weight ip apps
#include "nvs_flash.h"              //non volatile storage

#include "wifi.h"

#define WIFI_SUCCESS    1 << 0
#define WIFI_FAILURE    1 << 1

int retry_num = 0;
extern const char *ssid;
extern const char *pass;

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t s_wifi_event_group;

static void wifi_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi CONNECTING...\n");
        esp_wifi_connect();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi CONNECTED\n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection\n");
        if (retry_num < 5) {
            esp_wifi_connect();
            retry_num++;
            printf("Retrying to Connect...\n");
        }
        else
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAILURE);
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("Wifi got IP...\n\n");
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI("WiFi", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_SUCCESS);
        retry_num = 0;
        break;    
    }
}

Wifi::Wifi(/* args */)
{
    // _initCfg = WIFI_INIT_CONFIG_DEFAULT();
    // _state = NOT_INITIALISED;
}

Wifi::~Wifi()
{
}

void Wifi::init() {

    /** INITIALIZE ALL THE THINGS **/    
    nvs_flash_init();

	//initialize the esp network interface
    ESP_ERROR_CHECK(esp_netif_init());

    //initialize default esp event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //create wifi station in the wifi driver
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t _initCfg = WIFI_INIT_CONFIG_DEFAULT();
    //setup wifi station with the default wifi configuration
    ESP_ERROR_CHECK(esp_wifi_init(&_initCfg));

    /** EVENT LOOP CRAZINESS **/
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_handler,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_handler,
                                                        NULL,
                                                        NULL));

    
    wifi_config_t _wifiCfg = {.sta = {}};

    strcpy((char*)_wifiCfg.sta.ssid, ssid);    
    strcpy((char*)_wifiCfg.sta.password, pass);
    
    // _wifiCfg.sta.pmf_cfg.capable = true;
    // _wifiCfg.sta.pmf_cfg.required = false;

    /** START THE WIFI DRIVER **/
    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // set the wifi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &_wifiCfg));

    // start the wifi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WiFi", "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_SUCCESS) or
     * connection failed for the maximum number of re-tries (WIFI_FAILURE), set by event_handler() */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_SUCCESS | WIFI_FAILURE,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, 
     * hence we can test which event actually happened. */
    if (bits & WIFI_SUCCESS) {
        ESP_LOGI("WiFi", "connected to ap");
    }
    else if (bits & WIFI_FAILURE) {
        ESP_LOGI("WiFi", "Failed to connect to ap");
    }
    else {
        ESP_LOGE("WiFi", "UNEXPECTED EVENT");
    }
}
