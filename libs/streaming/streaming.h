#pragma once

#include "esp_http_server.h"


namespace streaming
{
class streaming
{
private:
    // httpd_uri_t uriGET{};
    // camera_config_t camConfig = {};

public:
    streaming(/* args */);
    ~streaming();

    esp_err_t camera_init(void);
    // esp_err_t stream_jpg_httpd_handler(httpd_req_t *req);
    esp_err_t server_setup(void);
};
}

