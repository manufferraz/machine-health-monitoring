#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

DHT Sensor(18, DHT11);

void setup(){

  Sensor.begin();
  Serial.begin(115200);

}

void loop(){

  delay(2000);
  
  float Temperatura = Sensor.readTemperature();
  float Umidade = Sensor.readHumidity();

  Serial.print("Temperatura: ");
  Serial.println(Temperatura);
  Serial.print("Umidade: ");
  Serial.println(Umidade);
}

