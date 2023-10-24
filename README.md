*Work in progress*

# Temperature and humidity sensor smart thing

Simple arduino sketch that reads data from an AHT20 sensor, connects to a WiFi
network and sends the readings to a remote MQTT server.

The device will enter deep sleep mode after running and wake up every 5 minutes forever.

BOM:
- NodeMCU - ESP8266-12F controller
- AHT20 sensor (temp + humidity)


## Notes
- Waking up from deep sleep will only work if pins D0 and RST are wired together

## Todo
- [ ] Use ESP-01 board instead
- [ ] Use battery power
