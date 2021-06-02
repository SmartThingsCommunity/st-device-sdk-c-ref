/* ***************************************************************************
 *
 * Copyright (c) 2020 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

//for implementing main features of IoT device
#include <stdbool.h>

#include "freertos/FreeRTOS.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "st_dev.h"
#include "cJSON.h"
#include "string.h"
#include "ota_util.h"
#include "caps_firmwareUpdate.h"

#include "mbedtls/sha256.h"
#include "mbedtls/rsa.h"
#include "mbedtls/pk.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_https_ota.h>
#include <esp_ota_ops.h>
#include <esp_log.h>

#define CONFIG_OTA_SERVER_URL "https://192.168.1.3:4443/"
#define CONFIG_FIRMWARE_VERSOIN_INFO_URL CONFIG_OTA_SERVER_URL"version_info.json"
#define CONFIG_FIRMWARE_UPGRADE_URL CONFIG_OTA_SERVER_URL"signed_ota_demo.bin"

#define OTA_SIGNATURE_SIZE 256
#define OTA_SIGNATURE_FOOTER_SIZE 6
#define OTA_SIGNATURE_PREFACE_SIZE 6
#define OTA_DEFAULT_SIGNATURE_BUF_SIZE OTA_SIGNATURE_PREFACE_SIZE + OTA_SIGNATURE_SIZE + OTA_SIGNATURE_FOOTER_SIZE

#define OTA_DEFAULT_BUF_SIZE 256
#define OTA_CRYPTO_SHA256_LEN 32

extern const uint8_t public_key_start[]	asm("_binary_public_key_pem_start");
extern const uint8_t public_key_end[]		asm("_binary_public_key_pem_end");

extern const uint8_t root_pem_start[]	asm("_binary_root_pem_start");
extern const uint8_t root_pem_end[]		asm("_binary_root_pem_end");


static unsigned int polling_day = 1;

int ota_get_polling_period_day()
{
	return polling_day;
}

void _set_polling_period_day(unsigned int value)
{
	polling_day = value;
}

void ota_nvs_flash_init()
{
	// Initialize NVS.
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
		// OTA app partition table has a smaller NVS partition size than the non-OTA
		// partition table. This size mismatch may cause NVS initialization to fail.
		// If this happens, we erase NVS partition and initialize NVS again.
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK( err );
}

static const char name_deviceInfo[] = "deviceInfo";
static const char name_version[] = "firmwareVersion";

ota_err_t ota_api_get_firmware_version_load(unsigned char *device_info,
		unsigned int device_info_len, char **firmware_version)
{
	ota_err_t ret = OTA_OK;
	cJSON *root = NULL;
	cJSON *profile = NULL;
	cJSON *item = NULL;
	char *version = NULL;
	char *data = NULL;
	size_t str_len = 0;

	if (!device_info || device_info_len == 0)
		return OTA_ERR_INVALID_ARG;

	data = malloc((size_t) device_info_len + 1);
	if (!data) {
		return OTA_ERR_NO_MEM;
	}

	memcpy(data, device_info, device_info_len);
	data[device_info_len] = '\0';

	root = cJSON_Parse((char *)data);
	profile = cJSON_GetObjectItem(root, name_deviceInfo);
	if (!profile) {
		ret = OTA_FAIL;
		goto load_out;
	}

	/* version */
	item = cJSON_GetObjectItem(profile, name_version);
	if (!item) {
		ret = OTA_FAIL;
		goto load_out;
	}
	str_len = strlen(cJSON_GetStringValue(item));
	version = malloc(str_len + 1);
	if (!version) {
		ret = OTA_ERR_NO_MEM;
		goto load_out;
	}
	strncpy(version, cJSON_GetStringValue(item), str_len);
	version[str_len] = '\0';

	*firmware_version = version;

	if (root)
		cJSON_Delete(root);

	free(data);

	return ret;

load_out:

	if (firmware_version)
		free(firmware_version);

	if (root)
		cJSON_Delete(root);
	if (data)
		free(data);

	return ret;
}

static const char name_versioninfo[] = "versioninfo";
static const char name_latest[] = "latest";
static const char name_upgrade[] = "upgrade";
static const char name_polling[] = "polling";

