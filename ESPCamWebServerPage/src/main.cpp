//    Copyright (c) 2022
//    Author      : Bruno Capuano
//    Create Time : 2022 March
//    Change Log  :
//    - Starts a webserver on port 80
//    - Creates an endpoint named [/flash] to flash the camera
//    - Render a simple html page with a button to trigger the camera
//    - Turn ON and OFF the Flash on an ESP32-CAM board
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
const char *ssid = "IoTLabs2";
const char *password = "12345678";

// ledPin refers to ESP32-CAM GPIO 4 (flashlight)
#define FLASH_GPIO_NUM 4

void flashOnForNSeconds(int seconds)
{
  digitalWrite(FLASH_GPIO_NUM, HIGH);
  delay(seconds * 1000);
  digitalWrite(FLASH_GPIO_NUM, LOW);
}

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { text-align:center; }
    .vert { margin-bottom: 10%; }
    .hori{ margin-bottom: 0%; }
  </style>
</head>
<body>
  <div id="container">
    <h2>ESP32 CAM - Labs</h2>
    <p>
      <button onclick="triggerFlash()">Trigger Flash</button>      
    </p>
  </div>
  <h4>Bruno Capuano - @elbruno</h4> 

</body>
<script>
  var deg = 0;
  function triggerFlash() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/flash", true);
    xhr.send();
  }
</script>
</html>)rawliteral";

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

  initAndConnectWifi();

  flashOnForNSeconds(1);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  // Route for trigger flash
  server.on("/flash", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              flashOnForNSeconds(1);
              request->send_P(200, "text/plain", "Flash Triggered"); });

  // Start server
  server.begin();
}

void loop()
{
  delay(10);
}