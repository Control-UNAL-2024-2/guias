#include <Arduino.h>
#include "definitions.h"
#include <PubSubClient.h>
#include "mqtt_utils.h"
#include <ArduinoJson.h>
#include "WiFi.h"
#include "FreeRTOSConfig.h"
#include "topics.h"
#include "connection_settings.h"

// Conectivity
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Buffers to send through mqtt
// This buffers are used to send many data at once, for preventing saturation of the comunication channel
// For each variable you want to send, you must create a buffer for it
float kpBuffer[BUFFER_SIZE];
float kiBuffer[BUFFER_SIZE];
float kdBuffer[BUFFER_SIZE];


// Funtions
void connectWiFi();
void connectMqtt();
void initMqtt();
void mqttCallback(char* topic, byte* message, unsigned int length);
void publishVars(uint indexFrame, uint currFrame);

// Tasks
void handleConnectionsTask(void *pvParameters);
void randomTask(void *pvParameters);
static void publishStateTask (void *pvParameters);


// Handlers
TaskHandle_t h_publishStateTask;

// mqtt helpers
uint32_t np = 0;
uint32_t total_time = 4294967295;


/*------------------------------------------------------------------------------------------------------------------

                                                      Constants

------------------------------------------------------------------------------------------------------------------*/
float kp = 0;
float ki = 0;
float kd = 0;




void setup() {
  setCpuFrequencyMhz(160);

  initMqtt();

  xTaskCreatePinnedToCore(randomTask,"Random task",8192,NULL,3,&h_publishStateTask,CORE_CONTROL);
  xTaskCreatePinnedToCore(publishStateTask,"User command IOT",8192,NULL,3,&h_publishStateTask,CORE_COMM);
  xTaskCreatePinnedToCore(handleConnectionsTask,"Handle connections",4096,nullptr,3,nullptr,CORE_COMM);
}

void loop() {
}



void randomTask(void *pvParameters){
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    // Random stuff
    



    // At the end of the code, send te notification for MQTT
    xTaskNotify(h_publishStateTask, 0b0001, eSetBits);
    np +=1;
    xTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5)); 
  }
}





/*------------------------------------------------------------------------------------------------------------------

                                                  Connectivity

------------------------------------------------------------------------------------------------------------------*/


// Wifi connection
void connectWiFi(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  printf("Plant %s is connecting to %s network , please wait\n", PLANT_NUMBER, WIFI_SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      vTaskDelay(pdMS_TO_TICKS(250));
  }
  printf("\n");
  printf("Connected to WIFI through IP: %s \n", WiFi.localIP().toString());
}

// Initialization for MQTT
void initMqtt() {
  mqttClient.setServer(BROKER, 1883);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setBufferSize (4096);
}

// Connection to MQTT
void connectMqtt(){
  printf("Starting MQTT connection...");
  if (mqttClient.connect(THINGNAME, "", ""))
  {
      mqttClient.subscribe(USER_SYS_PID); 
      printf("now connected to broker %s !\n", BROKER);
  }
  else
  {
      printf("Failed MQTT connection code %d \n try again in 3 seconds\n", mqttClient.state());
  }
}

// Function to keeop trying to connect to wifi and mqtt
void handleConnectionsTask(void *pvParameters) {
  for (;;) {
      if (WiFi.status() != WL_CONNECTED) {
          connectWiFi();
      }
      if (!mqttClient.connected()) {
          vTaskDelay(2000);
          connectMqtt();
      }
      mqttClient.loop();
      vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// Publish vars trought MQTT
void publishVars(uint indexFrame, uint currFrame) {
  JsonDocument doc;
  char jsonBuffer[BUFFER_SIZE * 40];
  
  // Pass variables values to JSON
  doc["kp"] = arrayToString(kpBuffer, indexFrame + 1);
  doc["ki"] = arrayToString(kiBuffer, indexFrame + 1);
  doc["kd"] = arrayToString(kdBuffer, indexFrame + 1);

  serializeJson(doc, jsonBuffer); // print to client
  mqttClient.publish(SYS_USER_VARS, jsonBuffer);
}

// Publish vars trought MQTT
static void publishStateTask(void *pvParameters) {
  // Local constants
  uint32_t notificationValue;
  uint16_t indexFrame, currFrame;
  uint16_t indexFrame_imu, currFrame_imu;

  for (;;) {
    // Wait task notification
    xTaskNotifyWait(0, 0b0011, &notificationValue, portMAX_DELAY);

    // Wait if notification is relevant
    if (notificationValue & 0b0001) {
      // Compute current index and frame
      indexFrame = np % BUFFER_SIZE;
      currFrame = np / BUFFER_SIZE + 1;

      // Save data in buffers
      kpBuffer[indexFrame] = kp;
      kiBuffer[indexFrame] = ki;
      kdBuffer[indexFrame] = kd;

      // Publish data if buffer is ready
      if (indexFrame == BUFFER_SIZE - 1 || np == total_time) {
          np = 0;
          publishVars(indexFrame, currFrame);
      }
    }
  }
}


// Function to recieve values from web server
void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print("Callback");
  JsonDocument doc;
  if (String(topic) == USER_SYS_PID) {
      deserializeJson(doc, message);
      kp = hex2Float((const char *) doc["kp"]);
      ki = hex2Float((const char *) doc["ki"]);
      kd = hex2Float((const char *) doc["kd"]);

      // Imprimir los valores recibidos
      Serial.println("PID actualizado:");
      Serial.println("Kp: " + String(kp));
      Serial.println("Ki: " + String(ki));
      Serial.println("Kd: " + String(kd));
  } else {
      Serial.println("Tópico no reconocido: " + String(topic));
  }
}

