/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <esp_err.h>
#include <esp_log.h>
#include <esp_spi_flash.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#define DEVICE_MAC_ADDR_BUF_LENGTH 18
#define GPIO_BEEPER GPIO_NUM_2

static const char *TAG = "show_mac";

esp_err_t get_wifi_mac_addr(char *buf) {
  esp_err_t ret = ESP_OK;
  uint8_t mac[6];

  ret = esp_efuse_mac_get_default(mac);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to read base MAC address.");
    return ret;
  }

  int cnt =
      snprintf(buf, DEVICE_MAC_ADDR_BUF_LENGTH, "%02x:%02x:%02x:%02x:%02x:%02x",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (cnt != DEVICE_MAC_ADDR_BUF_LENGTH - 1) {
    ESP_LOGE(TAG, "Failed to format MAC address.");
    return ESP_FAIL;
  }

  return ESP_OK;
}

esp_err_t print_wifi_mac_address(void) {
  char buf[DEVICE_MAC_ADDR_BUF_LENGTH];
  esp_err_t ret;

  ret = get_wifi_mac_addr(buf);
  if (ret != ESP_OK) {
    return ret;
  }

  ESP_LOGI(TAG, "Device WiFi MAC address: %s", buf);
  return ESP_OK;
}

static void init_beeper(void) {
  gpio_pad_select_gpio(GPIO_BEEPER);
  gpio_intr_disable(GPIO_BEEPER);
  gpio_set_direction(GPIO_BEEPER, GPIO_MODE_OUTPUT);
}

void app_main() {
  init_beeper();
  int level = 0;
  while (true) {
    ESP_ERROR_CHECK(print_wifi_mac_address());
    level ^= 1;
    ESP_LOGI(TAG, "Beeper level: %d", level);
    gpio_set_level(GPIO_BEEPER, level);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