static ota_err_t _get_available_version(char *update_info, unsigned int update_info_len, char *firmware_version, char **new_version)
{
	cJSON *root = NULL;
	cJSON *profile = NULL;
	cJSON *item = NULL;
	char *latest_version = NULL;
	char *data = NULL;
	size_t str_len = 0;

	bool is_new_version = false;

	ota_err_t ret = OTA_OK;

	if (!update_info) {
		printf("Invalid parameter \n");
		return OTA_ERR_INVALID_ARG;
	}

	data = malloc((size_t) update_info_len + 1);
	if (!data) {
		printf("Couldn't allocate memory to add version info\n");
		return OTA_ERR_NO_MEM;
	}
	memcpy(data, update_info, update_info_len);
	data[update_info_len] = '\0';

	root = cJSON_Parse((char *)data);
	profile = cJSON_GetObjectItem(root, name_versioninfo);
	if (!profile) {
		ret = OTA_FAIL;
		goto clean_up;
	}

	/* polling */
	item = cJSON_GetObjectItem(profile, name_polling);
	if (!item) {
		ret = OTA_FAIL;
		goto clean_up;
	}
	polling_day = atoi(cJSON_GetStringValue(item));

	_set_polling_period_day((unsigned int)polling_day);

	cJSON * array = cJSON_GetObjectItem(profile, name_upgrade);

	for (int i = 0 ; i < cJSON_GetArraySize(array) ; i++)
	{
		char *upgrade = cJSON_GetArrayItem(array, i)->valuestring;

		if (strcmp(upgrade, firmware_version) == 0) {
			is_new_version = true;
			break;
		}
	}

	printf("isNewVersion : %d \n", is_new_version);

	if (is_new_version) {

		/* latest */
		item = cJSON_GetObjectItem(profile, name_latest);
		if (!item) {
			printf("IOT_ERROR_UNINITIALIZED \n");
			ret = OTA_FAIL;
			goto clean_up;
		}

		str_len = strlen(cJSON_GetStringValue(item));
		latest_version = malloc(str_len + 1);
		if (!latest_version) {
			printf("Couldn't allocate memory to add latest version\n");
			ret = OTA_ERR_NO_MEM;
			goto clean_up;
		}
		strncpy(latest_version, cJSON_GetStringValue(item), str_len);
		latest_version[str_len] = '\0';

		*new_version = latest_version;
	}

clean_up:

	if (root)
		cJSON_Delete(root);
	if (data)
		free(data);

	return ret;
}

static void _print_sha256 (const uint8_t *image_hash, const char *label)
{
	char hash_print[OTA_CRYPTO_SHA256_LEN * 2 + 1];
	hash_print[OTA_CRYPTO_SHA256_LEN * 2] = 0;
	for (int i = 0; i < OTA_CRYPTO_SHA256_LEN; ++i) {
		sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
	}
	printf("%s: %s \n", label, hash_print);
}

static int _crypto_sha256(const unsigned char *src, size_t src_len, unsigned char *dst)
{
	int ret;

	printf("src: %d@%p, dst: %p", src_len, src, dst);

	ret = mbedtls_sha256_ret(src, src_len, dst, 0);
	if (ret) {
		printf("mbedtls_sha256_ret = -0x%04X", -ret);
		return ret;
	}

	return 0;
}

static int _pk_verify(const unsigned char *sig, const unsigned char *hash)
{
    int ret;

    mbedtls_pk_context pk;

    unsigned char *public_key = (unsigned char *) public_key_start;
    unsigned int public_key_len = public_key_end - public_key_start;
    unsigned char *public_key_buffer = NULL;

    public_key_buffer = (unsigned char *)malloc(public_key_len + 1);
    if (!public_key_buffer) {
        printf("Couldn't allocate memory \n");
        return -1;
    }

    memcpy(public_key_buffer, public_key, public_key_len);
    public_key_buffer[public_key_len] = '\0';

    mbedtls_pk_init( &pk );

    ret = mbedtls_pk_parse_public_key( &pk, (const unsigned char *)public_key_buffer, public_key_len + 1 );
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100);
        printf( "parse error -0x%04x - %s \n", -ret, error_buf );
        goto clean_up;
    }

    if (!mbedtls_pk_can_do( &pk, MBEDTLS_PK_RSA))
    {
    	printf("Failed! Key is not an RSA key\n");
    	ret = MBEDTLS_ERR_SSL_PK_TYPE_MISMATCH;
    	goto clean_up;
    }

    ret = mbedtls_rsa_check_pubkey(mbedtls_pk_rsa(pk));
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100);
        printf( "parse error -0x%04x - %s \n", -ret, error_buf );
        goto clean_up;
    }

    if ((ret = mbedtls_pk_verify(&pk, MBEDTLS_MD_SHA256, hash, OTA_CRYPTO_SHA256_LEN, sig, OTA_SIGNATURE_SIZE)) != 0 ) {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100);
        printf( "parse error -0x%04x - %s \n", -ret, error_buf );
        goto clean_up;
    }

