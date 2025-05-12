// primeiro execute idf.py menuconfig va em Partition Table 
// e troque de Single factory app (large), no OTA para
// Custon partition table CSV e coloque o nome da tabela de partição utilizada
// no caso será partitions_example.csv

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "SPIFFS exemplo";
esp_vfs_spiffs_conf_t conf;
FILE* file;
int read_flag = 0;

void spiffs_config(){
    ESP_LOGI(TAG, "Inicializando SPIFFS");
    
    conf.base_path = "/spiffs";
    conf.partition_label = NULL;
    conf.max_files = 5;
    conf.format_if_mount_failed = true;

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    if(used > total) {
        ret = esp_spiffs_check(conf.partition_label);

        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return;
        } else {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }
}

void write_file(char *PATH, char *info) {
    struct stat st;
    file = fopen(PATH, "w");

    if(file == NULL){
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(file, info);
    fclose(file);
    ESP_LOGI(TAG, "File written");

    if(stat("/spiffs/foo.txt", &st) == 0) {
        unlink("/spiffs/foo.txt");
    }

    ESP_LOGI(TAG, "Renaming file");
    if(rename(PATH, "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        read_flag = 1;
        return;
    }
}

void append_file(char *PATH, char *info) {
    if(read_flag == 1){
        if(rename("/spiffs/foo.txt", PATH) != 0) {
            ESP_LOGE(TAG, "Rename failed");
            read_flag = 0;
            return;
        }
    }
    
    struct stat st;
    file = fopen(PATH, "a");

    if(file == NULL){
        ESP_LOGE(TAG, "Failed to open file for appending");
        return;
    }
    fprintf(file, info);
    fclose(file);
    ESP_LOGI(TAG, "File append");

    if(stat("/spiffs/foo.txt", &st) == 0) {
        unlink("/spiffs/foo.txt");
    }

    ESP_LOGI(TAG, "Renaming file");
    if(rename(PATH, "/spiffs/foo.txt") != 0) {
        ESP_LOGE(TAG, "Rename failed");
        read_flag = 1;
        return;
    }

}

void read_file() {

    file = fopen("/spiffs/foo.txt", "r");
    if(file == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), file);
    fclose(file);
    
    char* pos = strchr(line, '\n');
    if(pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file: '%s'", line);

}

void app_main(void) {

    spiffs_config();
    write_file("/spiffs/hello.txt", "Test Phrase");
    read_file();
    append_file("/spiffs/hello.txt", "Append test with change the name");
    read_file();
    esp_vfs_spiffs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "SPIFFS unmounted");
}
    
