
#define BLYNK_TEMPLATE_ID "TMPL6XcbPjJ8Q"
#define BLYNK_TEMPLATE_NAME "Monitoring Suhu"
#define BLYNK_AUTH_TOKEN "Bp228pNmaSemasJG4Vv6YQyol5H-3bqv"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ===================================
// 1. KONFIGURASI WIFI
// ===================================

const char* ssid = "realme C55";
const char* password = "BakulCubung";

char auth[] = BLYNK_AUTH_TOKEN;

WebServer server(80);

// ===================================
// 2. KONFIGURASI PIN
// ===================================

#define DHT_PIN 5
#define DHT_TYPE DHT22
#define RELAY_PIN 23

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT_TYPE);

// ===================================
// 3. PENGATURAN SUHU
// ===================================

const float SUHU_ON = 37.8;
const float SUHU_OFF = 38.5;

// Relay aktif LOW
#define RELAY_ON LOW
#define RELAY_OFF HIGH

bool lampuNyala = false;

// ===================================
// 4. DATA SENSOR
// ===================================

float suhu = 0;
float kelembapan = 0;

bool sensorError = true;

// ===================================
// 5. PENGATURAN LCD
// ===================================

const unsigned long DURASI_SUHU = 10000;
const unsigned long DURASI_KELEMBAPAN = 3000;

unsigned long waktuGantiLCD = 0;
unsigned long waktuBacaSensor = 0;

bool tampilSuhu = true;

// ===================================
// 6. PENGATURAN WIFI DAN BLYNK
// ===================================

unsigned long waktuCobaWiFi = 0;
unsigned long waktuCobaBlynk = 0;

const unsigned long INTERVAL_WIFI = 10000;
const unsigned long INTERVAL_BLYNK = 10000;

// ===================================
// 7. HALAMAN WEB DASHBOARD
// ===================================

void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Monitoring Inkubator Ayam</title>

<style>
* {
  box-sizing: border-box;
}

body {
  margin: 0;
  padding: 20px;
  font-family: Arial, sans-serif;
  background: #101827;
  color: white;
}

.container {
  max-width: 750px;
  margin: auto;
}

.header {
  text-align: center;
  padding: 20px 0;
}

.header h1 {
  margin-bottom: 8px;
  color: #38bdf8;
}

.header p {
  color: #94a3b8;
}

.grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 15px;
}

.card {
  background: #1e293b;
  border-radius: 16px;
  padding: 22px;
  text-align: center;
}

.card h3 {
  color: #cbd5e1;
  font-size: 16px;
}

.value {
  font-size: 36px;
  font-weight: bold;
  margin-top: 15px;
  color: #38bdf8;
}

.status {
  margin-top: 15px;
  padding: 18px;
  background: #1e293b;
  border-radius: 16px;
  text-align: center;
}

.online {
  color: #4ade80;
}

.offline {
  color: #f87171;
}

.footer {
  text-align: center;
  color: #64748b;
  margin-top: 25px;
  font-size: 12px;
}

@media(max-width: 450px) {
  .grid {
    grid-template-columns: 1fr;
  }
}
</style>
</head>

<body>

<div class="container">

  <div class="header">
    <h1>INKUBATOR AYAM</h1>
    <p>ESP32 Temperature Monitoring</p>
  </div>

  <div class="grid">

    <div class="card">
      <h3>SUHU INKUBATOR</h3>
      <div class="value" id="suhu">-- °C</div>
    </div>

    <div class="card">
      <h3>KELEMBAPAN</h3>
      <div class="value" id="kelembapan">-- %</div>
    </div>

  </div>

  <div class="status">
    <h3>STATUS PEMANAS</h3>
    <div class="value" id="lampu">--</div>
  </div>

  <div class="status">
    <h3>STATUS SENSOR</h3>
    <div id="sensorStatus">Menghubungkan...</div>
  </div>

  <div class="status">
    <h3>KONEKSI</h3>
    <div class="online">ESP32 Web Server Aktif</div>
  </div>

  <div class="footer">
    Monitoring lokal melalui WiFi hotspot
    <br><br>
    Update otomatis setiap 2 detik
  </div>

</div>

<script>

async function updateData() {

  try {

    const response = await fetch('/data');

    if (!response.ok) {
      throw new Error('HTTP error');
    }

    const data = await response.json();

    if (data.error) {

      document.getElementById('suhu').innerText = '-- °C';
      document.getElementById('kelembapan').innerText = '-- %';
      document.getElementById('lampu').innerText = 'OFF';

      document.getElementById('sensorStatus').innerText =
        'Sensor bermasalah - pemanas OFF';

      document.getElementById('sensorStatus').className = 'offline';

    } else {

      document.getElementById('suhu').innerText =
        data.suhu + ' °C';

      document.getElementById('kelembapan').innerText =
        data.kelembapan + ' %';

      document.getElementById('lampu').innerText =
        data.lampu;

      document.getElementById('sensorStatus').innerText =
        'Sensor normal';

      document.getElementById('sensorStatus').className = 'online';

    }

  } catch (error) {

    document.getElementById('sensorStatus').innerText =
      'Koneksi ke ESP32 terputus';

    document.getElementById('sensorStatus').className = 'offline';

  }

}

updateData();
setInterval(updateData, 2000);

</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// ===================================
// 8. API DATA SENSOR
// ===================================

void handleData() {

  String json = "{";

  json += "\"error\":";
  json += sensorError ? "true" : "false";

  json += ",\"suhu\":\"";
  json += sensorError ? "--" : String(suhu, 1);

  json += "\",\"kelembapan\":\"";
  json += sensorError ? "--" : String(kelembapan, 1);

  json += "\",\"lampu\":\"";
  json += lampuNyala ? "ON" : "OFF";

  json += "\"}";

  server.send(200, "application/json", json);
}

