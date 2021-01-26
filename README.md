# ESP32 USBHOST TO BLE KEYBOARD
 DIY your ESP32 BLE KEYBOARD with USB HOST SHIELD:</br>
 ![alt text](https://github.com/Cemu0/ESP32_USBHOST_TO_BLE_KEYBOARD/blob/main/IMG_5495.JPG "keyboard mod")
 ## Ferture:
 - Pair with as many Device as u want ... just look the code (but it cost 4-5 seconds to switch (unable to connect ESP32 with multy device))
- High power consumption (>90mah+your keyboard)
## Todo:
- forget ESP32 for ble keyboard and use a nRFxxx instead !!! due to high power consumption problem (maybe slove in the future)
## How to:
- I use [platformio](https://platformio.org/) so just download and open by visual code with platformio extension installed (if not visual code usually ask u to install it).
- not a schematic:</br>
USB Host Mini v2 to ESP32 </br>
GPIO5 : SS </br>
GPIO16 : RESET </br>
GPIO17 : INT</br>
GPIO18 : SCK </br>
GPIO19 : MISO </br>
GPIO23 : MOSI</br>

note: from https://create.arduino.cc/projecthub/139994/plug-any-usb-device-on-an-esp8266-e0ca8a u have to cut the track inorder to use 5v USB devices</br>
- Orther pin is not important:</br>
GPIO2: Statusled</br>
GPIO0: connect it with some key on your keyboard so you can update ESP32 & wake it up from deep-sleep mode
GPIO34: Baterry vol (with 30k & 100k resistor more info [here](https://en.wikipedia.org/wiki/Voltage_divider))
- u should add a swith or PNP transistor for turn Keyboard off (it cost >100 mah LOL)

## Thanks to:
- [T-vK](https://github.com/T-vK) for [ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard) arduino library
- [felis](https://github.com/felis) for [USB_Host_Shield_2.0](https://github.com/felis/USB_Host_Shield_2.0) arduino library

 