clean_up:

    if (public_key_buffer) {
        free(public_key_buffer);
    }

    mbedtls_pk_free( &pk );

    return ret;
}

static bool _check_firmware_validation(const unsigned char *sha256, unsigned char *sig_data, unsigned int sig_len)
{
	bool ret = false;

	unsigned char hash[OTA_CRYPTO_SHA256_LEN] = {0,};

	// Get the message digest info structure for SHA256
	if (!sig_data) {
		printf("invalid sig data \n");
		goto clean_up;
	}

	if (sig_len < OTA_DEFAULT_SIGNATURE_BUF_SIZE) {
		printf("invalid sig len : %d\n", sig_len);
		goto clean_up;
	}

	// Dump buffer for debugging
	_print_sha256(sha256, "SHA-256 for current firmware: ");

	// Get the message digest info structure for SHA256
	if (_crypto_sha256(sha256, OTA_CRYPTO_SHA256_LEN, hash) != 0) {
		printf("invalid digest \n");
		goto clean_up;
	}

	// check signature's header
	if (sig_data[0] != 0xff || sig_data[1] != 0xff ||
		sig_data[2] != 0xff || sig_data[3] != 0xff) {
		printf("invalid signature header \n");
		goto clean_up;
	}

	// check signature's footer
	if (sig_data[OTA_DEFAULT_SIGNATURE_BUF_SIZE - 1] != 0xff || sig_data[OTA_DEFAULT_SIGNATURE_BUF_SIZE - 2] != 0xff ||
		sig_data[OTA_DEFAULT_SIGNATURE_BUF_SIZE - 3] != 0xff || sig_data[OTA_DEFAULT_SIGNATURE_BUF_SIZE - 4] != 0xff) {
		printf("Invalid footer \n");
		goto clean_up;
	}

	unsigned char sig[OTA_SIGNATURE_SIZE] = {0,};
	memcpy(sig, sig_data + OTA_SIGNATURE_PREFACE_SIZE, OTA_SIGNATURE_SIZE);

	if (_pk_verify((const unsigned char *)sig, hash) != 0) {
		printf("_pk_verify is failed \n");
	} else {
		ret = true;
	}

clean_up:

	return ret;
}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	switch(evt->event_id) {
		case HTTP_EVENT_ERROR:
			printf("HTTP_EVENT_ERROR\n");
			break;
		default:
			break;
	}
	return ESP_OK;
}

static void _http_cleanup(esp_http_client_handle_t client)
{
	esp_http_client_close(client);
	esp_http_client_cleanup(client);
}

