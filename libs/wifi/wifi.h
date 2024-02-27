#ifndef __WIFI_H
#define __WIFI_H

#include "esp_wifi.h" //esp_wifi_init functions and wifi operations

void wifi_connection();

class Wifi
{
public:
    Wifi(/* args */);
    ~Wifi();
    
    void init();

private:    
    
};

#endif