#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

#define AWS_IOT_PUBLISH_TOPIC   "nitro/pub/portao"
#define AWS_IOT_SUBSCRIBE_TOPIC "nitro/sub"

//declaro net e um client herdando de net
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

//declaro as funções
void messageHandler(char* topic, byte* payload, unsigned int lenght);
void connectAWS();
void publishMessage();

void setup()
{
    Serial.println("\nIniciando ESP32 AWS IoT...");
    Serial.begin(115200);
    connectAWS();
}

void loop()
{
    publishMessage();
    client.loop();
    delay(1000);
}

void messageHandler(char* topic, byte* payload, unsigned int lenght)
{
    Serial.print("incoming: ");
    Serial.println(topic);

    StaticJsonDocument<200> doc;
    deserializeJson(doc, payload);
    const char* message = doc["message"];
    Serial.println(message); 
}

void connectAWS()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.println("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    }

    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    client.setServer(AWS_IOT_ENDPOINT, 8883);

    client.setCallback(messageHandler);

    while (!client.connect(THINGNAME))
    {
        Serial.println(".");
        delay(10);
    }

    if(!client.connected())
    {
        Serial.println("AWS IoT Timeout!");
    }

 
    // Subscribe to a topic
    client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
    Serial.println("AWS IoT Connected!");
}