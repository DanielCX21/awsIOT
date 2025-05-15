#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "secrets.h"

#define LED1 13
#define LED2 14

// Prototipagem das funções
void connectToWiFi();
void connectToAWS();
void mqttCallback(char *topic, byte *payload, unsigned int length);

// Configuração do cliente seguro e MQTT
WiFiClientSecure net;
PubSubClient client(net);

void setup()
{
  Serial.begin(115200);
  Serial.println("\nIniciando ESP32 AWS IoT...");

  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);

  // Conectar ao WiFi
  connectToWiFi();

  // Configura os certificados SSL/TLS
  net.setCACert(ca_cert);
  net.setCertificate(client_cert);
  net.setPrivateKey(client_key);

  // Configura o cliente MQTT
  client.setServer(awsEndpoint, 8883);
  client.setCallback(mqttCallback);

  // Conecta ao AWS IoT
  connectToAWS();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi desconectado! Reconectando...");
    connectToWiFi();
  }

  if (!client.connected())
  {
    Serial.println("MQTT desconectado! Reconectando...");
    connectToAWS();
  }

  client.loop();
  delay(100);
}

void connectToWiFi()
{
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if (++attempts > 20)
    {
      Serial.println("\nFalha na conexão WiFi! Reiniciando...");
      ESP.restart();
    }
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void connectToAWS()
{
  Serial.println("Conectando ao AWS IoT...");

  while (!client.connected())
  {
    if (client.connect("esp32_controler"))
    {
      Serial.println("Conectado ao AWS IoT!");
      client.subscribe("relay/pin13");
      client.subscribe("relay/pin14");
    }
    else
    {
      Serial.print("Falha na conexão, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String message;
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem: ");
  Serial.println(message);

  if(message == "turn_on" && strcmp(topic, "relay/pin13") == 0)
  {
    digitalWrite(LED1,HIGH);
  }
  else if(message == "turn_off" && strcmp(topic, "relay/pin13") == 0)
  {
    digitalWrite(LED1,LOW);
  }

  if(message == "turn_on" && strcmp(topic, "relay/pin14") == 0)
  {
    digitalWrite(LED2,HIGH);
  }
  else if(message == "turn_off" && strcmp(topic, "relay/pin14") == 0)
  {
    digitalWrite(LED2,LOW);
  }
}
