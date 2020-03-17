# ble_service


This is basically a copy from bluez/tools/gatt-service.c modified to call the blinkt_service, that I have modified to use call the blinkt_service dBus API to set colors for the blink LEDS.

I still have some issues to get this working on a Raspberry Pi Zero W, with Raspbian Buster. First you'll need to install bluez and enable bluetooth. The I need to edit the: "/etc/systemd/system/bluetooth.service" and add --experimental as an start command:  

ExecStart=/usr/lib/bluetooth/bluetoothd --experimental

Then for each time you want to restart the gatt-service run the reset_blue.sh

