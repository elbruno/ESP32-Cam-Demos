//    Copyright (c) 2022
//    Author      : Bruno Capuano
//    Create Time : 2022 March
//    Change Log  :
//    - Starts a webserver on port 80
//    - Takes a picture every 5 seconds
//    - Creates an endpoint named [/photo] to return the last saved photo
//
//    The MIT License (MIT)
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in
//    all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//    THE SOFTWARE.

#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"          // Disable brownour problems
#include "soc/rtc_cntl_reg.h" // Disable brownour problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>

// Replace with your network credentials
const char *ssid = "IoTLabs";
const char *password = "12345678";

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define FLASH_GPIO_NUM 4

// ledPin refers to ESP32-CAM GPIO 4 (flashlight)
#define FLASH_GPIO_NUM 4

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

boolean takeNewPhoto = false;
boolean workInProgress = false;
boolean flashEnabled = false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Check if photo capture was successful
bool checkPhoto(fs::FS &fs)
{
  File f_pic = fs.open(FILE_PHOTO);
  unsigned int pic_sz = f_pic.size();
  return (pic_sz > 100);
}

// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs(void)
{
  if (workInProgress == false)
  {
    // Take a photo with the camera
    Serial.println("start work in progress");

    workInProgress = true;
    camera_fb_t *fb = NULL; // pointer
    bool ok = 0;            // Boolean indicating if the picture has been taken correctly

    do
    {
      // Take a photo with the camera
      Serial.println("Taking a photo...");

      fb = esp_camera_fb_get();
      if (!fb)
      {
        Serial.println("Camera capture failed");
        workInProgress = false;
        return;
      }

      Serial.println("Camea Capture OK");

      // Photo file name
      Serial.printf("Picture file name: %s\n", FILE_PHOTO);
      File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

      // Insert the data in the photo file
      if (!file)
      {
        Serial.println("Failed to open file in writing mode");
      }
      else
      {
        file.write(fb->buf, fb->len); // payload (image), payload length
        Serial.print("The picture has been saved in ");
        Serial.print(FILE_PHOTO);
        Serial.print(" - Size: ");
        Serial.print(file.size());
        Serial.println(" bytes");
      }
      // Close the file
      file.close();
      esp_camera_fb_return(fb);

      // check if file has been correctly saved in SPIFFS
      ok = checkPhoto(SPIFFS);
    } while (!ok);

    Serial.println("wip back to false");
    workInProgress = false;
  }
}

void initFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else
  {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
}

void initCamera()
{
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // lower quality for better performance
  config.jpeg_quality = 30;

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
}

void flashOnForNSeconds(int seconds)
{
  digitalWrite(FLASH_GPIO_NUM, HIGH);
  delay(seconds * 1000);
  digitalWrite(FLASH_GPIO_NUM, LOW);
}

void initAndConnectWifi()
{
  // Connect to Wi-Fi
  int startWifiTime = millis();
  int connectAttemps = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    int secondsWaiting = (millis() - startWifiTime) / 1000;
    Serial.println("Connecting to WiFi..");
    Serial.print("Seconds waiting : ");
    Serial.println(secondsWaiting);
    delay(1000);
    connectAttemps++;
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(9600);

  // initialize digital pin ledPin as an output
  pinMode(FLASH_GPIO_NUM, OUTPUT);

  // connect to wifi
  initAndConnectWifi();
  flashOnForNSeconds(1);

  // init fs and camera
  initFS();
  initCamera();
  flashOnForNSeconds(1);
 
  // init and start server
  server.on("/photo", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, FILE_PHOTO, "image/jpg", false); });
  server.begin();
  flashOnForNSeconds(1);
}

void loop()
{
  capturePhotoSaveSpiffs();
  delay(5000);
}