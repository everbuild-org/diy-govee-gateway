#include <WiFi.h>
#include "client.h"
#include "esp_task_wdt.h"

NetClient::NetClient(String ssid, String password, String server, String userid)
{
    this->server = server;
    this->userid = userid;

    printf("Connecting to %s..", ssid.c_str());
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    int timeout = 5;
    while (WiFi.status() != WL_CONNECTED)
    {
        printf(".");
        delay(1000);
        timeout--;
        if (timeout == 0)
        {
            printf("[Not connected]\n Error: Net Capabilities will not be enabled!\n");
            enabled = false;
            return;
        }
    }

    printf("[Connected]\n IP: %s\n", WiFi.localIP().toString().c_str());
    client.setInsecure();

    enabled = true;
}

void NetClient::submit(const char *id, float temperature, float humidity, float battery)
{
    if (!enabled)
        return;

    if (!client.connect(this->server.c_str(), 443))
    {
        Serial.println("Connection failed!");
        delay(10000);
        ESP.restart();
    }
    char buf[256] = "";
    sprintf(buf, "{\"id\": \"%s\", \"temperature\": %04f, \"humidity\": %04f, \"battery\": %04f}", id, temperature, humidity, battery);
    int len = strlen(buf);

    client.print("PUT https://");
    client.print(this->server);
    client.println("/api HTTP/1.0");
    client.print("Host: ");
    client.println(this->server);
    client.println("Content-Type: application/json");
    client.print("X-User: ");
    client.println(this->userid);
    client.print("Content-Length: ");
    client.println(len);
    client.println("Connection: close");
    client.println();
    client.println(buf);
    client.println();

    client.stop();
}
