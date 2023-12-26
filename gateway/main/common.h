#ifndef _COMMON_H_
#define _COMMON_H_

#include "driver/gpio.h"

typedef enum {
    ESPNOW_SEND_CB,
    ESPNOW_RECV_CB,
} example_espnow_event_id_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} example_espnow_event_send_cb_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} example_espnow_event_recv_cb_t;

typedef union {
    example_espnow_event_send_cb_t send_cb;
    example_espnow_event_recv_cb_t recv_cb;
} example_espnow_event_info_t;

typedef struct {
    example_espnow_event_id_t id;
    example_espnow_event_info_t info;
} espnow_event_t;

typedef struct __attribute__((packed)) {
    uint8_t node_id;
    float soil_moisture;
} sensorData_t;

typedef struct {
    uint8_t pump_id;
    gpio_num_t pump_pin;
} pumpData_t;

typedef enum {PUBLISH, SUBSCRIBE, STOP} TOPIC_TYPE;

typedef struct {
    int data_len;
    char data[64];
} MQTT_t;

#endif
