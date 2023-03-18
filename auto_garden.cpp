#include "auto_garden.h"
#include "credentials.h"

// Definitions and variable declarations ...

void AutoGarden::setup() {
  Serial.begin(115200);
  // OLED screens 1 and 2 setup
  I2C_One.begin(OLED1_SDA, OLED1_SCL);
  I2C_Two.begin(OLED2_SDA, OLED2_SCL);

  display1.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display2.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  pinMode(piezoPin, OUTPUT); // buzer set to output
  dht.begin();
  analogReadResolution(12); // Set the resolution to 12 bits for ESP32

  display1.clearDisplay();
  display1.setTextSize(1);
  display1.setTextColor(WHITE);

  display2.clearDisplay();
  display2.setTextSize(1);
  display2.setTextColor(WHITE);
  
  pinMode(pump_relay_pin, OUTPUT); //relay pin module set to output
  digitalWrite(pump_relay_pin, HIGH);
    setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void AutoGarden::loop() {
  // if client is connected to broker
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
// reading sensor values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soilMoistureRaw = analogRead(SOIL_MOISTURE_PIN);
  int soilMoisturePercentage = mapMoisturePercentage(soilMoistureRaw);
// displaying on the screens
  display1.clearDisplay();
  display1.setCursor(0, 0);
  display1.print("Temperature: ");
  display1.print(temperature);
  display1.print(" C");
  display1.setCursor(0, 20);
  display1.print("Humidity: ");
  display1.print(humidity);
  display1.print(" %");
  display1.display();

  display2.clearDisplay();
  display2.setCursor(0, 0);
  display2.print("Soil Moisture: ");
  display2.print(soilMoisturePercentage);
  display2.print(" %");
  display2.display();

  // Create JSON object with sensor readings
  DynamicJsonDocument doc(256);
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["soil_moisture"] = soilMoisturePercentage;

  // Serialize JSON object to a string
  String sensor_values_json;
  serializeJson(doc, sensor_values_json);

  // Publish the JSON string to the MQTT topic
  client.publish(sensor_values_topic, sensor_values_json.c_str());


  delay(2000); // Update sensor values, display, and MQTT every 2 seconds
}

void AutoGarden::setup_wifi() {
    delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void AutoGarden::callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message received on topic ");
  Serial.print(topic);
  Serial.print(": ");
  String messageStr;
  for (int i = 0; i < length; i++) {
    messageStr += (char)message[i];
  }
  Serial.print(messageStr);
  Serial.println();
  if (strcmp(topic, pump_topic) == 0) {
    if (strcmp(messageStr.c_str(), "enable") == 0) {
      pump_enabled = true;
      client.publish(pump_status_topic, "Pump enabled");
    }
    else if (strcmp(messageStr.c_str(), "1") == 0 && pump_enabled) {
      pump_state = 1;
      digitalWrite(pump_relay_pin, LOW);
      client.publish(pump_status_topic, "Pump turned ON");
    }
    else if (strcmp(messageStr.c_str(), "0") == 0 && pump_enabled) {
      pump_state = 0;
      digitalWrite(pump_relay_pin, HIGH);
      client.publish(pump_status_topic, "Pump turned OFF");
    }
    else {
      client.publish(pump_status_topic, "Invalid message received.");
    }
  }
//  else if (strcmp(topic, "party/mode") == 0 && messageStr == "party") {
//    melody_playing();
//  }
}

void AutoGarden::reconnect() {
   while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("waterpump/status", "ESP32 connected");
      // ... and resubscribe
      client.subscribe(pump_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int AutoGarden::mapMoisturePercentage(int rawValue) {
  int mapMoisturePercentage(int rawValue) {
  int moisturePercentage = map(rawValue, AirValue, WaterValue, 0, 100);
  moisturePercentage = constrain(moisturePercentage, 0, 100);
  return moisturePercentage;
}
