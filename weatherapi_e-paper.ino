#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold9pt7b.h>


const char* ssid     = "dimaspp";
const char* password = "12345oke";

String apiKey = "4456c20ee40ee55a7e63dfb8da7cafd3";
String city   = "Jakarta,id";   
String url    = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric&lang=id";

// --- e-paper pinout ---
#define EPD_CS   5
#define EPD_DC   16
#define EPD_RST  17
#define EPD_BUSY 4

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY));


const unsigned char icon_sun [] PROGMEM = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0xbc, 0x7e, 0xff, 
	0xff, 0x8c, 0x3c, 0xff, 0xff, 0x84, 0x30, 0xff, 0xff, 0x8f, 0xf0, 0xff, 0xff, 0xb8, 0x1c, 0xff, 
	0xff, 0xe0, 0x07, 0x87, 0xe0, 0xc0, 0x03, 0x0f, 0xf1, 0x80, 0x01, 0x9f, 0xf1, 0x00, 0x00, 0x9f, 
	0xfb, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x7f, 0xe2, 0x00, 0x00, 0x4f, 0xc2, 0x00, 0x00, 0x47, 
	0xc2, 0x00, 0x00, 0x41, 0xf2, 0x00, 0x00, 0x47, 0xfe, 0x00, 0x00, 0x5f, 0xff, 0x00, 0x00, 0xdf, 
	0xf9, 0x00, 0x00, 0x8f, 0xf9, 0x80, 0x01, 0x8f, 0xf0, 0xc0, 0x03, 0x07, 0xe0, 0xe0, 0x07, 0xff, 
	0xff, 0xb8, 0x1d, 0xff, 0xff, 0x0f, 0xf1, 0xff, 0xff, 0x0c, 0x21, 0xff, 0xff, 0x3c, 0x31, 0xff, 
	0xff, 0x7e, 0x3d, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff};
const unsigned char icon_cloud [] PROGMEM = { /* isi bitmap */ };
const unsigned char icon_rain [] PROGMEM = { /* isi bitmap */ };

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung!");

  display.init();
  display.setRotation(1);
  display.setFont(&FreeSansBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  updateWeather();   
}

void loop() {
  updateWeather();   
  delay(600000);     t
}


void updateWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);

      String kondisi = doc["weather"][0]["description"].as<String>();
      String icon    = doc["weather"][0]["icon"].as<String>();
      float suhu     = doc["main"]["temp"].as<float>();
      int kelembapan = doc["main"]["humidity"].as<int>();

      Serial.println("Cuaca: " + kondisi);
      Serial.println("Suhu: " + String(suhu));
      Serial.println("Icon: " + icon);

      display.setFullWindow();
      display.firstPage();
      do {
        display.fillScreen(GxEPD_WHITE);

        display.setCursor(10, 20);
        display.print("Kota: " + city);

        display.setCursor(10, 50);
        display.print("" + String(suhu) + " C");

        display.setCursor(10, 90);
        display.print(kondisi);

        display.setCursor(10, 120);
        display.print("Hum: " + String(kelembapan) + "%");

      
        if (icon.startsWith("01")) {
          display.drawBitmap(100, 30, icon_sun, 32, 32, GxEPD_BLACK);
        } else if (icon.startsWith("02") || icon.startsWith("03") || icon.startsWith("04")) {
          display.drawBitmap(100, 30, icon_cloud, 32, 32, GxEPD_BLACK);
        } else if (icon.startsWith("09") || icon.startsWith("10")) {
          display.drawBitmap(100, 30, icon_rain, 32, 32, GxEPD_BLACK);
        }

      } while (display.nextPage());

    } else {
      Serial.println("HTTP Error");
    }
    http.end();
  }
}