# ESP32 CAM Labs

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](/LICENSE)
[![Twitter: elbruno](https://img.shields.io/twitter/follow/elbruno.svg?style=social)](https://twitter.com/kartben)
![GitHub: elbruno](https://img.shields.io/github/followers/elbruno?style=social)

This repo contains several demos and labs working with an ESP32 Cam board. I'm currently using a development board that avoid me the wiring and soldering.


![ESP32-CAM-MB WiFi Development Board](img/esp32camdevboard.jpg)



## Visual Studio Code and Platform IO

For all these demos I use the [PlatformIO IDE](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) (VS Code extension) or the [PlatformIO command-line interface](https://platformio.org/install/cli) to deploy the application to your ESP32 Cam Arduino Board. 


# Demos

## Flash Demo

This project turn on and off the flash of the camera. It's a simple demo that you can use to test the LED pin.

![Turn led on and off](img/20220305ESP32CamFlashSmall.gif)

Source code available on [ESPCamFlashDemo folder](./ESPCamFlashDemo).

## WebServer to turn on the Flash with an HTTP Endpoint

This project 
- Connects the ESP32 Cam board to a Wifi network
- Starts a webserver on port 80
- Creates an endpoint named [/flash] to flash the camera
- Turn ON and OFF the Flash on an ESP32-CAM board

![call and http endpoint and turn on the flash](img/httpendpointflashtriggered.jpg)

Source code available on [ESPCamWebServer folder](./ESPCamWebServer).

## WebServer hosting an HTML page to turn on the Flash 

This project 
- Connects the ESP32 Cam board to a Wifi networ
- Starts a webserver on port 80
- Creates an endpoint named [/flash] to flash the camera
- Render a simple html page with a button to trigger the camera
- Turn ON and OFF the Flash on an ESP32-CAM board


![HTML Page with a button to turn on the flash](img/arduinoesp32camrenderingapage.jpg)

Source code available on [ESPCamWebServerPage folder](./ESPCamWebServerPage).

## Take a photo and save it on the SD card

This project 
- Init the camera
- Init the access to the SD card
- Take a photo and save it on the SD card

Source code available on [ESPCamPhotoSDCard folder](./ESPCamPhotoSDCard).

## WebServer to take a photo every 5 seconds

This project 
- Starts a webserver on port 80
- Takes a picture every 5 seconds
- Creates an endpoint named [/photo] to return the last saved photo

Source code available on [ESPCamWebSrvPhotoSavedSPIFFS folder](./ESPCamWebSrvPhotoSavedSPIFFS).


## Additional Resources

In my personal blog "[ElBruno.com](https://elbruno.com)", I wrote about several scenarios on how to work and code with [ESP32 CAM](https://elbruno.com/tag/esp32cam/). 

## Author

👤 **Bruno Capuano**

* Website: https://elbruno.com
* Twitter: [@elbruno](https://twitter.com/elbruno)
* Github: [@elbruno](https://github.com/elbruno)
* LinkedIn: [@elbruno](https://linkedin.com/in/elbruno)

## 🤝 Contributing

Contributions, issues and feature requests are welcome!

Feel free to check [issues page](https://github.com/elbruno/ESP32camDemos/issues).

## Show your support

Give a ⭐️ if this project helped you!


## 📝 License

Copyright &copy; 2021 [Bruno Capuano](https://github.com/elbruno).

This project is [MIT](/LICENSE) licensed.

***