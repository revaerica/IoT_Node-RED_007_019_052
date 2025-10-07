# IoT_Node-RED_007_019_052

#### Nama Anggota
| No. | Nama                                    | NRP         | 
|-----|-----------------------------------------|-------------|
| 1   | Revalina Erica Permatasari              | 5027241007  | 
| 2   | Syifa Nurul Alfiah                      | 5027241019  | 
| 3   | Salsa Bil Ulla                          | 5027241052  | 

## Langkah-Langkah Pengerjaan Project ESP32 di Node-RED, Mosquitto, dan Arduino IDE
1. Set up Arduino IDE untuk melakukan instalasi PubSubClient (MQTT communication) dan DHT sensor library (DHT).

   Tools> Manage Libraries
   <img width="1919" height="708" alt="image" src="https://github.com/user-attachments/assets/612bace2-158b-462f-83a8-df9956d17dbf" />

2. Install Node-RED di terminal menggunakan command `npm install -g --unsafe-perm node-red` > run Node-RED menggunakan command `node-red` > buka Node-RED di browser `http://127.0.0.1:1880/`

   <img width="1919" height="1242" alt="image" src="https://github.com/user-attachments/assets/b5081e27-2d6f-4f99-b0b1-27ecd3378453" />

3. 

![WhatsApp Image 2025-10-07 at 12 24 33 (1)](https://github.com/user-attachments/assets/32e74cd4-f317-4fc8-8900-733eaca6bed3)

![WhatsApp Image 2025-10-07 at 12 24 33](https://github.com/user-attachments/assets/9657feb8-6877-4ad4-a376-478a35d89213)

## Penjelasan Kode

ESP32 membaca data suhu dan kelembapan dari sensor **DHT11**, kemudian mengirimkan data tersebut ke **broker Mosquitto** menggunakan protokol **MQTT**.
Selanjutnya, **Node-RED** berfungsi sebagai *subscriber* untuk menampilkan data dalam bentuk output **Debug**.

---

### Kode Program ESP32

```
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
```

---

### Penjelasan Kode

#### Library & Konstanta

* `WiFi.h` → Menghubungkan ESP32 ke jaringan WiFi
* `PubSubClient.h` → Mengatur komunikasi MQTT dengan broker
* `DHT.h` → Membaca data suhu dan kelembapan dari sensor DHT11

Konfigurasi jaringan:

```
const char* ssid = "Nijiro";
const char* password = "Murakami";
const char* mqtt_server = "10.200.123.34";
```

---

#### setup_wifi()

Menghubungkan ESP32 ke jaringan WiFi dan menampilkan alamat IP:

```
setup_wifi();
```

**Output di Serial Monitor:**

```
Menghubungkan ke Nijiro
.........
WiFi tersambung!
IP Address ESP32: 192.168.x.x
```

---

#### reconnect()

Menghubungkan ESP32 ke **broker Mosquitto**.
Jika koneksi gagal, ESP32 akan mencoba lagi setiap 5 detik.

```
client.connect("ESP32Client");
```

---

#### loop()

* Membaca data suhu & kelembapan dari DHT11
* Mengonversi data ke string
* Mengirimkan data ke broker melalui MQTT

Topic yang dikirim:

* `/sensor/suhu`
* `/sensor/kelembapan`

**Output di Serial Monitor:**

```
Suhu: 27.50 °C | Kelembapan: 65.20 %
```

---

### Konfigurasi Node-RED

#### Flow:

```
[mqtt in (topic: /sensor/suhu)] → [debug]
[mqtt in (topic: /sensor/kelembapan)] → [debug]
```

#### Konfigurasi Node “MQTT in”:

| Properti | Nilai                                    |
| -------- | ---------------------------------------- |
| Server   | `10.200.123.34` (IP broker Mosquitto)    |
| Port     | `1883`                                   |
| Topic    | `/sensor/suhu` atau `/sensor/kelembapan` |

Setelah klik **Deploy**, buka tab **Debug (kanan atas)**.
Jika berhasil, Node-RED akan menampilkan pesan seperti berikut:

```
msg.payload : string
"27.50"
```

---

### Ringkasan Sistem

| Komponen      | Fungsi                                     | Output                             |
| ------------- | ------------------------------------------ | ---------------------------------- |
| **ESP32**     | Membaca suhu & kelembapan, kirim ke broker | Data MQTT                          |
| **DHT11**     | Sensor suhu dan kelembapan                 | Nilai suhu & kelembapan            |
| **Mosquitto** | Broker MQTT yang menyalurkan data          | Menyimpan & mendistribusikan pesan |
| **Node-RED**  | Subscriber yang menerima data dari broker  | Menampilkan data di tab Debug      |

---

### Alur Komunikasi

1. **ESP32** membaca data dari **DHT11**
2. Data dikirim via WiFi ke **Mosquitto Broker**
3. **Node-RED** subscribe ke topic yang sama (`/sensor/suhu`, `/sensor/kelembapan`)
4. Data muncul di tab **Debug** Node-RED

---

### Contoh Output

```
WiFi tersambung!
IP Address ESP32: 192.168.0.115
Tersambung ke broker!
Suhu: 27.36 °C | Kelembapan: 64.80 %
```

**Node-RED Debug:**

```
msg.payload : string
"27.36"
```
