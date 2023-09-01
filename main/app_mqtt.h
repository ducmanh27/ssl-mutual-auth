#ifndef __APP_MQTT_H
#define __APP_MQTT_H

ESP_EVENT_DECLARE_BASE(MQTT_DEV_EVENT);
enum mqtt_event{
    MQTT_DEV_EVENT_CONNECTED,
    MQTT_DEV_EVENT_DISCONNECT,
    MQTT_DEV_EVENT_DATA,
    MQTT_DEV_EVENT_SUBSCRIBED,
    MQTT_DEV_EVENT_UNSUBSCRIBED,
    MQTT_DEV_EVENT_PUBLISHED
};
typedef void (*mqtt_data_callback_t)(char *data);
typedef struct {
    char buf[256];
    size_t offset;
} json_gen_test_result_t;

typedef void (*mqtt_data_handle_t) (char *data, int len);
void app_mqtt_set_data_callback(void *cb);

void mqtt_app_start(void);
void mqtt_set_data_callback(void *cb);
void json_gen_test(json_gen_test_result_t *result, char *key1, bool value1, char *key2, int value2, char *key3, char *value3);
void app_mqtt_publish(char *topic, char *data, int len);
void app_mqtt_subscribe(char *topic);

#endif