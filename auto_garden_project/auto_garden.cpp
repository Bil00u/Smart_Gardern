#include "auto_garden.h"
#include "credentials.h"
AutoGarden::AutoGarden() :
  display(128, 64, &Wire, -1),
  dht(DHTPIN, DHTTYPE),
  client(espClient)
{
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(PIEZO_BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  dht.begin();

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(std::bind(&AutoGarden::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void AutoGarden::setup_wifi() {
  delay(10);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void AutoGarden::reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client2")) {
      client.subscribe(WATERPUMP_TOPIC);
      client.subscribe(PARTY_MODE_TOPIC);
    } else {
      delay(5000);
    }
  }
}

void AutoGarden::callback(char* topic, byte* payload, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
  }

  if (String(topic) == WATERPUMP_TOPIC) {
    if (messageTemp == "enable") {
      pinMode(RELAY_PIN, OUTPUT);
    } else if (messageTemp == "1") {
      digitalWrite(RELAY_PIN, LOW);
      client.publish(WATERPUMP_STATE_TOPIC, "on");
    } else if (messageTemp == "0") {
      digitalWrite(RELAY_PIN, HIGH);
      client.publish(WATERPUMP_STATE_TOPIC, "off");
    }
  }

  if (String(topic) == PARTY_MODE_TOPIC) {
    if (messageTemp == "party/on") {
      partyMode(true);
    } else if (messageTemp == "party/off") {
      partyMode(false);
    }
  }
}

void AutoGarden::readAndPublishSensorData() {
  // Read temperature and humidity from DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Read soil moisture
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);
  int soilMoisturePercent = map(soilMoistureValue, 2544, 910, 0, 100);

  // Read water level
  int waterLevelValue = analogRead(WATER_LEVEL_PIN);
  int waterLevelPercent = map(waterLevelValue, EMPTY_VALUE, FULL_VALUE, 0, 100);

  // Publish sensor data
    snprintf(msg, 50, "Temperature: %.1f, Humidity: %.1f", t, h);
    client.publish("esp/dht2", msg);
    snprintf(msg, 50, "%.1f", h);
    client.publish("esp/hum", msg);
    snprintf(msg, 50, "%.1f", t);
    client.publish("esp/temp", msg);
  
    snprintf(msg, 50, "Soil Moisture: %d%%", soilMoisturePercent);
    client.publish("esp/soil2", msg);
  
    snprintf(msg, 50, "%d", soilMoisturePercent);
    client.publish("esp/SM/nolabel", msg);
  
    snprintf(msg, 50, "Water Level: %d%%", waterLevelPercent);
    client.publish("esp/water/level", msg);
  
    snprintf(msg, 50, "%d", waterLevelPercent);
    client.publish("esp/WL/nolabel", msg);

    // Update OLED display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.printf("Temperature: %0.1fC\nHumidity: %0.1f%%\n", t, h);
  display.printf("Soil Moisture: %d%%\n", soilMoisturePercent);
  display.printf("Water Level: %d%%\n", waterLevelPercent);
  display.display();

  // Control LEDs based on water level
  if (waterLevelPercent < 20) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else if (waterLevelPercent >= 21 && waterLevelPercent <= 65) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
}


void partyMode(bool enable) {
  if (enable) {
    // Music and LED patterns
    for (int i = 0; i < 7; i++) {
      digitalWrite(RED_LED_PIN, HIGH);
      delay(200);
      ledcWriteTone(BUZZER_CHANNEL, 3000);
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(YELLOW_LED_PIN, HIGH);
      delay(200);
      ledcWriteTone(BUZZER_CHANNEL, 200);
      digitalWrite(YELLOW_LED_PIN, LOW);
      digitalWrite(GREEN_LED_PIN, HIGH);
      delay(200);
      digitalWrite(GREEN_LED_PIN, LOW);
      // Play a tone
      ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY);
      delay(200);
    }
    
  } else {
    // Turn off music and LEDs
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    // Stop the tone
    ledcWriteTone(BUZZER_CHANNEL, 0);
  }
}