static ota_err_t _update_device()
{
	printf("_update_device \n");

	ota_err_t ret = OTA_FAIL;
	esp_err_t result = ESP_FAIL;

	unsigned int content_len;
	unsigned int firmware_len;

	esp_http_client_config_t config = {
		.url = CONFIG_FIRMWARE_UPGRADE_URL,
		.cert_pem = (char *)root_pem_start,
		.event_handler = _http_event_handler,
	};

	mbedtls_sha256_context ctx;
	mbedtls_sha256_init( &ctx );
	if (mbedtls_sha256_starts_ret( &ctx, 0) != 0 ) {
		printf("Failed to initialise api \n");
		return ret;
	}

	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL) {
		printf("Failed to initialise HTTP connection\n");
		return ret;
	}

	if (esp_http_client_get_transport_type(client) != HTTP_TRANSPORT_OVER_SSL) {
		printf("Transport is not over HTTPS\n");
		return ret;
	}

	result = esp_http_client_open(client, 0);
	if (result != ESP_OK) {
		esp_http_client_cleanup(client);
		printf("Failed to open HTTP connection: %d \n", result);
		return ret;
	}
	content_len = esp_http_client_fetch_headers(client);
	if (content_len <= OTA_DEFAULT_SIGNATURE_BUF_SIZE) {
		printf("content size error \n");
		_http_cleanup(client);
		return ret;
	}

	firmware_len = content_len - (OTA_DEFAULT_SIGNATURE_BUF_SIZE);

	esp_ota_handle_t update_handle = 0;
	const esp_partition_t *update_partition = NULL;
	printf("Starting OTA...\n");
	update_partition = esp_ota_get_next_update_partition(NULL);
	if (update_partition == NULL) {
		printf("Passive OTA partition not found\n");
		_http_cleanup(client);
		return ret;
	}
	printf("Writing to partition subtype %d at offset 0x%x \n",
			 update_partition->subtype, update_partition->address);

	result = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
	if (result != ESP_OK) {
		printf("esp_ota_begin failed, error=%d", result);
		_http_cleanup(client);
		return ret;
	}
	printf("esp_ota_begin succeeded\n Please Wait. This may take time\n");

	esp_err_t ota_write_err = ESP_OK;
	char *upgrade_data_buf = (char *)malloc(OTA_DEFAULT_BUF_SIZE);
	if (!upgrade_data_buf) {
		printf("Couldn't allocate memory to upgrade data buffer\n");
		_http_cleanup(client);
		return ESP_ERR_NO_MEM;
	}
	unsigned char *sig_ptr = NULL;
	unsigned char *sig = NULL;
	unsigned int sig_len = 0;
	unsigned int total_read_len = 0;
	unsigned int remain_len = 0;
	unsigned int excess_len = 0;

	sig = (unsigned char *)malloc(OTA_DEFAULT_SIGNATURE_BUF_SIZE);
	if (!sig) {
		printf("Couldn't allocate memory to add sig buffer\n");
		free(upgrade_data_buf);
		_http_cleanup(client);
		return OTA_ERR_NO_MEM;
	}
	memset(sig, '\0', OTA_DEFAULT_SIGNATURE_BUF_SIZE);

	sig_ptr = sig;

	while (1) {
		int data_read = esp_http_client_read(client, upgrade_data_buf, OTA_DEFAULT_BUF_SIZE);
		if (data_read == 0) {
			printf("Connection closed,all data received\n");
			break;
		}
		if (data_read < 0) {
			printf("Error: SSL data read error\n");
			break;
		}

		// checking data to get signature info
		if (data_read + total_read_len > firmware_len) {

			remain_len = firmware_len - total_read_len;
			excess_len = data_read - remain_len;

			if (sig_len + excess_len > OTA_DEFAULT_SIGNATURE_BUF_SIZE) {
				printf("Invalid signature len : %d, %d\n", sig_len, excess_len);
				break;
			}

			memcpy(sig_ptr, upgrade_data_buf + remain_len, excess_len);
			sig_ptr += excess_len;
			sig_len += excess_len;

			/* Change value to write firmware data */
			data_read = remain_len;
		}

		if (data_read > 0) {
			if (mbedtls_sha256_update_ret(&ctx, (const unsigned char *)upgrade_data_buf, data_read) != 0) {
				printf("Failed getting HASH \n");
			}

			ota_write_err = esp_ota_write( update_handle, (const void *)upgrade_data_buf, data_read);
			if (ota_write_err != ESP_OK) {
				break;
			}

			total_read_len += data_read;
		}
	}

	printf("Total binary data length writen: %d\n", total_read_len);

	unsigned char md[OTA_CRYPTO_SHA256_LEN] = {0,};

	if (mbedtls_sha256_finish_ret( &ctx, md) != 0) {
		printf("Failed getting HASH \n");
		goto clean_up;
	}
	mbedtls_sha256_free(&ctx);

	/* Check firmware validation */
	if (_check_firmware_validation((const unsigned char *)md, sig, sig_len) != true) {
		printf("Signature verified NOK\n");
		goto clean_up;
	}

	result = esp_ota_end(update_handle);
	if (ota_write_err != ESP_OK) {
		printf("esp_ota_write failed! err=0x%d\n", ota_write_err);
		goto clean_up;
   	} else if (result != ESP_OK) {
		printf("esp_ota_end failed! err=0x%d. Image is invalid\n", result);
		goto clean_up;
	}

	result = esp_ota_set_boot_partition(update_partition);
	if (result != ESP_OK) {
		printf("esp_ota_set_boot_partition failed! err=0x%d\n", result);
		goto clean_up;
	}

	printf("esp_ota_set_boot_partition succeeded\n");

    ret = OTA_OK;

