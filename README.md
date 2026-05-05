# 📡 TA2CAY Taktik Radar Sistemi

[![ESP32](https://img.shields.io/badge/Board-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![C++](https://img.shields.io/badge/Language-C++-00599C.svg)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**TA2CAY Taktik Radar Sistemi**, ESP32 mikrodenetleyicisi gücünü kullanarak etrafındaki nesneleri ultrasonik sensör ile tarayan, mesafeyi ölçen ve aynı zamanda ortam sıcaklığı ile nemini takip eden gelişmiş, gerçek zamanlı bir radar projesidir. Sistem, kendine ait bir Wi-Fi ağı oluşturarak tamamen bağımsız çalışır ve modern, taktiksel bir web arayüzü sunar.

---

## ✨ Özellikler

- **Modern Web Arayüzü (Tactical HUD):** WebSockets kullanılarak gecikmesiz, gerçek zamanlı radar taraması. Cyberpunk/Taktiksel tasarım dili.
- **Kendi Wi-Fi Ağı & Captive Portal:** Dış internete ihtiyaç duymaz. `ta2cay` ağına bağlandığınızda arayüz cihazınızda otomatik olarak açılır.
- **Çoklu Sensör Entegrasyonu:** Engel tespiti için HC-SR04, ortam verileri (Sıcaklık ve Nem) için DHT11.
- **OLED Ekran Desteği:** Radar taraması ve sistem istatistikleri doğrudan cihaz üzerindeki I2C OLED ekrandan da anlık olarak izlenebilir.
- **Sesli ve Görsel Uyarılar:** 50 cm altındaki yakın engellerde buzzer ile sesli "ping" uyarısı ve LED bildirimleri. Web arayüzünde Türkçe sesli uyarı desteği.

---

## 🛠️ Kullanılan Donanımlar

Projeyi hayata geçirmek için aşağıdaki bileşenlere ihtiyacınız vardır:

| Bileşen | Açıklama | Pin Bağlantısı |
| :--- | :--- | :--- |
| **ESP32** | Ana mikrodenetleyici | - |
| **HC-SR04** | Ultrasonik Mesafe Sensörü | Trig: `27`, Echo: `26` |
| **Servo Motor** | Radar dönüşünü sağlayan motor (Örn: SG90) | Sinyal: `18` |
| **DHT11 / DHT22** | Sıcaklık ve Nem Sensörü | Data: `25` |
| **1.3" OLED Ekran** | I2C Haberleşmeli (SH1106 vb.) | SDA / SCL (Varsayılan) |
| **Buzzer** | Sesli Uyarı Sistemi | Sinyal: `14` |
| **LED** | Görsel Uyarı | Sinyal: `13` |

---

## 🚀 Kurulum ve Yükleme

1. **Gereksinimler:** Arduino IDE'sini kurun ve ESP32 board eklentisini yükleyin.
2. **Kütüphaneler:** Aşağıdaki kütüphaneleri Library Manager üzerinden indirin:
   - `ArduinoJson`
   - `DHT sensor library` (Adafruit)
   - `ESP32Servo`
   - `U8g2`
   - `WebSockets`
3. Proje klasörünü bilgisayarınıza indirin (`git clone https://github.com/ta2cay/ta2cay-radar-projesi.git`).
4. `radar.ino` dosyasını Arduino IDE ile açıp ESP32 kartınıza yükleyin.

## 📱 Kullanım

1. ESP32'ye güç verdiğinizde cihaz kısa bir kalibrasyon turu yapacak ve kendi ağını oluşturacaktır.
2. Telefon, tablet veya bilgisayarınızdan Wi-Fi ağlarına girip **`ta2cay`** isimli ağa bağlanın.
   - **Şifre:** `radar1234`
3. Bağlantı sonrası arayüz otomatik olarak açılacaktır (Captive Portal). Eğer açılmazsa tarayıcınıza **`http://192.168.4.1`** yazarak taktik radar paneline erişebilirsiniz.
4. Sol üstten "Sesi Aktifleştir" butonuna tıklayarak tespit bildirimlerini sesli alabilirsiniz.

---

*TA2CAY tarafından Amatör Radyo ve Elektronik meraklıları için geliştirilmiştir. 73!*
