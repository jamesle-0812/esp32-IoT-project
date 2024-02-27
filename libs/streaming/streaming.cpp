#include <functional>
#include "esp_log.h"
#include "esp_camera.h"
#include "esp_timer.h"

#include "camera_pins.h"
#include "streaming.h"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/bmp\r\nContent-Length: %u\r\n\r\n";

#define CONFIG_XCLK_FREQ 10000000

namespace streaming
{
streaming::streaming(/* args */)
{
}

streaming::~streaming()
{
}

esp_err_t streaming::camera_init(void)
{
    camera_config_t camConfig = {
        .pin_pwdn  = CAM_PIN_PWDN,          // the power-down pin
        .pin_reset = CAM_PIN_RESET,         // the reset pin
        .pin_xclk = CAM_PIN_XCLK,           // the clock pin
        .pin_sccb_sda = CAM_PIN_SIOD,       // the SCCB (I2C-like) data
        .pin_sccb_scl = CAM_PIN_SIOC,       // the SCCB (I2C-like) clock

        .pin_d7 = CAM_PIN_D7,               // the data pins
        .pin_d6 = CAM_PIN_D6,               // the data pins
        .pin_d5 = CAM_PIN_D5,               // the data pins
        .pin_d4 = CAM_PIN_D4,               // the data pins
        .pin_d3 = CAM_PIN_D3,               // the data pins
        .pin_d2 = CAM_PIN_D2,               // the data pins
        .pin_d1 = CAM_PIN_D1,               // the data pins
        .pin_d0 = CAM_PIN_D0,               // the data pins
        .pin_vsync = CAM_PIN_VSYNC,         // the vertical sync
        .pin_href = CAM_PIN_HREF,           // the horizontal reference
        .pin_pclk = CAM_PIN_PCLK,           // the pixel clock pins

        .xclk_freq_hz = CONFIG_XCLK_FREQ,   // freq of the clock signal, in Hz
        // .ledc_timer = LEDC_TIMER_0,         // the timer for LED PWM controller
        // .ledc_channel = LEDC_CHANNEL_0,     // the channel numbers for LED PWM controller

        .pixel_format = PIXFORMAT_RGB565,     // Output format: YUV422,GRAYSCALE,RGB565.  
        .frame_size = FRAMESIZE_QVGA,

        // .jpeg_quality = 12,                 // 0-63, for OV series camera sensors, lower number means higher quality
        .fb_count = 2,                      // number of frame buffers to use for capturing, if > 1, i2s runs in continuous mode.
        // .fb_location = CAMERA_FB_IN_DRAM,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY // specifies that the camera should fill its buffers when they are empty
    };

    return esp_camera_init(&camConfig);
}

esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t* fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _img_buf_len;
    uint8_t* _img_buf;
    char* part_buf[64];
    static int64_t last_frame = 0;
    
    int64_t frameConverted = 0;
    int64_t frameSent = 0;

    if (!last_frame) {
        last_frame = esp_timer_get_time();
    }

    // setting the content type of the HTTP response
    ESP_ERROR_CHECK(httpd_resp_set_type(req, _STREAM_CONTENT_TYPE));

    while (true) {
        // get a frame captured from the camera
        fb = esp_camera_fb_get();        
        if (!fb) {
            ESP_LOGE("STREAM", "Camera capture failed");
            res = ESP_FAIL;
            break;
        }

        // format it to BMP type
        bool bmp_converted = frame2bmp(fb, &_img_buf, &_img_buf_len);
        if (!bmp_converted) {// if the conversion fails
            ESP_LOGE("STREAM", "Conversion failed");
            esp_camera_fb_return(fb);
            res = ESP_FAIL;
        }

        frameConverted = esp_timer_get_time();

        // sends the boundary string
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        
        // send the part header (includes the content type and content length)
        if (res == ESP_OK) {
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _img_buf_len);

            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }

        frameSent = esp_timer_get_time();
        
        // send the captured image
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)_img_buf, _img_buf_len);
        }
        
        if (fb->format != PIXFORMAT_JPEG) {
            free(_img_buf);
        }
        
        // returns the frame buffer back to the camera to be reused
        esp_camera_fb_return(fb);
        
        if (res != ESP_OK) {
            break;
        }
        
        // logs the size and processing time of the captured image
        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        frame_time /= 1000;

        frameConverted -= last_frame;
        frameConverted /= 1000;

        frameSent = fr_end - frameSent;
        frameSent /= 1000;

        last_frame = fr_end;
        ESP_LOGI("STREAM", "MBMP: %uKB, Converted time %ums, Frame sending time %ums (%.1ffps)",
            (uint32_t)(_img_buf_len/1024), (uint32_t)(frameConverted),
            (uint32_t)frameSent, 1000.0 / (uint32_t)frame_time);
    }

    last_frame = 0;

    return res;
}

/**  
 *  @brief  sets up an HTTP server that listens for GET requests
 */
esp_err_t streaming::server_setup(void)
{
    esp_err_t err = ESP_OK;
    httpd_handle_t streamHTTPD  = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t uriGET = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL
    };
    
    // uriGET.handler = std::bind(&streaming::stream_handler, this, std::placeholders::_1);

    err = httpd_start(&streamHTTPD , &config);
    if (err == ESP_OK)
    {
        // registers the uriGet structure as a handler for GET requests
        err = httpd_register_uri_handler(streamHTTPD , &uriGET);
    }

    return err;
}

}
