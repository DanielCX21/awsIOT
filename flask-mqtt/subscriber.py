import ssl
import paho.mqtt.client as mqtt

# Define os caminhos absolutos para os certificados
CA_CERT = "certs/ca.crt"
CLIENT_CERT = "certs/client.crt"
CLIENT_KEY = "certs/client.key"

# Configura o broker e porta (porta 443 com ALPN)
MQTT_BROKER = "a358pitee9rjfx-ats.iot.sa-east-1.amazonaws.com"
MQTT_PORT = 443
MQTT_TOPIC = "a/b/c"

def on_connect(client, userdata, flags, rc):
    print("Connected with result code:", rc)
    client.subscribe(MQTT_TOPIC)
    print(f"Subscribed to topic: {MQTT_TOPIC}")

def on_message(client, userdata, msg):
    print(f"Received message on topic {msg.topic}: {msg.payload.decode()}")

# Cria o cliente MQTT
client = mqtt.Client()

# Configura callbacks
client.on_connect = on_connect
client.on_message = on_message

# Cria contexto TLS com ALPN (obrigatório para AWS IoT na porta 443)
context = ssl.create_default_context()
context.set_alpn_protocols(["x-amzn-mqtt-ca"])
context.load_verify_locations(cafile=CA_CERT)
context.load_cert_chain(certfile=CLIENT_CERT, keyfile=CLIENT_KEY)

# Aplica o contexto TLS
client.tls_set_context(context)

# Conecta e entra no loop
client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
client.loop_forever()
