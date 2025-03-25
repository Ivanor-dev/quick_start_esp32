#include <stdio.h>
#include <inttypes.h>
#include "esp_http_client.h"
#include "esp_event.h"
#include "esp_log.h"
#include "http_client.h"
#include "esp_crt_bundle.h"

#define TAG "HTTP_CLIENT"
#define URL "http://192.168.0.109:5500/Site-Pallet-Manager/index.html"
#define URL2 "https://marine-api.open-meteo.com/v1/marine?latitude=51.874&longitude=-176.634&hourly=wave_height"
#define URL3 "http://192.168.100.221:8080/produto"

esp_err_t _http_event_handle(esp_http_client_event_t *event) {

    switch (event->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG ,"HTTP_EVENT_ERROR");
        ESP_LOGI(TAG, "Error: %s", esp_err_to_name(esp_http_client_get_errno(event->client)));
        break;
    
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG ,"HTTP_EVENT_ON_CONNECTED");
        break;
    
    case HTTP_EVENT_HEADERS_SENT:
        ESP_LOGI(TAG ,"HTTP_EVENT_HEADERS_SENT");
        break;
    
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG ,"HTTP_EVENT_HEADER");
        printf("%.*s", event->data_len, (char*)event->data);
        break;

    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", event->data_len);
        printf("%.*s", event->data_len, (char*)event->data);
        printf("\n");
        break;

    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG,"HTTP_EVENT_ON_FINISH");
        break;

    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG ,"HTTP_EVENT_DISCONNECTED");
        break;

    case HTTP_EVENT_REDIRECT:
        ESP_LOGI(TAG ,"HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;

}

void http_request(){
    esp_http_client_config_t config = {
        .url = URL2,
        .event_handler = _http_event_handle,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if(err == ESP_OK) {
        ESP_LOGI(TAG, "Status: %d, content_length: %lld",
        esp_http_client_get_status_code(client),
        esp_http_client_get_content_length(client));
    }

    esp_http_client_cleanup(client);
}


void http_response(){
    esp_http_client_config_t config = {
        .url = URL2,
        .event_handler = _http_event_handle,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Dados a serem enviados no corpo do POST
    const char *post_data = "{ \"name\": \"Produto A\",\"dataProduction\": \"2024-09-24\",\"dataValidity\": \"2025-09-24\",\"weight\": 120.5, \"supplier\": \"Fornecedor ABC\", \"countProductPack\": 1000}";

    // Configura o método para POST
    esp_http_client_set_method(client, HTTP_METHOD_POST);

    // Define o cabeçalho da requisição como JSON
    esp_http_client_set_header(client, "Content-Type", "application/json");

    // Define os dados do corpo do POST
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Executa a requisição
    esp_err_t err = esp_http_client_perform(client);

    if(err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // Limpa o cliente HTTP
    esp_http_client_cleanup(client);
}
