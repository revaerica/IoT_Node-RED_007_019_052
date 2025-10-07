#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

const char* ssid = "Nijiro";
const char* password = "Murakami";
const char* mqtt_server = "10.200.123.34"; 

#define DHTPIN 3
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi tersambung!");
  Serial.print("IP Address ESP32: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Tersambung ke broker!");
    } else {
      Serial.print("Gagal (rc=");
      Serial.print(client.state());
      Serial.println("), coba lagi 5 detik...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal baca data DHT!");
    delay(2000);
    return;
  }

  char tempStr[8];
  char humStr[8];
  dtostrf(t, 1, 2, tempStr);
  dtostrf(h, 1, 2, humStr);

  client.publish("/sensor/suhu", tempStr);
  client.publish("/sensor/kelembapan", humStr);

  Serial.print("Suhu: ");
  Serial.print(t);
  Serial.print(" °C | Kelembapan: ");
  Serial.print(h);
  Serial.println(" %");

  delay(5000);
}
