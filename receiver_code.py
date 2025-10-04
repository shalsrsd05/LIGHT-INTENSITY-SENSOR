import paho.mqtt.client as mqtt
import json
import mysql.connector

# MQTT Broker Configuration
MQTT_BROKER = "broker.emqx.io"
MQTT_TOPIC = "esp8266/gy30"

# MySQL Configuration
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="iot_data"
)
cursor = db.cursor()
cursor.execute("""
CREATE TABLE IF NOT EXISTS gy30_data (
  id INT AUTO_INCREMENT PRIMARY KEY,
  lux FLOAT,
  timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
)
""")

def on_message(client, userdata, message):
    data = json.loads(message.payload.decode())
    lux = data["lux"]
    cursor.execute("INSERT INTO gy30_data (lux) VALUES (%s)", (lux,))
    db.commit()
    print(f"Received Light Intensity: {lux} lux")

client = mqtt.Client()
client.on_message = on_message
client.connect(MQTT_BROKER, 1883)
client.subscribe(MQTT_TOPIC)
client.loop_forever()
