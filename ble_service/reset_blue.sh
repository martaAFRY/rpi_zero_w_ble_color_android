sudo systemctl restart bluetooth;
sudo btmgmt -i hci0 power off;
sudo btmgmt -i hci0 le on;
sudo btmgmt -i hci0 connectable on;
sudo btmgmt -i hci0 bredr off;
sudo btmgmt -i hci0 advertising on;
sudo btmgmt -i hci0 power on;
