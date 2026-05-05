#include <ArduinoJson.h>
#include <DHT.h>
#include <DNSServer.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>
#include <Wire.h>

#define SERVO_PIN 18
#define TRIG_PIN 27
#define ECHO_PIN 26
#define PIN_LED 13
#define PIN_BUZZER 14
#define PIN_DHT 25
#define DHTTYPE DHT11

DHT dht(PIN_DHT, DHTTYPE);
bool systemReady = false;

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
Servo servo;

const char *AP_SSID = "ta2cay";
const char *AP_PASSWORD = "radar1234";
const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer httpServer(80);
WebSocketsServer webSocket(81);

float temperature = 0.0, humidity = 0.0;
unsigned long lastDHTRead = 0;
const unsigned long DHT_INTERVAL = 5000;

const int CENTER_X = 64, CENTER_Y = 63, MAX_RADIUS = 45, RINGS = 4;

#define MAX_OBSTACLES 20
struct Obstacle {
  int angle;
  long distance;
  unsigned long timestamp;
  bool active;
};
Obstacle obstacleHistory[MAX_OBSTACLES];

long closestDistance = 999;
int totalDetections = 0;
long totalDistance = 0;
unsigned long scanCount = 0;
unsigned long lastBlink = 0;
bool blinkState = false;

#define FADE_LENGTH 8
int fadeAngles[FADE_LENGTH];
int fadeIndex = 0;

// --- Fonksiyon prototipleri ---
void drawStaticRadar();
void updateRadarDisplay(int angle, long distance);
void addObstacle(int angle, long distance);
void updateFadeTrail(int currentAngle);
void drawFadeEffect();
void drawObstacleHistory();
char *getDangerSymbol(long distance);
long getDistance();
void updateTemp();
void handleLedBuzzer(long dist);

// --- WebSocket veri gönder ---
void sendRadarData(int angle, long distance, int direction) {
  StaticJsonDocument<200> doc;
  doc["angle"] = angle;
  doc["distance"] = (distance >= 999) ? 0 : (int)distance;
  doc["temp"] = (int)temperature;
  doc["hum"] = (int)humidity;
  doc["detections"] = totalDetections;
  doc["scans"] = (int)(scanCount / 36);
  doc["dir"] = direction;
  doc["closest"] = (closestDistance >= 999) ? 0 : (int)closestDistance;
  char json[200];
  serializeJson(doc, json);
  webSocket.broadcastTXT(json);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                    size_t length) {
  if (type == WStype_CONNECTED)
    Serial.printf("[WS] Tablet baglandi #%u\n", num);
  if (type == WStype_DISCONNECTED)
    Serial.printf("[WS] Tablet ayrildi  #%u\n", num);
}

extern const char WEB_HTML[];

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  for (int i = 0; i < FADE_LENGTH; i++)
    fadeAngles[i] = -1;

  dht.begin();
  Wire.begin();
  display.begin();

  // WiFi AP (Kararli Baglanti Icin Statik IP)
  IPAddress local_ip(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // DNS Sunucusu: Android/iOS'un internet baglantisi sorgularini kendi
  // sunucumuza kandirarak yolla
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // HTTP
  httpServer.on("/", []() { httpServer.send_P(200, "text/html", WEB_HTML); });
  httpServer.onNotFound([]() {
    httpServer.sendHeader("Location", String("http://192.168.4.1/"), true);
    httpServer.send(302, "text/plain", "");
  });
  httpServer.begin();

  // WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // --- Acilis animasyonu (Yenilendi) ---
  for (int r = 0; r <= 30; r += 3) {
    display.clearBuffer();
    display.drawCircle(64, 32, r);
    if (r > 10)
      display.drawCircle(64, 32, r - 10);
    display.sendBuffer();
    delay(30);
  }
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB10_tr);
  int w1 = display.getStrWidth("TA2CAY");
  display.drawStr(64 - (w1 / 2), 28, "TA2CAY");
  display.setFont(u8g2_font_6x10_tr);
  int w2 = display.getStrWidth("RADAR");
  display.drawStr(64 - (w2 / 2), 44, "RADAR");
  display.drawRFrame(64 - (w1 / 2) - 8, 12, w1 + 16, 38, 4);
  display.sendBuffer();
  delay(1500);

  // WiFi bilgisi OLED'de goster
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(0, 12, "WiFi Agi:");
  display.drawStr(0, 26, "ta2cay");
  display.drawStr(0, 40, "Sifre: radar1234");
  display.drawStr(0, 54, "Tarayici: 192.168.4.1");
  display.sendBuffer();
  delay(4000);

  // Ilerleme
  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(8, 20, "RADAR YUKLENIYOR...");
  display.drawRFrame(14, 28, 100, 12, 2);
  display.sendBuffer();
  for (int p = 0; p <= 96; p += 4) {
    display.drawBox(16, 30, p, 8);
    display.sendBuffer();
    httpServer.handleClient();
    webSocket.loop();
    delay(40);
  }
  delay(300);

  // Sonar animasyonu
  for (int wave = 0; wave <= 30; wave += 3) {
    display.clearBuffer();
    display.setFont(u8g2_font_6x10_tr);
    display.drawStr(16, 12, "TARAMA BASLIYOR!");
    display.drawDisc(64, 42, 2);
    if (wave >= 3)
      display.drawCircle(64, 42, wave);
    if (wave >= 6)
      display.drawCircle(64, 42, wave - 6);
    if (wave >= 9)
      display.drawCircle(64, 42, wave - 12);
    display.sendBuffer();
    httpServer.handleClient();
    webSocket.loop();
    delay(50);
  }

  // Servo kalibrasyon - Güçlü başlatma sekansı
  servo.attach(SERVO_PIN, 500, 2400); 
  delay(500);
  
  // Statik sürtünmeyi kırmak için tam tur hareket
  servo.write(0);
  delay(800);
  servo.write(180);
  delay(800);
  servo.write(90);
  delay(500);

  systemReady = true;

  display.clearBuffer();
  drawStaticRadar();
  display.sendBuffer();
}

void loop() {
  dnsServer.processNextRequest();
  httpServer.handleClient();
  webSocket.loop();

  if (millis() - lastDHTRead >= DHT_INTERVAL) {
    updateTemp();
    lastDHTRead = millis();
  }

  closestDistance = 999;

  for (int angle = 0; angle <= 180; angle += 5) {
    httpServer.handleClient();
    webSocket.loop();
    servo.write(angle);
    delay(150);
    long dist = getDistance();
    updateRadarDisplay(angle, dist);
    sendRadarData(angle, dist, 1);
    handleLedBuzzer(dist);
  }

  for (int angle = 180; angle >= 0; angle -= 5) {
    httpServer.handleClient();
    webSocket.loop();
    servo.write(angle);
    delay(150);
    long dist = getDistance();
    updateRadarDisplay(angle, dist);
    sendRadarData(angle, dist, -1);
    handleLedBuzzer(dist);
  }
}

void handleLedBuzzer(long dist) {
  if (dist > 0 && dist < 50) {
    digitalWrite(PIN_LED, HIGH);
    if (systemReady) {
      for (int i = 0; i < 10; i++) {
        digitalWrite(PIN_BUZZER, HIGH);
        delayMicroseconds(400);
        digitalWrite(PIN_BUZZER, LOW);
        delayMicroseconds(400);
      }
    }
  } else {
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_BUZZER, LOW);
  }
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 30000);
  if (d == 0)
    return 999;
  long dist = (d / 2) / 29.1;
  return (dist < 0 || dist > 200) ? 999 : dist;
}

void updateTemp() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t)) {
    humidity = h;
    temperature = t;
  }
}
