#ifndef AUTO_GARDEN_H
#define AUTO_GARDEN_H

#include <string>

class AutoGarden {
public:
    AutoGarden();
    void setTemperature(float temperature);
    void setHumidity(float humidity);
    void setSoilMoisture(float soilMoisture);
    void setLightIntensity(float lightIntensity);
    void setWateringInterval(int wateringInterval);
    void setFertilizingInterval(int fertilizingInterval);
    void setTemperatureThreshold(float temperatureThreshold);
    void setHumidityThreshold(float humidityThreshold);
    void setSoilMoistureThreshold(float soilMoistureThreshold);
    void setLightIntensityThreshold(float lightIntensityThreshold);
    void run();

private:
    float temperature;
    float humidity;
    float soilMoisture;
    float lightIntensity;
    int wateringInterval;
    int fertilizingInterval;
    float temperatureThreshold;
    float humidityThreshold;
    float soilMoistureThreshold;
    float lightIntensityThreshold;
    std::string status;

    bool isTemperatureWithinThreshold();
    bool isHumidityWithinThreshold();
    bool isSoilMoistureWithinThreshold();
    bool isLightIntensityWithinThreshold();
    void waterPlants();
    void fertilizePlants();
};

#endif
