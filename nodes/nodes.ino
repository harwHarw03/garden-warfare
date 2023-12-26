#define USE_WEMOS

#include <ESP8266WiFi.h>
#include <espnow.h>

ADC_MODE(ADC_VCC);
uint8_t soilPin = 34;
const float intercept = 1.72;
const float slope = -0.11;

uint8_t remoteDevice[] = {0xe0, 0x5a, 0x1b, 0x77, 0x68, 0xfc};
//{0xC8, 0xF0, 0x9E, 0xF2, 0x63, 0xDC};
//0xC8, 0xF0, 0x9E, 0xF2, 0x63, 0xDC

typedef struct __attribute__((packed)) {
    uint8_t node_id;
    float soil_moisture;
} sensorData_t;

sensorData_t nodeData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 10000;  // send readings timer

bool esp_now_send_status;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
  esp_now_send_status = true;
}

void ICACHE_FLASH_ATTR simple_cb(u8 *macaddr, u8 *data, u8 len) {

}

#define SLEEP 2   // 1 minite sleep

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  uint8_t chnnel = wifi_get_channel();
  Serial.print("chnnel=");
  Serial.println(chnnel);
  
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(remoteDevice, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  static int isSended = 0;
  if (isSended == 0) {
    lastTime = millis();
    nodeData.node_id = 1;
    nodeData.soil_moisture = random(0, 100); //read_moisture();
    Serial.print("soil moisture : \t");
    Serial.println(nodeData.soil_moisture);
    esp_now_send(remoteDevice, (uint8_t *) &nodeData, sizeof(nodeData));

    isSended = 1;
    // Goto DEEP SLEEP
    Serial.println("DEEP SLEEP START!!");
    uint32_t time_us = SLEEP * 1000 * 1000;
    ESP.deepSleep(time_us, WAKE_RF_DEFAULT);
  }
}

float read_moisture()
{
  float voltage = analogRead(soilPin) * 3.3 / 4095.0;
  float volumetric_water_content = ((1.0/voltage)*slope)+intercept; 
  // inverse rmse, --> volumetric water content in m3/m3
  return volumetric_water_content;
}