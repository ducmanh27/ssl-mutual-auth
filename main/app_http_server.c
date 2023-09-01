
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include "esp_netif.h"
#include <esp_http_server.h>
#include <string.h>
#include "app_http_server.h"
static const char *TAG1 = "HTTP";
static void app_http_server_post_default_handler_func(char *buf, int len);
static void app_http_server_get_default_handler_func(char *url_querry, char *host);
static http_post_handler_func_t http_post_handler_func = app_http_server_post_default_handler_func;
static http_get_handler_func_t http_get_handler_func = app_http_server_get_default_handler_func;
static httpd_req_t *resp;
static char http_post_buf[APP_HTTP_SERVER_MAX_BUFFER];
static httpd_handle_t http_server = NULL;

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

static void app_http_server_post_default_handler_func(char *buf, int len)
{
    ESP_LOGI(TAG1, "============RECEIVED POST DATA===========");
    ESP_LOGI(TAG1, "%.*s", len, buf);
    ESP_LOGI(TAG1, "=========================================");
}
static void app_http_server_get_default_handler_func(char *url_querry, char *host)
{
    ESP_LOGI(TAG1, "============RECEIVED GET PARAM===========");
    ESP_LOGI(TAG1, "url_querry: %s host: %s", url_querry, host);
    ESP_LOGI(TAG1, "=========================================");
}
/* An HTTP GET handler */
esp_err_t http_get_handler(httpd_req_t *req)
{
    char *pt = index_html_start;
    ESP_LOGW(TAG1, "<=> Address HTML: 0x%x",(unsigned int)pt);
    httpd_resp_set_type(req, "text/html"); // Nhúng giao diện HTML vào
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
    return ESP_OK;
}

static const httpd_uri_t http_get = {
    .uri = APP_HTTP_SERVER_GET_DEFAULT_URI,
    .method = HTTP_GET,
    .handler = http_get_handler,
    .user_ctx = NULL};

/* An HTTP POST handler */
static esp_err_t http_post_handler(httpd_req_t *req)
{
    char *buf = http_post_buf;
    int buf_len = sizeof(http_post_buf); //
    int ret, remaining = req->content_len;
    while ((remaining > 0))
    {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, buf_len))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
        http_post_handler_func(buf, req->content_len);
    }

    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t http_post = {
    .uri = APP_HTTP_SERVER_POST_DEFAULT_URI, //
    .method = HTTP_POST,
    .handler = http_post_handler,
    .user_ctx = NULL};

void app_http_server_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    ESP_LOGI(TAG1, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&http_server, &config) == ESP_OK)
    {
        ESP_LOGI(TAG1, "Registering URI handlers");
        httpd_register_uri_handler(http_server, &http_get);
        httpd_register_uri_handler(http_server, &http_post);
    }
}

void app_http_server_stop(void)
{
    httpd_stop(http_server);
}
void app_http_server_post_set_callback(void *post_handler_callback)
{
    http_post_handler_func = post_handler_callback;
}
void app_http_server_get_set_callback(void *get_handler_callback)
{
    http_get_handler_func = get_handler_callback;
}