#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
// for idf V5
#include "esp_mac.h"
#include "common.h"
#include "driver/gpio.h"

static const char *TAG = "MAIN_GATEWAY";

/* MAC ADDRESS LIST */
uint8_t client1[6] = {0xEC, 0x94, 0xCB, 0x4C, 0xCD, 0x88};
float moisture_threshold = 50.0;

pumpData_t pumps[] = {
    {1, GPIO_NUM_35},
    {2, GPIO_NUM_34}
};

void control_pump(uint8_t pump_id, float soil_moisture)
{
    if (soil_moisture < moisture_threshold)
    {
        ESP_LOGI(TAG, "Turning on Pump %u (GPIO %u)", pump_id, pumps[pump_id - 1].pump_pin);
        gpio_set_level(pumps[pump_id - 1].pump_pin, 1);
        vTaskDelay(pdMS_TO_TICKS(10000));
        gpio_set_level(pumps[pump_id - 1].pump_pin, 0);
    }
}

void on_recv_cb(const esp_now_recv_info_t *packet_info, const uint8_t *packet, int packet_len)
{
    if (packet_len == sizeof(sensorData_t)) {
        sensorData_t *received_data = (sensorData_t *)packet;

        ESP_LOGI(TAG, "Received data from " MACSTR ":", MAC2STR(packet_info->src_addr));
        ESP_LOGI(TAG, "Node ID: %u", received_data->node_id);
        ESP_LOGI(TAG, "Soil Moisture: %f", received_data->soil_moisture);
        
        control_pump(received_data->node_id, received_data->soil_moisture);
    } else {
        ESP_LOGE(TAG, "Received invalid packet length: %d", packet_len);
    }
}

void system_init(void)
{
    ESP_LOGI(TAG, "System Initializing ...");
    
    ESP_ERROR_CHECK(nvs_flash_init());

    /*init wiffi*/
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_err_t err = esp_now_init();
    if (err == ESP_OK) 
    {
        printf("esp now init ok\n");
        ESP_ERROR_CHECK(esp_now_register_recv_cb(on_recv_cb));
        // esp_now_set_self_role(1);
         /*
        ESP_NOW_ROLE_IDLE	=	0,
        ESP_NOW_ROLE_CONTROLLER,
        ESP_NOW_ROLE_SLAVE,
        ESP_NOW_ROLE_COMBO,	 //	both	slave	and	controller
        ESP_NOW_ROLE_MAX,
        */
        // esp_now_add_peer(client1);
    } else {
        printf("esp_now init failed\n");
    }
    ESP_LOGD(TAG, "System Initialized, Start Listening from Nodes");
}

void app_main(void)
{
    uint8_t my_mac[6];
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(my_mac));

    uint8_t index = 0;
    char macId[50];

    for(uint8_t i = 0; i < 6; i++) 
    {
        index += sprintf(&macId[index], "0x%02x", my_mac[i]);
        if (i < 5) index += sprintf(&macId[index], ":");
    }
    ESP_LOGI(TAG, "macId = %s", macId);

    system_init();

    while(1)
    {
        printf("Waiting packet ...\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
