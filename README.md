# Camera Webserver ESP32-CAM for the OV7670 camera module

## Overview

This is an experimental project that plays with **OV7670 camera module** to demonstrate video streaming from 
ESP32 + OV7670 using WebSocket protocol in the local network. This could be used as a prototype for 
a **smart surveillance station** set up outdoor, so my parents can keep an eye on it even though they are in kitchen.

## How it works

The ESP32 creates a little `HTTP` server and then listen to `GET` requests at its own IP address with WiFi-STA mode. 
When the request is triggered, it streams QVGA BMP image from the camera.

<p align="center">
    <img src="./examples/hand.gif" alt="Example with hand" width="400">
</p>

## Why BMP instead of JPG?

This is the big advantage of OV7670 which doesn't support JPG mode. The only way to stream JPG is to capture frames in
YUV422/RGB565 and then convert them by software. After doing some test, each conversion takes about 200-500ms/frame.

Instead, if it is captured in RGB565, and then put a configured BMP-header in front of the framebuffer and 
send the whole thing as bmp image, it would be easier. The trade off is that the image's size is a bit bigger.

## Requirements

- ESP32-WROVER kit
- Non-FIFO OV7670 camera module
- ESP-IDF (v4.2.5)
- [esp32-camera](https://github.com/espressif/esp32-camera) repository as a submodule
- Enable PSRAM and set it to 64MBit. PSRAM configuration should be added directly to sdkconfig once the menuconfig is done.
- Configure your WiFi SSID and Password via idf.py menuconfig

## Todo

- support AP mode.
- support OV2640 camera module for better resolution and quality image.
- support movements detection.