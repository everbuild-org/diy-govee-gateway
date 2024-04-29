#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Preferences.h>
#include "easyKey.h"
#include "client.h"

struct RawSensorPayload_t
{
    uint32_t sensorReadingsWrongEndianesss;
    uint8_t batteryPercent;
};

struct SensorPayload_t
{
    float temperature;
    float humidity;
    uint8_t battery;
};

struct NVS_t
{
    String ssid;
    String passwd;
    String server;
    String userid;
};

int scanTime = 5; // In seconds
BLEScan *pBLEScan;
NetClient *client;
TaskHandle_t handle = NULL;
Preferences preferences;
NVS_t nvs;

uint32_t changeEndianness32(uint32_t val)
{
    return (val << 24) |
           ((val << 8) & 0x00ff0000) |
           ((val >> 8) & 0x0000ff00) |
           ((val >> 24) & 0x000000ff);
}

SensorPayload_t buildPayload(RawSensorPayload_t raw)
{
    SensorPayload_t payload;
    payload.battery = raw.batteryPercent;
    uint32_t sensorReadings = changeEndianness32(raw.sensorReadingsWrongEndianesss);

    payload.humidity = ((float)(sensorReadings % 1000)) / 10;

    uint8_t negativity = 1;
    if (sensorReadings & 0x800000)
    {
        negativity = -1;
        sensorReadings = sensorReadings ^ 0x800000;
    }

    payload.temperature = negativity * ((float)sensorReadings) / 10000.0;

    return payload;
}

bool isRelevant(const char *data, int len)
{
    return len == 8 && data[0] == 0x88 && data[1] == 0xec;
}

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice device)
    {
        if (!device.haveManufacturerData())
            return;
        std::string stringData = device.getManufacturerData();
        const char *data = stringData.data();
        int len = stringData.length();
        if (!isRelevant(data, len))
            return;
        RawSensorPayload_t raw;
        memcpy(&raw, data + 2, sizeof(RawSensorPayload_t));
        SensorPayload_t payload = buildPayload(raw);

        printf("temp: %.2f, humid: %.2f, battery: %d\n", payload.temperature, payload.humidity, payload.battery);
        client->submit(device.getAddress().toString().c_str(), payload.temperature, payload.humidity, payload.battery);
    }
};

void initNVS()
{
    if (!preferences.isKey("nvsVersion"))
    {
        preferences.putString("ssid", "");
        preferences.putString("password", "");
        preferences.putString("server", "");
        preferences.putString("user", "");
        preferences.putInt("nvsVersion", 1);
    }

    nvs.ssid = preferences.getString("ssid");
    nvs.passwd = preferences.getString("password");
    nvs.server = preferences.getString("server");
    nvs.userid = preferences.getString("user");
}

void saveNVS()
{
    preferences.putString("ssid", nvs.ssid);
    preferences.putString("password", nvs.passwd);
    preferences.putString("server", nvs.server);
    preferences.putString("user", nvs.userid);
}

String readString(String name, bool hidden = false) {
    char buf[256];
    enterString(name.c_str(), buf, 256, hidden);
    return String(buf); 
}

void changeSSID() {
    nvs.ssid = readString("SSID> ");
}

void changePass() {
    nvs.passwd = readString("Password> ", true);
}

void changeServer() {
    nvs.server = readString("Server> ");
}

void changeUserId() {
    nvs.userid = readString("User ID> ");
}

void enterSetup()
{
    Serial.read();
    int8_t choice = -1;
    while (choice != 0)
    {
        Menu *menu = new Menu();
        menu->start("Configure DIY Govee Gateway", 1);
        menu->add("WiFi SSID", nvs.ssid.c_str(), 1);
        menu->add("WiFi Password", "***", 2);
        menu->add("Server adress", nvs.server.c_str(), 3);
        menu->add("User ID", nvs.userid.c_str(), 4);

        choice = menu->userChoice();

        switch (choice)
        {
        case 1:
            changeSSID();
            break;
        case 2:
            changePass();
            break;
        case 3:
            changeServer();
            break;
        case 4:
            changeUserId();
            break;
        }
    }

    saveNVS();
    ESP.restart();
}

void setup()
{
    Serial.begin(115200);
    preferences.begin("govee", false);
    initNVS();

    printf("Press Enter to enter setup... (could take up to 10 seconds)\n");

    if (nvs.ssid.length() > 0 && nvs.passwd.length() > 0 && nvs.server.length() > 0 && nvs.userid.length() > 0) {
        client = new NetClient(nvs.ssid, nvs.passwd, nvs.server, nvs.userid);
    } else {
        printf("Warning: could not connect to server due to misconfiguration\n");
    }

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void loop()
{
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    pBLEScan->clearResults();
    vTaskDelay(10000);

    if (Serial.available())
        enterSetup();
}