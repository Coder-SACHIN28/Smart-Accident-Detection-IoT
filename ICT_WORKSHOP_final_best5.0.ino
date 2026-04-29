#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>

// --- WiFi Configuration ---
const char* ssid = "YOUR_WIFI_SSID"; 
const char* password = "YOUR_WIFI_PASSWORD";

// --- API KEYS (REMOVED FOR SECURITY) ---
const char* botToken = "YOUR_TELEGRAM_BOT_TOKEN"; 
const char* chatId = "YOUR_TELEGRAM_CHAT_ID"; 
const char* thingSpeakKey = "YOUR_THINGSPEAK_WRITE_KEY"; 
const char* telegramUser = "@YOUR_USERNAME"; // For CallMeBot

// --- Default/Fallback Coordinates ---
const String fallbackLat = "28.416656";
const String fallbackLng = "77.523525";
const String fallbackAddr = "Default Security Office Address";

// --- Hardware Pins ---
#define BUZZER_PIN 15
#define BUTTON_PIN 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
HardwareSerial SerialGPS(2);
WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// --- Logic Variables ---
bool accidentDetected = false;
unsigned long accidentTime = 0;
unsigned long lastLogTime = 0;
double lat = 0.0, lng = 0.0;
float ax, ay, az;
float currentSpeed = 50.0;
float impactSpeed = 0.0;
String humanAddress = "";

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) while(1);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 25); display.print("BOOTING SYSTEM...");
  display.display();

  if (!mpu.begin()) Serial.println("MPU Fail");
  
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
}

void loop() {
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      if (gps.location.isValid()) {
        lat = gps.location.lat();
        lng = gps.location.lng();
      }
    }
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  ax = a.acceleration.x; ay = a.acceleration.y; az = a.acceleration.z;
  float totalA = sqrt(sq(ax) + sq(ay) + sq(az));

  if (!accidentDetected) {
    if (totalA > 11.0) { currentSpeed += 2.5; }
    else {
      if (currentSpeed > 52.0) currentSpeed -= 0.5;
      else if (currentSpeed < 48.0) currentSpeed += 0.5;
    }
    if (currentSpeed > 115.0) currentSpeed = 115.0; 
  }

  if (millis() - lastLogTime > 15000 && !accidentDetected) {
    logToThingSpeak();
    lastLogTime = millis();
  }

  if (totalA > 25.0 && !accidentDetected) {
    impactSpeed = currentSpeed;
    accidentDetected = true;
    accidentTime = millis();
  }

  if (accidentDetected) handleAccident();
  else drawDashboard();
}

void handleAccident() {
  int remaining = 10 - ((millis() - accidentTime) / 1000);
  if (remaining > 0) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      accidentDetected = false;
      noTone(BUZZER_PIN);
      display.clearDisplay(); display.setTextSize(1);
      display.setCursor(20, 30); display.print("ALARM CANCELLED");
      display.display();
      delay(1500);
      return;
    }
    for (int hz = 2500; hz < 3500; hz += 150) { tone(BUZZER_PIN, hz); delay(10); }
    display.clearDisplay();
    display.fillRect(0,0, 128, 14, WHITE);
    display.setTextColor(BLACK); display.setTextSize(1); 
    display.setCursor(25, 3); display.print("CRASH ALERT!");
    display.setTextColor(WHITE); display.setTextSize(4);
    display.setCursor(55, 22); display.print(remaining);
    display.display();
  } else { executeSOS(); }
}

void executeSOS() {
  noTone(BUZZER_PIN);
  currentSpeed = 0; 
  display.clearDisplay(); display.setCursor(15, 30); display.print("SENDING SOS..."); display.display();
  if (lat != 0.0) fetchAddress(); 
  else humanAddress = fallbackAddr;
  sendTelegram();      
  makeVoiceCall();     
  display.clearDisplay(); display.setCursor(25, 30); display.print("SOS COMPLETED!"); display.display();
  delay(3000);
  accidentDetected = false;
}

void fetchAddress() {
  HTTPClient http;
  String url = "http://nominatim.openstreetmap.org/reverse?format=jsonv2&lat=" + String(lat, 6) + "&lon=" + String(lng, 6);
  http.begin(url);
  http.setUserAgent("ESP32-Tracker");
  if (http.GET() == 200) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, http.getString());
    humanAddress = String(doc["display_name"]).substring(0, 45);
  } else { humanAddress = fallbackAddr; }
  http.end();
}

void logToThingSpeak() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://api.thingspeak.com/update?api_key=" + String(thingSpeakKey) + "&field1=" + String(ax) + "&field2=" + String(ay) + "&field3=" + String(az) + "&field4=" + String(currentSpeed);
    http.begin(url); http.GET(); http.end();
  }
}

void sendTelegram() {
  String msg = "🚨 *EMERGENCY: Accident Detected!*\n\n";
  msg += "📍 *Address:* " + humanAddress + "\n";
  msg += "🏎 *Impact Speed:* " + String(impactSpeed, 1) + " km/h\n";
  if (lat != 0.0) msg += "🔗 *Maps:* https://www.google.com/maps?q=" + String(lat, 6) + "," + String(lng, 6);
  else msg += "🔗 *Maps:* https://www.google.com/maps?q=" + fallbackLat + "," + fallbackLng;
  bot.sendMessage(chatId, msg, "Markdown");
}

void makeVoiceCall() {
  HTTPClient http;
  String callMsg = "Emergency! Accident detected. Check Telegram for location.";
  String url = "http://api.callmebot.com/start.php?user=" + String(telegramUser) + "&text=" + callMsg + "&lang=en-GB-Standard-B";
  http.begin(url); http.GET(); http.end();
}

void drawDashboard() {
  display.clearDisplay();
  display.fillRect(0,0, 128, 12, WHITE);
  display.setTextColor(BLACK); display.setCursor(15, 2); display.print("VEHICLE SECURED");
  display.setTextColor(WHITE); display.setCursor(0, 18);
  display.print(lat != 0 ? "GPS: LOCKED" : "GPS: SEARCHING...");
  display.drawRect(0, 35, 128, 28, WHITE);
  display.setCursor(5, 42); display.print("X:"); display.print(ax, 1);
  display.setCursor(45, 42); display.print("Y:"); display.print(ay, 1);
  display.setCursor(85, 42); display.print("Z:"); display.print(az, 1);
  display.setCursor(5, 52); display.print("Speed: "); display.print(currentSpeed, 1); display.print(" km/h");
  display.display();
}