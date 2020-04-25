/**
 * This code is public domain.
 */
#include <lwip/apps/sntp.h>

#include <esp_spiffs.h>
#include <esp_log.h>

#include "common.h"
#include "system/my_settings.h"
#include "system/my_log.h"
#include "system/ota.h"

#include "my_http.h"


static const char* MY_TAG = PROJECT_TAG("http");


static esp_err_t status_handler(httpd_req_t* req);
static esp_err_t system_handler(httpd_req_t* req);
static esp_err_t settings_get_handler(httpd_req_t* req);
static esp_err_t settings_set_handler(httpd_req_t* req);
static esp_err_t log_handler(httpd_req_t* req);
static esp_err_t action_handler(httpd_req_t* req);

static const char* RET_OK = "OK";
static const char* RET_ERR = "ERR";
static const char* RET_ERR_SIZE = "ERR_SIZE";
static const char* RET_ERR_INVALID = "ERR_INVALID";

static httpd_uri_t basic_handlers[] = {
	{
		.uri	= "/status",
		.method	= HTTP_GET,
		.handler= status_handler,
	},
	{
		.uri	= "/action",
		.method	= HTTP_GET,
		.handler= action_handler,
	},
	{
		.uri	= "/system",
		.method	= HTTP_GET,
		.handler= system_handler,
	},
	{
		.uri	= "/sget",
		.method	= HTTP_GET,
		.handler= settings_get_handler,
	},
	{
		.uri	= "/sset",
		.method	= HTTP_GET,
		.handler= settings_set_handler,
	},
	{
		.uri	= "/log",
		.method	= HTTP_GET,
		.handler= log_handler,
	}
};


static bool save_to_file(const char* filename, const char* buf, size_t buf_len);


httpd_handle_t http_start()
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
	config.max_uri_handlers = 11;

	if (httpd_start(&server, &config) == ESP_OK)
	{
		int num = sizeof(basic_handlers) / sizeof(httpd_uri_t);
		for (int i=0; i<num; ++i)
			httpd_register_uri_handler(server, &basic_handlers[i]);
		return server;
	}

	return NULL;
}


void http_stop(httpd_handle_t server)
{
	httpd_stop(server);
}


static bool get_int(httpd_req_t* req, int* val)
{
	bool ret = false;

    size_t buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
	{
        char* buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
		{
			ret = (sscanf(buf, "%d", val)==1);
		}
		free(buf);
	}

	return ret;
}


static bool is_string(httpd_req_t* req, const char* str)
{
	bool ret = false;

	size_t buf_len = httpd_req_get_url_query_len(req) + 1;
	if (buf_len > 1)
	{
		char* buf = malloc(buf_len);
		if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
		{
			ret = strcmp(buf, str) == 0;
		}
		free(buf);
	}

	return ret;
}


esp_err_t status_handler(httpd_req_t* req)
{
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);

	int64_t uptime = esp_timer_get_time() / 1000 / 1000;

	const size_t bufsize = 1024;
	char* buf = malloc(bufsize);
	int buflen = snprintf(buf, bufsize,
						  "version %s\n"
						  " ident %s\n"
						  " free-ram %u\n"
						  " boots %d\n"
						  " uptime %lld\n"
						  " time %02d:%02d\n",
						  PROJECT_VERSION,
						  PROJECT_NAME,
						  esp_get_free_heap_size(),
						  settings_boot_counter(),
						  uptime,
						  timeinfo.tm_hour, timeinfo.tm_min);
	httpd_resp_send(req, buf, buflen);

	free(buf);
	return ESP_OK;
}


esp_err_t action_handler(httpd_req_t* req)
{
	size_t buf_len = httpd_req_get_url_query_len(req) + 1;
	if (buf_len > 1)
	{
		char* key = malloc(buf_len);
		if (httpd_req_get_url_query_str(req, key, buf_len) == ESP_OK)
		{
			printf("--%s\n", key);
#if 0
			int32_t val;
			ret = settings_get_int32(STORAGE_APP, key, &val, false);
			if (ret == ESP_OK)
			{
				reply_len = 20;
				reply = malloc(reply_len);
				reply_len = snprintf(reply, reply_len, "%d", val);
				ret = ESP_OK;
			}

			if (!reply)
			{
				reply = strdup(RET_ERR);
				ret = settings_get_str(STORAGE_APP, key, &reply, false);
				reply_len = strlen(reply);
			}
#endif
		}
		free(key);
	}

	httpd_resp_send(req, RET_OK, strlen(RET_OK));
	return ESP_OK;
}


