# SIWX-917 Demo
Code and instructions on how to run the SIWX-917 positioning demo.


## Installation
1) Install [Simplicity Studio](https://www.silabs.com/developer-tools/simplicity-studio).
2) Import this project by going to "File"->"Import..." and select the wifi_mod.sls file.

#### sl_net_default_values.h
Define the following variables in config/sl_net_default_values.h to allow a WiFi connection.
```c
#ifndef DEFAULT_WIFI_CLIENT_PROFILE_SSID
#define DEFAULT_WIFI_CLIENT_PROFILE_SSID "WIFI_NAME"
#endif

#ifndef DEFAULT_WIFI_CLIENT_CREDENTIAL
#define DEFAULT_WIFI_CLIENT_CREDENTIAL "WIFI_PASSWORD"
#endif
```

#### app.c
Set the following variables to allow an MQTT connection:
- MQTT_BROKER_IP
- MQTT_BROKER_PORT
- SERVICE_TOKEN
- USERNAME
- PASSWORD

## Run
Open Simplicity Studio and connect your SIWX-917 device via USB. A device should now appear below the "Debug Adapters" tab. Right-click it and select "Connect".
To view the output of the program we are flashing to the device, right-click the device and press "Launch Console...". Make sure to open the "Serial 1" tab.
Right click the project directory in Simplicity Studio and select "Build". A directory named "Binaries" should now appear. Right-click the fitting binary (use "wifi_mod.s37" if unsure) and press "Flash to Device..."->"Program". The output should now appear in the console tab.