// ===================================
// 9. MEMBACA SENSOR DAN KONTROL RELAY
// ===================================

void bacaSensor() {

  float suhuBaru = dht.readTemperature();
  float kelembapanBaru = dht.readHumidity();

  if (isnan(suhuBaru) || isnan(kelembapanBaru)) {

    sensorError = true;
    lampuNyala = false;

    digitalWrite(RELAY_PIN, RELAY_OFF);

    Serial.println("ERROR SENSOR DHT22!");

    // Kirim status error ke Blynk
    if (Blynk.connected()) {
      Blynk.virtualWrite(V2, 0);
      Blynk.virtualWrite(V3, 0);
    }

    return;
  }

  suhu = suhuBaru;
  kelembapan = kelembapanBaru;

  sensorError = false;

  // Kontrol suhu dengan hysteresis
  if (suhu <= SUHU_ON) {

    lampuNyala = true;

  }
  else if (suhu >= SUHU_OFF) {

    lampuNyala = false;

  }

  digitalWrite(
    RELAY_PIN,
    lampuNyala ? RELAY_ON : RELAY_OFF
  );

  Serial.print("Suhu: ");
  Serial.print(suhu);

  Serial.print(" C | Kelembapan: ");
  Serial.print(kelembapan);

  Serial.print(" % | Lampu: ");
  Serial.println(lampuNyala ? "ON" : "OFF");

  // Kirim data ke Blynk
  if (Blynk.connected()) {

    Blynk.virtualWrite(V0, suhu);
    Blynk.virtualWrite(V1, kelembapan);
    Blynk.virtualWrite(V2, lampuNyala ? 1 : 0);
    Blynk.virtualWrite(V3, 1);

  }
}

// ===================================
// 10. MENAMPILKAN LCD
// ===================================

void tampilkanLCD() {

  lcd.clear();

  if (sensorError) {

    lcd.setCursor(0, 0);
    lcd.print("ERROR SENSOR!");

    lcd.setCursor(0, 1);
    lcd.print("Pemanas OFF");

    return;
  }

  if (tampilSuhu) {

    lcd.setCursor(0, 0);
    lcd.print("Suhu: ");
    lcd.print(suhu, 1);
    lcd.write(223);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Lampu: ");
    lcd.print(lampuNyala ? "ON" : "OFF");

  } else {

    lcd.setCursor(0, 0);
    lcd.print("Kelembapan:");

    lcd.setCursor(0, 1);
    lcd.print(kelembapan, 1);
    lcd.print(" %");

    lcd.setCursor(10, 1);
    lcd.print(lampuNyala ? "ON" : "OFF");

  }
}

// ===================================
// 11. SETUP
// ===================================

void setup() {

  Serial.begin(115200);

  // Keamanan awal relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  dht.begin();

  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("INKUBATOR AYAM");

  lcd.setCursor(0, 1);
  lcd.print("Memulai Sistem");

  delay(2000);

  bacaSensor();
  tampilkanLCD();

  waktuBacaSensor = millis();
  waktuGantiLCD = millis();

  // ===================================
  // KONEKSI WIFI
  // ===================================

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Menghubungkan ke hotspot");

  unsigned long mulaiWiFi = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - mulaiWiFi < 20000) {

    delay(500);
    Serial.print(".");

  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi berhasil terhubung!");

    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println("WiFi belum terhubung.");
    Serial.println("Akan mencoba kembali.");

  }

  // ===================================
  // KONFIGURASI BLYNK
  // ===================================

  Blynk.config(auth);

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Menghubungkan ke Blynk...");

    if (Blynk.connect(5000)) {

      Serial.println("Blynk berhasil terhubung!");

    } else {

      Serial.println("Blynk belum terhubung.");

    }

  }

  // ===================================
  // JALANKAN WEB SERVER
  // ===================================

  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();

  Serial.println("Web Server Aktif!");

}

// ===================================
// 12. LOOP
// ===================================

void loop() {

  unsigned long sekarang = millis();

  // Jalankan komunikasi Blynk
  if (WiFi.status() == WL_CONNECTED) {

    Blynk.run();

  }

  // Membaca sensor setiap 2 detik
  if (sekarang - waktuBacaSensor >= 2000) {

    waktuBacaSensor = sekarang;

    bacaSensor();
    tampilkanLCD();

  }

  // Ganti tampilan LCD
  unsigned long durasiTampilan =
    tampilSuhu ? DURASI_SUHU : DURASI_KELEMBAPAN;

  if (sekarang - waktuGantiLCD >= durasiTampilan) {

    waktuGantiLCD = sekarang;

    tampilSuhu = !tampilSuhu;

    tampilkanLCD();

  }

  // Layani permintaan browser
  if (WiFi.status() == WL_CONNECTED) {

    server.handleClient();

  }

  // Coba sambungkan ulang WiFi
  if (WiFi.status() != WL_CONNECTED) {

    if (sekarang - waktuCobaWiFi >= INTERVAL_WIFI) {

      waktuCobaWiFi = sekarang;

      Serial.println("Mencoba menghubungkan WiFi...");

      WiFi.reconnect();

    }

  }

  // Coba sambungkan ulang Blynk
  else if (!Blynk.connected()) {

    if (sekarang - waktuCobaBlynk >= INTERVAL_BLYNK) {

      waktuCobaBlynk = sekarang;

      Serial.println("Mencoba menghubungkan Blynk...");

      Blynk.connect(1000);

    }

  }

}
