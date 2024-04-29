# DIY Govee gateway

![Discord](https://img.shields.io/discord/1209253136112549918) ![GitHub Org's stars](https://img.shields.io/github/stars/everbuild-org?style=flat)

This project takes sensor data from a [Govee Bluetooth Hygrometer Thermometer H5075](https://eu.govee.com/collections/smart-sensors/products/govee-bluetooth-hygrometer-thermometer-h5075) and pushes it into Grafana. This is done using prometheus as a datastore and a simple Kotlin app to accept requests from a ESP32 near the sensor.

## Project structure
- `sketch` contains the C++ code to send sensor data from the esp32 to the kotlin server
- `prints` contains the .stl file for our case for the [ESP32 Himalaya Matrix Core](https://eckstein-shop.de/HIMALAYAMatrix-CoreESP32EntwicklerboardmitESP32-BitWiFi2BBluetoothIoTDEVBoardEN)
- `server` contains the kotlin backend to transform the dataset into a prometheus application
- `deployment` contains an example deployment in our kubernetes cluster

## Requirements
> If you want to use this project without setting it up yourself or getting a working ESP32, please contact us at [https://everbuild.org/contact](https://everbuild.org/contact) or join our [discord server](https://discord.everbuild.org/). We'll be happy to work with you to get you set up in our cluster.

- ESP32 - We use the [ESP32 Himalaya Matrix Core](https://eckstein-shop.de/HIMALAYAMatrix-CoreESP32EntwicklerboardmitESP32-BitWiFi2BBluetoothIoTDEVBoardEN), but any ESP32 should work. You might need to adjust the [platformio.ini](sketch/platformio.ini) file to match your board.
- Govee Bluetooth Hygrometer Thermometer H5075
- A Kubernetes cluster with Grafana and Prometheus Operator installed
- An SSL Reverse proxy in front of the kotlin server

## Setup
1. Flash the ESP32 with the [sketch](sketch) code
2. Deploy the [server](server) to your kubernetes cluster
3. Configure your ESP32 via the serial console to point to your server (Baudrate: 115200)
4. Add a new datasource in Grafana to point to your prometheus instance
5. Build yourself a dashboard in Grafana
6. (Optional) Treat yourself to a nice ESP32 case by printing the [prints](prints) file


## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing
We'd love to have you contribute to this project. Just open a pull request and we'll review it as soon as possible. For major changes, please open an issue first to discuss what you would like to change.

If you have any questions, please contact us at [https://everbuild.org/contact](https://everbuild.org/contact) or join our [discord server](https://discord.everbuild.org/).

## Authors
This project was created by [Everbuild](https://everbuild.org) in 2024. 
Maintainers are:
- [Christian](https://github.com/Bloeckchengrafik)
- [Nico](https://github.com/TheNick24)

Please contact us at [https://everbuild.org/contact](https://everbuild.org/contact) or join our [discord server](https://discord.everbuild.org/) if you have any questions or need help with this project.


## Acknowledgments
We used [@tsaitsai](https://github.com/tsaitsai)'s [govee_bluetooth_gateway](https://github.com/tsaitsai/govee_bluetooth_gateway/tree/main) as a base for our C++ code. Thank you for your work!

