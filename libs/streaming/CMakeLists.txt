idf_component_register(SRCS "streaming.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES esp32-camera
                             esp_http_server
                             )
