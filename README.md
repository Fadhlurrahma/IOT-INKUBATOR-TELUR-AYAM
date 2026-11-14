# IOT-INKUBATOR-TELUR-AYAM
# Inkubator Telur Ayam Otomatis Berbasis ESP32

## 1. Deskripsi Proyek

Proyek ini merupakan sistem inkubator telur ayam berbasis ESP32 yang berfungsi untuk memantau suhu dan kelembapan ruang penetasan serta mengendalikan lampu pemanas secara otomatis.

Sistem menggunakan sensor DHT22 untuk membaca kondisi di dalam inkubator. Data ditampilkan melalui LCD 16x2 dan dapat dipantau melalui dashboard web lokal serta aplikasi Blynk.

Pengendalian lampu pemanas dilakukan secara otomatis berdasarkan suhu yang dibaca oleh sensor. Proyek ini tidak menggunakan pemutar telur otomatis.

## 2. Fitur Utama

- Membaca suhu dan kelembapan menggunakan sensor DHT22.
- Menampilkan data suhu dan kelembapan melalui LCD 16x2 I2C.
- Mengendalikan lampu pemanas 220 V AC menggunakan modul relay.
- Menggunakan metode histeresis untuk menjaga kestabilan suhu.
- Menampilkan data melalui dashboard web lokal.
- Mengirim data pemantauan ke aplikasi Blynk.
- Mematikan relay pemanas ketika sensor mengalami kegagalan pembacaan.

## 3. Komponen yang Digunakan

| Komponen | Fungsi |
|---|---|
| ESP32 DevKit | Pusat kendali sistem |
| Sensor DHT22 | Membaca suhu dan kelembapan |
| LCD 16x2 I2C | Menampilkan data sensor |
| Modul relay 5 V | Mengendalikan lampu pemanas |
| Lampu pijar 5 W, 220 V AC | Sumber panas inkubator |
| Breadboard dan kabel jumper | Menghubungkan komponen |

## 4. Konfigurasi Pin

Berikut konfigurasi pin yang digunakan pada rangkaian:

| Komponen | Pin ESP32 |
|---|---|
| DHT22 DATA | GPIO 5 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| Relay IN | GPIO 23 |
| DHT22 VCC | 3V3 |
| LCD I2C VCC | VIN / 5V |
| Relay VCC | VIN / 5V |
| GND seluruh rangkaian | GND |

Alamat I2C LCD yang digunakan adalah `0x27`.

**Catatan:** ESP32 tidak tahan terhadap sinyal logika 5 V pada GPIO. Pastikan jalur SDA dan SCL LCD tidak memberikan tegangan 5 V langsung ke ESP32. Gunakan level shifter atau penyesuaian pull-up 3,3 V jika diperlukan.

## 5. Cara Kerja Sistem

### 5.1 Pembacaan Suhu dan Kelembapan

Sensor DHT22 membaca suhu dan kelembapan di dalam ruang inkubator. ESP32 melakukan pembacaan sensor setiap 2 detik.

### 5.2 Pengendalian Lampu Pemanas

Lampu pemanas dikendalikan menggunakan modul relay dengan metode histeresis. Pengaturan suhu pada program adalah sebagai berikut:

| Kondisi Suhu | Tindakan |
|---|---|
| Suhu ≤ 37,8 °C | Lampu ON |
| Suhu ≥ 38,5 °C | Lampu OFF |
| Suhu di antara 37,8–38,5 °C | Mempertahankan kondisi relay sebelumnya |

Apabila sensor gagal memberikan pembacaan yang valid, sistem akan mengatur relay ke kondisi OFF.

### 5.3 Tampilan LCD

LCD 16x2 menampilkan informasi suhu dan kelembapan secara bergantian:
- Suhu ditampilkan selama sekitar 10 detik.
- Kelembapan ditampilkan selama sekitar 3 detik.

### 5.4 Dashboard Web Lokal

ESP32 menjalankan web server yang memungkinkan pengguna memantau suhu, kelembapan, dan status lampu melalui browser.

Setelah ESP32 terhubung ke jaringan Wi-Fi, alamat IP perangkat dapat dilihat melalui Serial Monitor.

Buka alamat IP ESP32 melalui browser:

```text
http://ALAMAT_IP_ESP32
```

