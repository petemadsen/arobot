/**
 * http_request example.
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_log.h>

#include <lwip/err.h>

#include <esp_http_client.h>

#include <string.h>

#include "common.h"
#include "system/ota.h"
#include "system/my_settings.h"
#include "system/wifi.h"
#include "system/my_i2c.h"


static const char* MY_TAG = PROJECT_TAG("shutters");


#define DEFAULT_SAVE_URL	PROJECT_SHUTTERS_ADDRESS "/arobot/save"
#define DEFAULT_TIME_URL	PROJECT_SHUTTERS_ADDRESS "/time"
#define DEFAULT_CMD_URL		PROJECT_SHUTTERS_ADDRESS "/cmd/arobot"

#define SETTING_SAVE_URL	"shutters.save"
#define SETTING_TIME_URL	"shutters.time"

#define RCV_BUFLEN 64
static char m_rcv_buffer[RCV_BUFLEN];

static bool has_option(char* buffer, const char* option);


static esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(MY_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(MY_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(MY_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(MY_TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
			ESP_LOGI(MY_TAG, "HTTP_EVENT_ON_DATA, len=%d is_chunked=%d",
					 evt->data_len,
					 esp_http_client_is_chunked_response(evt->client));
//			if (!esp_http_client_is_chunked_response(evt->client))
//                printf("%.*s", evt->data_len, (char*)evt->data);
			if (evt->data_len < RCV_BUFLEN)
			{
				strncpy(m_rcv_buffer, (char*)evt->data, evt->data_len);
				m_rcv_buffer[evt->data_len] = 0;
			}
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(MY_TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(MY_TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}


void shutters_task(void* pvParameters)
{
	char* save_url = strdup(DEFAULT_SAVE_URL);
	settings_get_str(STORAGE_APP, SETTING_SAVE_URL, &save_url, true);

	char* time_url = strdup(DEFAULT_TIME_URL);
//	settings_get_str(SETTING_TIME_URL, &save_url, true);

	char* cmd_url = strdup(DEFAULT_CMD_URL);

	bool with_wifi = false;

#if 0
	uint8_t data[1] = { 0 };
	uint8_t addr = 8;
	for (;;)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		data[0] += 1;
		i2c_master_write_slave(addr, data, 1);
		printf("--write-i2c[%d]: %d\n", addr, data[0]);
	}
#endif
	for (;;)
	{
		xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED, false, true,
				portMAX_DELAY);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

	for (;;)
	{
		xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED, false, true,
				portMAX_DELAY);
		ESP_LOGI(MY_TAG, "Run.");

		settings_get_str(STORAGE_APP, SETTING_SAVE_URL, &save_url, true);
//		settings_get_str(SETTING_TIME_URL, &time_url, true);

		// -- check for ota update
		ESP_LOGI(MY_TAG, "Checking for OTA updates.");
		if (ota_need_update())
		{
			ESP_LOGW(MY_TAG, "OTA updated required. Trying to reboot.");
			const char* err_msg = ota_reboot();
			if (!err_msg)
			{
				for (;;)
					vTaskDelay(1000 * 1000 / portTICK_PERIOD_MS);
			}
			ESP_LOGE(MY_TAG, "OTA reboot failed: %s", err_msg);
		}
		else
			ESP_LOGI(MY_TAG, "No OTA update available.");

		// -- get time
		ESP_LOGI(MY_TAG, "Time: %s", time_url);
		m_rcv_buffer[0] = 0;
		esp_http_client_config_t request = {
			.url = time_url,
			.event_handler = _http_event_handle,
			.timeout_ms = 1000,
		};
		esp_http_client_handle_t client = esp_http_client_init(&request);
		esp_err_t err = esp_http_client_perform(client);
		if (err == ESP_OK)
		{
			int status = esp_http_client_get_status_code(client);
			if (status == 200 && strlen(m_rcv_buffer))
			{
				time_t ts = (time_t)atol(m_rcv_buffer);
				struct timeval tv = { ts, 0 };
				settimeofday(&tv, NULL);
			}
		}
		else
			ESP_LOGE(MY_TAG, "Time request failed.");

		// -- cmd
		ESP_LOGI(MY_TAG, "Cmd: %s", cmd_url);
		m_rcv_buffer[0] = 0;
		esp_http_client_set_url(client, cmd_url);
		err = esp_http_client_perform(client);
		if (err == ESP_OK)
		{
			int status = esp_http_client_get_status_code(client);
			if (status == 200 && strlen(m_rcv_buffer))
			{
				ESP_LOGI(MY_TAG, "Cmd: %s", m_rcv_buffer);
				if (has_option(m_rcv_buffer, "wifi"))
					with_wifi = true;
				if (has_option(m_rcv_buffer, "nowifi"))
					with_wifi = false;
			}
		}
		else
			ESP_LOGE(MY_TAG, "Cmd request failed.");

		// -- save
		ESP_LOGI(MY_TAG, "Save: %s", save_url);
		const size_t POST_MAXLEN = 300;
		char* save_data = malloc(POST_MAXLEN);
		int save_data_len = snprintf(save_data, POST_MAXLEN,
									 "boots=%d"
									 "&wifi=%d",
									 settings_boot_counter(),
									 with_wifi ? 1 : 0);

		esp_http_client_set_method(client, HTTP_METHOD_POST);
		esp_http_client_set_url(client, save_url);
		esp_http_client_set_post_field(client, save_data, save_data_len);
		err = esp_http_client_perform(client);
		if (err == ESP_OK)
		{
			ESP_LOGI(MY_TAG, "Save/Status = %d, content_length = %d",
					esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client));
		}
		free(save_data);

		// -- cleanup
		esp_http_client_cleanup(client);

		// -- wait 3600 secs = 1h
		vTaskDelay(3600 * 1000 / portTICK_PERIOD_MS);
	}
}


static bool has_option(char* buffer_ro, const char* option)
{
	char* buffer = strdup(buffer_ro);
	bool ret = false;
	char delimiter[] = ",";
	char* ptr = strtok(buffer, delimiter);
	while (ptr != NULL)
	{
		if (strcmp(ptr, option) == 0)
		{
			ret = true;
			break;
		}
		ptr = strtok(NULL, delimiter);
	}

	free(buffer);
	return ret;
}