clean_up:

	if (sig)
		free(sig);

	if (upgrade_data_buf)
		free(upgrade_data_buf);

	_http_cleanup(client);

	return ret;
}


#define OTA_VERSION_INFO_BUF_SIZE 2048

static ota_err_t _read_version_info_from_server(char **version_info, unsigned int *version_info_len)
{
	ota_err_t ret = OTA_FAIL;

	esp_http_client_config_t config = {
		.url = CONFIG_FIRMWARE_VERSOIN_INFO_URL,
		.cert_pem = (char *)root_pem_start,
		.event_handler = _http_event_handler,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	if (client == NULL) {
		printf("Failed to initialise HTTP connection\n");
		return ESP_FAIL;
	}

	if (esp_http_client_get_transport_type(client) != HTTP_TRANSPORT_OVER_SSL) {
		printf("Transport is not over HTTPS\n");
		return ESP_FAIL;
	}

	ret = esp_http_client_open(client, 0);
	if (ret != ESP_OK) {
		esp_http_client_cleanup(client);
		printf("Failed to open HTTP connection: %d \n", ret);
		return OTA_FAIL;
	}
	esp_http_client_fetch_headers(client);

	char *upgrade_data_buf = (char *)malloc(OTA_DEFAULT_BUF_SIZE);
	if (!upgrade_data_buf) {
		esp_http_client_cleanup(client);
		printf("Couldn't allocate memory to upgrade data buffer\n");
		return OTA_ERR_NO_MEM;
	}

	unsigned int total_read_len = 0;

	char *read_data_buf = (char *)malloc(OTA_VERSION_INFO_BUF_SIZE);
	if (!read_data_buf) {
		esp_http_client_cleanup(client);
		free(upgrade_data_buf);
		printf("Couldn't allocate memory to read data buffer\n");
		return OTA_ERR_NO_MEM;
	}
	memset(read_data_buf, '\0', OTA_VERSION_INFO_BUF_SIZE);
	char *ptr = read_data_buf;

	while (1) {
		int data_read = esp_http_client_read(client, upgrade_data_buf, OTA_DEFAULT_BUF_SIZE);
		if (data_read == 0) {
			printf("Connection closed,all data received\n");
			break;
		}
		if (data_read < 0) {
			printf("Error: SSL data read error\n");
			break;
		}
		if (data_read > 0) {
			total_read_len += data_read;

			if (total_read_len >= OTA_VERSION_INFO_BUF_SIZE) {
				printf("Max length of data is exceeded. \n");
				break;
			}

			memcpy(ptr, upgrade_data_buf, data_read);
			ptr += data_read;
		}
	}

	_http_cleanup(client);

	free(upgrade_data_buf);

	if (total_read_len == 0) {
		printf("read error\n");
		free(read_data_buf);
		return OTA_ERR_INVALID_SIZE;
	}

	*version_info = read_data_buf;
	*version_info_len = total_read_len;

    ret = OTA_OK;

	return ret;
}

void ota_check_for_update(void *user_data)
{
    caps_firmwareUpdate_data_t *caps_data = (caps_firmwareUpdate_data_t *)user_data;

    if (!caps_data || !caps_data->currentVersion_value) {
        printf("Data is NULL.. \n");
        return;
    }

    char *read_data = NULL;
    unsigned int read_data_len = 0;

    ota_err_t ret = _read_version_info_from_server(&read_data, &read_data_len);
    if (ret == OTA_OK) {
        char *available_version = NULL;
        char* current_version = caps_data->currentVersion_value;

        ret = _get_available_version(read_data, read_data_len, current_version, &available_version);

        if (read_data)
            free(read_data);

        if (ret != OTA_OK) {
            printf("Cannot find new version: %d\n", ret);
            return;
        }

        if (available_version) {
            caps_data->set_availableVersion_value(caps_data, available_version);
            caps_data->attr_availableVersion_send(caps_data);

            free(available_version);
        }
    }
}

void ota_restart_device()
{
    esp_restart();
}

ota_err_t ota_update_device()
{
    ota_err_t ret = OTA_OK;

    ret = _update_device();

    return ret;
}
