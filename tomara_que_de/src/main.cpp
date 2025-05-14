#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
 
#define AWS_IOT_PUBLISH_TOPIC   "nitro/pub/portao"
#define AWS_IOT_SUBSCRIBE_TOPIC "nitro/sub"

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

void messageHandler(char* topic, byte* payload, unsigned int lenght);
void connectAWS();
void publishMessage();

void setup()
{
  Serial.begin(115200);
  connectAWS();
}
 
void loop()
{ 
  publishMessage();
  client.loop();
  delay(1000);
}

void messageHandler(char* topic, byte* payload, unsigned int length)
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
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.println(".");
    delay(10);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
  }

 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["primeiro"] = 1;
  doc["segundo"] = 2;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