esp_err_t system_handler(httpd_req_t* req)
{
	const char* reply = RET_ERR;

	size_t buf_len = httpd_req_get_url_query_len(req) + 1;
	if (buf_len > 1)
	{
		char* buf = malloc(buf_len);
		if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
		{
			if (strcmp(buf, "ota") == 0)
			{
				reply = ota_reboot();
				if (!reply)
					reply = RET_OK;
			}
			else if (strcmp(buf, "reboot") == 0)
			{
				xTaskCreate(ota_reboot_task, "ota_reboot_task", 2048, NULL, 5, NULL);
				reply = RET_OK;
			}
			else if (strcmp(buf, "logs") == 0)
			{
				if (log_handler(req) == ESP_OK)
					reply = NULL;
			}
			else if (strcmp(buf, "status") == 0)
			{
				if (status_handler(req) == ESP_OK)
					reply = NULL;
			}
		}
		free(buf);
	}

	if (reply)
		httpd_resp_sendstr(req, reply);

	return ESP_OK;
}


esp_err_t settings_get_handler(httpd_req_t* req)
{
	esp_err_t ret = ESP_FAIL;
	char* reply = NULL;
	int reply_len = 0;

	size_t buf_len = httpd_req_get_url_query_len(req) + 1;
	if (buf_len > 1)
	{
		char* key = malloc(buf_len);
		if (httpd_req_get_url_query_str(req, key, buf_len) == ESP_OK)
		{
			int32_t val;
			ret = settings_get_int32(STORAGE_APP, key, &val, false);
			if (ret == ESP_OK)
			{
				reply_len = 20;
				reply = malloc(reply_len);
				reply_len = snprintf(reply, reply_len, "%d", val);
				ret = ESP_OK;
			}

			if (!reply)
			{
				reply = strdup(RET_ERR);
				ret = settings_get_str(STORAGE_APP, key, &reply, false);
				reply_len = strlen(reply);
			}
		}
		free(key);
	}

	if (ret != ESP_OK)
		httpd_resp_send_404(req);
	else
		httpd_resp_send(req, reply, reply_len);

	if (reply)
		free(reply);

	return ESP_OK;
}


esp_err_t settings_set_handler(httpd_req_t* req)
{
	esp_err_t ret = ESP_FAIL;

	size_t buf_len = httpd_req_get_url_query_len(req) + 1;
	if (buf_len > 1)
	{
		char* buf = malloc(buf_len);
		if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
		{
			int32_t val;
			char* key = malloc(buf_len);
			char* value = malloc(buf_len);

			if (sscanf(buf, "%s=%d", key, &val) == 2)
			{
				ret = settings_set_int32(STORAGE_APP, key, val, true);
			}
			else if (sscanf(buf, "%[^=]=%s", key, value) == 2)
			{
				ret = settings_set_str(STORAGE_APP, key, value, true);
			}
			else if (strcmp(buf, "clear") == 0)
			{
				ret = settings_clear(STORAGE_APP);
			}
			else if (strcmp(buf, "ERASEALL") == 0)
			{
				ret = settings_erase();
			}

			free(key);
			free(value);
		}
		free(buf);
	}

	if (ret != ESP_OK)
		httpd_resp_send_404(req);
	else
		httpd_resp_sendstr(req, RET_OK);

	return ret;
}


esp_err_t log_handler(httpd_req_t* req)
{
	char* line = malloc(LOG_MAX_LINELEN);
	for (uint8_t i = 0; i < LOG_MAX_ENTRIES; ++i)
	{
		if (mylog_get(i, &line))
			httpd_resp_sendstr_chunk(req, line);
	}
	free(line);
	httpd_resp_sendstr(req, NULL);

	return ESP_OK;
}
