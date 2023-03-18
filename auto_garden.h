#ifndef AUTO_GARDEN_H
#define AUTO_GARDEN_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class AutoGarden {
  public:
    void setup();
    void loop();
  
  private:
    void setup_wifi();
    void callback(char* topic, byte* message, unsigned int length);
    void reconnect();
    int mapMoisturePercentage(int rawValue);
};

#endif // AUTO_GARDEN_H
