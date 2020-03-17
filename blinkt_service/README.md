# blinkt_service

This provides a service to front-end the blinkt RGB LED device: 
https://shop.pimoroni.com/products/blinkt, this code is partially based on 
the Pimoroni code: https://github.com/pimoroni/blinkt


If you don't already have the bcm2835 installed follow the instructions at:
https://www.airspayce.com/mikem/bcm2835/


To build:
gcc blinkts_service.c blinkts.c -o blinkt_service `pkg-config --cflags --libs libsystemd` -lbcm2835

To run: Since I am still developing this I just run this as a user service:

> ./blinkt_service &

To test if it works. This should set all LEDs to red:

busctl --user call com.example.Blinkts /com/example/Blinkts com.example.Blinkts SetRed "y" 255

This should clear all colors:
busctl --user call com.example.Blinkts /com/example/Blinkts com.example.Blinkts Clear

This is based on the 
