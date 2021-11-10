# What?
ESP-now to MQTT to Home assisstant garage door opener button

# How
The software is mostly copy-paste from different examples, nothing special.

The hardware is a barebones ESP8266 module with a 3.3V regulator and a pushbutton that connects VCC from my ebikebatterys USB-port. 

<p align="center">
  <img src="https://user-images.githubusercontent.com/5367846/141099940-2914e2a5-bf72-44fd-a9ea-8456ce608ca8.jpg" width="300">
</p>

The ESP (sender) boots up and sends a short ESP-now message. A ESP devmodule (receiver) listens for ESP-now messages and forwards any received messages via serial to a second ESP devmodule (mqtt) that publishes the message via MQTT so Home assistant can react to it and open my garage door.




# Ref links

https://randomnerdtutorials.com/esp-now-esp8266-nodemcu-arduino-ide/

https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/

https://github.com/plerup/espsoftwareserial/

https://github.com/yoursunny/WifiEspNow
