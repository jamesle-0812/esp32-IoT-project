idf_component_register(SRCS "wifi.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi 
                             esp_event 
                             esp_netif 
                             lwip
                             nvs_flash
                             )
