#include <Arduino.h>
#include "WiFi.h"
#include "definitions.h"
#include <ESP32Encoder.h>

// Conectividad
WiFiClient wifiClient;

ESP32Encoder encoder;

// put function declarations here:
void connectWifiTask(void *pvParameters);
TaskHandle_t connectWifiTaskHandle = NULL;

void readSpeedTask(void *pvParameters);
TaskHandle_t readSpeedHandle = NULL;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  encoder.attachFullQuad(CH_ENC_A, CH_ENC_B);
  encoder.setCount(0);
  
  xTaskCreatePinnedToCore(connectWifiTask, "Coneccion al wifi", 4096, NULL, 10, &connectWifiTaskHandle, 0);
  xTaskCreatePinnedToCore(readSpeedTask, "Read Speed Task", 4096, NULL, 15, &readSpeedHandle, 0);
}

void loop() {
  // put your main code here, to run repeatedly: 
}

// put function definitions here:
void connectWifiTask(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while(1){
    Serial.println("Wifi task");
    if(WiFi.status() != WL_CONNECTED){
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }else{
      Serial.println("Wifi connected");
    }

    xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5000));
  } 

}

void readSpeedTask(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
  encoder.clearCount();

  while(1){
    float speed = encoder.getCount() * pulses2degrees/0.05;
    encoder.clearCount();
    Serial.print("Velocidad del motor: ");
    Serial.print(speed);
    Serial.println(" deg\\s");
    xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000*0.05));
  }
}