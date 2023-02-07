#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_ADDRESS 0x3C
#define OLED_SDA 5
#define OLED_SCL 4

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(OLED_ADDRESS);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Temp: " + String(temperature) + "°C");
  display.println("Humidity: " + String(humidity) + "%");
  display.display();
  delay(2000);
}
