# SIWX-917 Demo
Code and instructions on how to run the SIWX-917 positioning demo.

Limitations:
- The device can currently only connect to a pre-defined WiFi.
- Currently sends a request every ~18 seconds.

## Installation
1) Install [Simplicity Studio](https://www.silabs.com/developer-tools/simplicity-studio).
2) Import this project by going to "File"->"Import..." and select the [wifi_mod.sls](wifi_mod.sls) file.

A few variables need to be set in the to allow a WiFi connection and MQTT authentication. Following are the files that require modifications:

### sl_net_default_values.h
Define the following variables in config/sl_net_default_values.h to allow a WiFi connection.
```c
#ifndef DEFAULT_WIFI_CLIENT_PROFILE_SSID
#define DEFAULT_WIFI_CLIENT_PROFILE_SSID "WIFI_NAME"
#endif

#ifndef DEFAULT_WIFI_CLIENT_CREDENTIAL
#define DEFAULT_WIFI_CLIENT_CREDENTIAL "WIFI_PASSWORD"
#endif
```

### app.c
Set the following variables to configure scanning behaviour:
- SCAN_RESULT_BUFFER_SIZE: Maximum MQTT message size (bytes)
- SCAN_INTERVAL_MS: Milliseconds to wait between each scan
- SCAN_ITERATIONS: Number of scans to perform, set as -1 to loop indefinitely

The following variables are required to allow an MQTT connection:
- MQTT_BROKER_IP: Host running MQTT service.
- MQTT_BROKER_PORT: Port of MQTT service.
- SERVICE_TOKEN: Inserted into MQTT topic like `tlc/v2/raw-data/service/<SERVICE_TOKEN>/device/<MAC_ADDRESS>`.
- USERNAME: MQTT username.
- PASSWORD: MQTT password.

## Run
1) Open Simplicity Studio and connect your SIWX-917 device via USB. A device should now appear below the "Debug Adapters" tab.
2) Right-click the device and select "Connect".
3) To view the output of the program we are flashing to the device, right-click the device and press "Launch Console...". Make sure to open the "Serial 1" tab in the console window.
4) Right click the project directory (wifi_mod for example) in Simplicity Studio and select "Build". A directory named "Binaries" should now exist.
5) Right-click the fitting binary in the Binaries directory ("xxx.s37" if unsure) and select "Flash to Device..."->"Program". The output of running the binary on your device should now appear in the console tab.