Dashboard web digunakan untuk pemantauan, sedangkan pengendalian lampu pemanas tetap dilakukan secara otomatis oleh ESP32.

### 5.5 Pemantauan Melalui Blynk

Sistem mengirimkan data pemantauan ke aplikasi Blynk menggunakan Virtual Pin berikut:

| Virtual Pin | Data |
|---|---|
| V0 | Suhu |
| V1 | Kelembapan |
| V2 | Status lampu (1 = ON, 0 = OFF) |
| V3 | Status sensor (1 = normal, 0 = gagal) |

Pastikan datastream pada template Blynk menggunakan Virtual Pin yang sesuai dengan konfigurasi program.

## 6. Persiapan dan Konfigurasi

### 6.1 Perangkat Lunak

Perangkat lunak yang digunakan:
- Arduino IDE
- ESP32 Arduino Core

Library yang diperlukan:
- `DHT sensor library` oleh Adafruit
- `LiquidCrystal_I2C`
- `Blynk` oleh Volodymyr Shymanskyy

Library bawaan ESP32:
- `WiFi.h`
- `WiFiClient.h`
- `WebServer.h`
- `Wire.h`

### 6.2 Instalasi Library

Instal library melalui Arduino IDE:

1. Buka menu **Sketch → Include Library → Manage Libraries**.
2. Cari `DHT sensor library`, kemudian instal versi dari Adafruit.
3. Cari dan instal `LiquidCrystal_I2C`.
4. Cari dan instal `Blynk` oleh Volodymyr Shymanskyy.
5. Pastikan paket board ESP32 sudah terpasang.

### 6.3 Konfigurasi Wi-Fi

Sesuaikan nama dan kata sandi jaringan Wi-Fi pada program:

```cpp
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
```

### 6.4 Konfigurasi Blynk

Masukkan kredensial template dan perangkat Blynk milik sendiri:

```cpp
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID_KAMU"
#define BLYNK_TEMPLATE_NAME "Monitoring Suhu"
#define BLYNK_AUTH_TOKEN "AUTH_TOKEN_PERANGKAT_KAMU"
```

Jangan membagikan kata sandi Wi-Fi dan Blynk Auth Token ke publik.

## 7. Langkah Menjalankan Proyek

1. Rakit komponen sesuai konfigurasi pin pada bagian 4.
2. Buka program proyek melalui Arduino IDE.
3. Pastikan seluruh library yang diperlukan sudah terinstal.
4. Sesuaikan konfigurasi Wi-Fi dan Blynk.
5. Pilih board **ESP32 DOIT DevKit V1**.
6. Pilih port COM yang sesuai dengan ESP32.
7. Klik **Verify** untuk memeriksa program.
8. Klik **Upload** untuk mengunggah program ke ESP32.
9. Buka **Tools → Serial Monitor** dan atur baud rate ke `115200`.
10. Pastikan ESP32 berhasil terhubung ke Wi-Fi dan catat alamat IP yang muncul.
11. Buka alamat IP ESP32 melalui browser untuk mengakses dashboard web.
12. Buka aplikasi Blynk untuk melihat data pemantauan.

## 8. Keselamatan

**Perhatian: Proyek ini menggunakan tegangan listrik 220 V AC yang berbahaya dan berisiko menyebabkan sengatan listrik maupun kebakaran.**

- Matikan dan cabut sumber listrik sebelum memasang atau mengubah kabel.
- Pastikan seluruh terminal dan sambungan listrik tertutup serta terisolasi dengan baik.
- Pisahkan jalur listrik 220 V AC dari rangkaian ESP32 dan kabel sensor.
- Jauhkan sambungan listrik dari air dan kelembapan di dalam inkubator.
- Pastikan rating kontak relay sesuai dengan beban lampu pemanas.
- Mintalah orang yang kompeten dalam instalasi listrik untuk memeriksa rangkaian AC sebelum digunakan.
- Lakukan pengujian kestabilan suhu menggunakan termometer pembanding sebelum inkubator digunakan untuk penetasan.

Nilai suhu 37,8 °C dan 38,5 °C merupakan pengaturan pada program dan perlu divalidasi melalui pengujian langsung di dalam inkubator.

---

**Proyek Inkubator Telur Ayam Otomatis Berbasis ESP32**
