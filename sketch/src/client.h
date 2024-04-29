#pragma once

#include <Arduino.h>
#include <WiFiClientSecure.h>

class NetClient {
    private:
        String server;
        String userid;
        bool enabled;
        WiFiClientSecure client;

    public:
        NetClient(String ssid, String password, String server, String userid);
        void submit(const char *id, float temperature, float humidity, float battery);
};