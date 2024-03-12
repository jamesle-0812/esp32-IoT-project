#pragma once

/* OV7670 */

#define ESP32_WROVER_CAM

#ifdef ESP32_WROOM_KIT

#define CAM_PIN_PWDN    -1  // power down is not used
#define CAM_PIN_RESET   -1  // software reset will be performed

#define CAM_PIN_SIOD    21
#define CAM_PIN_SIOC    22

#define CAM_PIN_XCLK    32  // Input clock
#define CAM_PIN_PCLK    34  // Pixel clock for pixel data synchronization

#define CAM_PIN_VSYNC   36  // 34
#define CAM_PIN_HREF    39  // 35

#define CAM_PIN_D7      35  // 4
#define CAM_PIN_D6      33  // 12
#define CAM_PIN_D5      25  // 13
#define CAM_PIN_D4      26  // 14
#define CAM_PIN_D3      27  // 15
#define CAM_PIN_D2      17  // 16
#define CAM_PIN_D1      16  // 17
#define CAM_PIN_D0      4   // 27

#endif

#ifdef ESP32_WROVER_KIT

#define CAM_PIN_PWDN    -1  // power down is not used
#define CAM_PIN_RESET   -1  // software reset will be performed
#define CAM_PIN_XCLK    21
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      36
#define CAM_PIN_D6      39
#define CAM_PIN_D5      34
#define CAM_PIN_D4      35
#define CAM_PIN_D3      19
#define CAM_PIN_D2      18
#define CAM_PIN_D1      5
#define CAM_PIN_D0      4
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

#endif

/* OV2640 */

#ifdef ESP32_WROVER_CAM

#define CAM_PIN_PWDN    -1  //power down is not used
#define CAM_PIN_RESET   -1 //software reset will be performed

#define CAM_PIN_XCLK    21
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27

#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      19
#define CAM_PIN_D2      18
#define CAM_PIN_D1      5
#define CAM_PIN_D0      4

#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

#endif
