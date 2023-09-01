#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "app_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "app_mqtt.h"
#include "app_nvs.h"
#include "app_ota.h"

#include <driver/gpio.h>

#define KEY "restart_cnt"
#define KEY1 "string"

static const char *TAG = "MQTTS_EXAMPLE";

void mqtt_data_func_callback(char *data)
{
    strcpy(data, "PHAN DUC MANH HIHI");
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == MQTT_DEV_EVENT && event_id == MQTT_DEV_EVENT_CONNECTED)
    {
        ESP_LOGW(TAG, "MQTT_DEV_EVENT_CONNECTED");
    }
    else if (event_base == MQTT_DEV_EVENT && event_id == MQTT_DEV_EVENT_DISCONNECT)
    {
        ESP_LOGW(TAG, "MQTT_DEV_EVENT_DISCONNECT");
    }
    else if (event_base == MQTT_DEV_EVENT && event_id == MQTT_DEV_EVENT_SUBSCRIBED)
    {
        ESP_LOGW(TAG, "MQTT_DEV_EVENT_SUBSCRIBED");
    }
    else if (event_base == MQTT_DEV_EVENT && event_id == MQTT_DEV_EVENT_DATA)
    {
        ESP_LOGW(TAG, "MQTT_DEV_EVENT_DATA");
    }
}

void mqtt_data_callback(char *data, int len)
{
    // printf("%s\n", data);
    if (strstr(data, "ON"))
    {
        gpio_set_level(2, 1);
    }
    else
        gpio_set_level(2, 0);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(MQTT_DEV_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    gpio_pad_select_gpio(2);
    gpio_set_direction(2, GPIO_MODE_INPUT_OUTPUT);
    // int restart_cnt = 0;
    // app_nvs_get_value(KEY, &restart_cnt);
    // restart_cnt ++;
    // app_nvs_set_value(KEY, restart_cnt);

    // char arr_set[50] = "";
    // sprintf(arr_set, "Hello World %d", restart_cnt);
    // char arr[50];
    // app_nvs_get_string(KEY1, arr);
    // app_nvs_set_string(KEY1, arr_set);
    // json_gen_test_result_t result;
    // json_gen_test(&result, "abcd",true,"abcde",12,"abcdef","NOT FOUND");
    app_mqtt_set_data_callback(mqtt_data_callback);
    app_config();
    mqtt_app_start();
    // mqtt_set_data_callback(mqtt_data_func_callback);
    // app_ota_start();
}
