#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "http_client.h"

SemaphoreHandle_t conexaoWiFiSemaphore;

void reliase_HTTP_Request (void * param){
    while(true){
        if(xSemaphoreTake(conexaoWiFiSemaphore, portMAX_DELAY)){
            ESP_LOGI("Main Task","Reliase Request");
            http_request();
        }
    }
}

void reliase_HTTP_Response(void * param){
    while(true){
        if(xSemaphoreTake(conexaoWiFiSemaphore, portMAX_DELAY)){
            ESP_LOGI("Main Task", "Reliase Response");
            http_response();
        }
    }
}

void app_main(void)
{
    
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    conexaoWiFiSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    xTaskCreate(&reliase_HTTP_Request, "can do response", 4096, NULL, 1, NULL);

}