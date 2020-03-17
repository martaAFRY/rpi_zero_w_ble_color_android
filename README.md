# rpi_zero_w_ble_color_android

This git provides and Android app: **BleBlinktColor** 
A gatt service to run on a Raspberry Pi: **ble_service**
A service to changes the LEDS from a Raspberry Pi: **blinkt_service**

The Android app has been built with Android Studio 3.6.1 on Ubuntu 19.10
The gatt and blinkt services are built and running on a Raspberry Pi Zero W, with Raspbian Buster.

When the Android app has managed to connect to a Bluetooth device with the name "raspberryp" the field below the sliders on the app should go from green to red and after that, the colors the you set with the RGB-sliders should be transmitted to you BLE capable rPi device. If every thing is working fine your LEDs should change color. 
