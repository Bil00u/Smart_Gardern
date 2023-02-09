#include <LiquidCrystal.h>
#include <DHT.h>
#include <BluetoothSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

LiquidCrystal lcd(25,26,27,14,12,13);
BluetoothSerial SerialBT;
#define DHTPIN 33 // The pin number connected to the DHT11 sensor
#define DHTTYPE DHT11 // The type of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

const int AirValue = 2575;
const int WaterValue = 859;
const int SensorPin = 36;
int soilMoistureValue = 0;
int soilmoisturepercent=0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin #
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
lcd.begin(16, 2);
lcd.clear();
lcd.print("Temp & Humidity");
lcd.setCursor(0,1);
lcd.print("Readings");

Serial.begin(115200);
SerialBT.begin("Bilal's ESP32"); // Name of the device visible to other devices
dht.begin();

display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
}

void loop() {
float temperature = dht.readTemperature();
float humidity = dht.readHumidity();

String data = "Temp: " + String(temperature) + " C\tHumidity: " + String(humidity) + "%";
SerialBT.println(data);
Serial.println(data);

// display the readings on the LCD screen
lcd.setCursor(0, 0);
lcd.print("Temp: ");
lcd.print(temperature);
lcd.print(" C ");
lcd.setCursor(0, 1);
lcd.print("Humidity: ");
lcd.print(humidity);
lcd.print("%");

soilMoistureValue = analogRead(SensorPin); //put Sensor insert into soil
soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
data = "Soil Moisture: " + String(soilmoisturepercent) + "%";
SerialBT.println(data);
Serial.println(data);

display.clearDisplay();
if(soilmoisturepercent > 100) {
Serial.println("100 %");
display.setCursor(45,0); //oled display
display.setTextSize(2);
display.setTextColor(WHITE);
display.println("Soil");
display.setCursor(20,15);  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Moisture");
  
  display.setCursor(30,40);  //oled display
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.println("100 %");
  display.display();
  
  delay(250);
  display.clearDisplay();
}
else if(soilmoisturepercent <0)
{
  Serial.println("0 %");
  
  display.setCursor(45,0);  //oled display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Soil");
  display.setCursor(20,15);  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Moisture");
  
  display.setCursor(30,40);  //oled display
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.println("0 %");
  display.display();
 
  delay(250);
  display.clearDisplay();
}
else if(soilmoisturepercent >=0 && soilmoisturepercent <= 100)
{
  Serial.print(soilmoisturepercent);
  Serial.println("%");
  
  display.setCursor(45,0);  //oled display
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Soil");
  display.setCursor(20,15);  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println("Moisture");
  
  display.setCursor(30,40);  //oled display
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.println(soilmoisturepercent);
  display.setCursor(70,40);
  display.setTextSize(3);
  display.println(" %");
  display.display();
 
  delay(250);
  display.clearDisplay();
}  
}
