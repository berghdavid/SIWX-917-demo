/***************************************************************************/ /**
 * @file
 * @brief Wi-Fi Provisioning via Access Point Example Application
 *******************************************************************************
 * # License
 * <b>Copyright 2024 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <string.h>

#include "app.h"
#include "cacert.pem.h"
#include "errno.h"
#include "jsmn.h"
#include "login.h"
#include "provisioning.h"
#include "sl_mqtt_client.h"
#include "sl_net.h"
#include "sl_net_si91x.h"
#include "sl_net_wifi_types.h"
#include "sl_si91x_socket.h"
#include "sl_si91x_socket_constants.h"
#include "sl_si91x_socket_support.h"
#include "sl_utility.h"
#include "sl_wifi.h"
#include "sl_wifi_callback_framework.h"
#include "wifi_provisioning.h"

/******************************************************
 *               Function Declarations
 ******************************************************/
//Application start API
static void application_start(void *argument);
void mqtt_client_event_handler(void *client, sl_mqtt_client_event_t event, void *event_data, void *context);
void mqtt_client_error_event_handler(void *client, sl_mqtt_client_error_status_t *error);
void mqtt_client_cleanup();
sl_status_t mqtt_example();

/******************************************************
 *               Variable Definitions
 ******************************************************/

static int scan_complete           = 0;
static sl_status_t callback_status = SL_STATUS_OK;

char wifi_client_profile_ssid[32]; // Assuming SSID can be up to 32 characters long
char wifi_client_credential[64];   // Assuming Password can be up to 64 characters long
char wifi_client_security_type[32];

static const osThreadAttr_t thread_attributes = {
  .name       = "app",
  .attr_bits  = 0,
  .cb_mem     = 0,
  .cb_size    = 0,
  .stack_mem  = 0,
  .stack_size = 3072,
  .priority   = osPriorityLow,
  .tz_module  = 0,
  .reserved   = 0,
};


/******************************************************
 *                 MQTT Definitions
 ******************************************************/
#define SCAN_RESULT_BUFFER_SIZE (1200)
#define SCAN_INTERVAL_MS (1000)
#define SCAN_TIMEOUT_MS 10000

#ifdef SLI_SI91X_ENABLE_IPV6
#define MQTT_BROKER_IP "2401:4901:1290:10de::1000"
#else
#define MQTT_BROKER_IP "52.48.71.218"
#endif

#define MQTT_BROKER_PORT 1883

#define CLIENT_PORT 1

#define CLIENT_ID "WISECONNECT-SDK-MQTT-CLIENT-ID"

#define SERVICE_TOKEN          "test"
#define PUBLISH_TOPIC          "tlc/v2/raw-data/service/%s/device/%s"
#define QOS_OF_PUBLISH_MESSAGE 0

#define IS_DUPLICATE_MESSAGE 0
#define IS_MESSAGE_RETAINED  1
#define IS_CLEAN_SESSION     1

#define LAST_WILL_TOPIC       "SiLabs last will"
#define LAST_WILL_MESSAGE     "SiLabs device has been disconnect from network"
#define QOS_OF_LAST_WILL      1
#define IS_LAST_WILL_RETAINED 1

#define ENCRYPT_CONNECTION     0
#define CERTIFICATE_INDEX      0
#define KEEP_ALIVE_INTERVAL    2000
#define MQTT_CONNECT_TIMEOUT   5000
#define MQTT_KEEPALIVE_RETRIES 0

#define SEND_CREDENTIALS 1

#define USERNAME "silabs"
#define PASSWORD "password"

unsigned int req_nbr = 0;
char device_id[18];
sl_mqtt_client_t client = { 0 };
sl_mqtt_client_credentials_t *client_credentails = NULL;
uint8_t is_execution_completed = 0;

sl_mqtt_client_configuration_t mqtt_client_configuration = { .is_clean_session = IS_CLEAN_SESSION,
                                                             .client_id        = (uint8_t *)CLIENT_ID,
                                                             .client_id_length = strlen(CLIENT_ID),
#if ENCRYPT_CONNECTION
                                                             .tls_flags = SL_MQTT_TLS_ENABLE | SL_MQTT_TLS_TLSV_1_2
                                                                          | SL_MQTT_TLS_CERT_INDEX_1,
#endif
                                                             .client_port = CLIENT_PORT };

static const sl_wifi_device_configuration_t wifi_mqtt_client_configuration = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .boot_config = { .oper_mode              = SL_SI91X_CLIENT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_CLIENT | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                              | SL_SI91X_TCP_IP_FEAT_SSL | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID
#ifdef SLI_SI91X_ENABLE_IPV6
                                              | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                                              ),
                   .custom_feature_bit_map     = (SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID),
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_SSL_VERSIONS_SUPPORT | SL_SI91X_EXT_FEAT_XTAL_CLK
                                                  | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)
                      | SL_SI91X_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS | SL_SI91X_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD
                      | SL_SI91X_EXT_EMB_MQTT_ENABLE),
                   .ble_feature_bit_map     = 0,
                   .ble_ext_feature_bit_map = 0,
                   .config_feature_bit_map  = 0 }
};

const sl_wifi_device_configuration_t ap_config = {
  .boot_option = LOAD_NWP_FW,
  .mac_address = NULL,
  .band        = SL_SI91X_WIFI_BAND_2_4GHZ,
  .region_code = US,
  .boot_config = { .oper_mode              = SL_SI91X_ACCESS_POINT_MODE,
                   .coex_mode              = SL_SI91X_WLAN_ONLY_MODE,
                   .feature_bit_map        = (SL_SI91X_FEAT_SECURITY_PSK | SL_SI91X_FEAT_AGGREGATION),
                   .tcp_ip_feature_bit_map = (SL_SI91X_TCP_IP_FEAT_DHCPV4_SERVER | SL_SI91X_TCP_IP_FEAT_DNS_CLIENT
                                             | SL_SI91X_TCP_IP_FEAT_SSL | SL_SI91X_TCP_IP_FEAT_EXTENSION_VALID
#ifdef SLI_SI91X_ENABLE_IPV6
                                             | SL_SI91X_TCP_IP_FEAT_DHCPV6_CLIENT | SL_SI91X_TCP_IP_FEAT_IPV6
#endif
                                             ),
                   .custom_feature_bit_map     = SL_SI91X_CUSTOM_FEAT_EXTENTION_VALID,
                   .ext_custom_feature_bit_map = (SL_SI91X_EXT_FEAT_SSL_VERSIONS_SUPPORT | SL_SI91X_EXT_FEAT_XTAL_CLK
                       | SL_SI91X_EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS | MEMORY_CONFIG
#if defined(SLI_SI917) || defined(SLI_SI915)
                                                  | SL_SI91X_EXT_FEAT_FRONT_END_SWITCH_PINS_ULP_GPIO_4_5_0
#endif
                                                  ),
                   .bt_feature_bit_map         = 0,
                   .ext_tcp_ip_feature_bit_map =
                     (SL_SI91X_EXT_TCP_IP_WINDOW_SCALING | SL_SI91X_EXT_TCP_IP_TOTAL_SELECTS(10)
                      | SL_SI91X_EXT_TCP_IP_FEAT_SSL_THREE_SOCKETS | SL_SI91X_EXT_TCP_IP_FEAT_SSL_MEMORY_CLOUD
                      | SL_SI91X_EXT_EMB_MQTT_ENABLE),
                   .ble_feature_bit_map        = 0,
                   .ble_ext_feature_bit_map    = 0,
                   .config_feature_bit_map     = 0 }
};



sl_mqtt_broker_t mqtt_broker_configuration = {
  .port                    = MQTT_BROKER_PORT,
  .is_connection_encrypted = ENCRYPT_CONNECTION,
  .connect_timeout         = MQTT_CONNECT_TIMEOUT,
  .keep_alive_interval     = KEEP_ALIVE_INTERVAL,
  .keep_alive_retries      = MQTT_KEEPALIVE_RETRIES,
};

sl_mqtt_client_message_t message_to_be_published = {
  .qos_level            = QOS_OF_PUBLISH_MESSAGE,
  .is_retained          = IS_MESSAGE_RETAINED,
  .is_duplicate_message = IS_DUPLICATE_MESSAGE,
  .topic                = (uint8_t *)PUBLISH_TOPIC,
  .topic_length         = strlen(PUBLISH_TOPIC),
  .content              = NULL,
  .content_length       = 0,
};

sl_mqtt_client_last_will_message_t last_will_message = {
  .is_retained         = IS_LAST_WILL_RETAINED,
  .will_qos_level      = QOS_OF_LAST_WILL,
  .will_topic          = (uint8_t *)LAST_WILL_TOPIC,
  .will_topic_length   = strlen(LAST_WILL_TOPIC),
  .will_message        = (uint8_t *)LAST_WILL_MESSAGE,
  .will_message_length = strlen(LAST_WILL_MESSAGE),
};

/******************************************************
 *               Function Definitions
 ******************************************************/

void sleep()
{
  while (1) {
    #if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
        // Let the CPU go to sleep if the system allows it.
        sl_power_manager_sleep();
    #else
        osDelay(osWaitForever);
    #endif
  }
}

static int init_up_net(sl_net_interface_t *net_interface,
                    sl_net_profile_id_t *profile,
                    const sl_wifi_device_configuration_t *config)
{
  sl_status_t status = sl_net_init(*net_interface, (const void *)config, NULL, NULL);
  if (status == SL_STATUS_ALREADY_INITIALIZED) {
    printf("Reusing previous Wi-Fi interface. Wi-Fi interface up\r\n");
    return 0;
  } else if (status != SL_STATUS_OK) {
    printf("Failed to init Wi-Fi interface: 0x%lx\r\n", status);
    return -1;
  }
  printf("Wi-Fi interface initialized\r\n");

  status = sl_net_up(*net_interface, *profile);
  if (status != SL_STATUS_OK) {
    printf("Failed to bring Wi-Fi interface up: 0x%lx\r\n", status);
    return -1;
  }
  printf("Wi-Fi interface up\r\n");
  return 0;
}

static int deinit_net(sl_net_interface_t *net_interface)
{
  sl_status_t status;
  status = sl_net_down(*net_interface);
  if (status != SL_STATUS_OK) {
    printf("Network down failed: 0x%lx\r\n", status);
    return -1;
  }

  // FIXME: AP network interface stuck on deinitialization.
  printf("Attempting network deinit...\r\n");
  status = sl_net_deinit(*net_interface);
  if (status != SL_STATUS_OK) {
    printf("Network deinit failed: 0x%lx\r\n", status);
    return -1;
  }
  printf("Network deinitialized\r\n");
  return 0;
}

/******************************************************
 *                    Wi-Fi Scan Functions
 ******************************************************/

static sl_status_t wlan_app_scan_callback_handler(sl_wifi_event_t event,
                                                  sl_wifi_scan_result_t *scan_result,
                                                  uint32_t result_length,
                                                  void *arg)
{
  UNUSED_PARAMETER(result_length);
  char *scan_result_buffer = (char *)arg;
  uint8_t *bssid           = NULL;

  // Check if the scan event indicates failure
  if (SL_WIFI_CHECK_IF_EVENT_FAILED(event)) {
    callback_status = *(sl_status_t *)scan_result;
    scan_complete = 1; // Indicate that the scan is complete
    return SL_STATUS_FAIL;
  }

  if (scan_result->scan_count) {
    uint32_t buffer_length = SCAN_RESULT_BUFFER_SIZE - 1;
    int32_t index = snprintf(
      scan_result_buffer,
      buffer_length,
      "{\"servicetoken\": \"%s\", \"id\": \"%s\", \"nr\": %u, \"wifiAccessPoints\": [",
      SERVICE_TOKEN,
      device_id,
      req_nbr++
    );
    scan_result_buffer += index;
    buffer_length -= index;

    // Iterate through scan results and format them as JSON
    // Note that one wifi object can maximally use 104 bytes
    for (uint32_t a = 0; a < scan_result->scan_count && buffer_length > 104; a++) {
      bssid = (uint8_t *)&scan_result->scan_info[a].bssid;
      index = snprintf(
        scan_result_buffer,
        buffer_length,
        "{\"macAdress\": \"%02x:%02x:%02x:%02x:%02x:%02x\", \"ssid\": \"%s\", \"signalStrength\": -%u}",
        bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5],
        scan_result->scan_info[a].ssid,
        scan_result->scan_info[a].rssi_val
      );
      scan_result_buffer += index;
      buffer_length -= index;
      if (a < scan_result->scan_count - 1) {
        index = snprintf(scan_result_buffer, buffer_length, ", ");
        scan_result_buffer += index;
        buffer_length -= index;
      }
    }
    index = snprintf(scan_result_buffer, buffer_length, "]}");
    scan_result_buffer += index;
    buffer_length -= index;
  }

  scan_complete = 1; // Indicate that the scan is complete
  return SL_STATUS_OK;
}

static void read_mac_address(sl_wifi_interface_t *wifi_interface)
{
  sl_mac_address_t mac;
  char *pos = &device_id[0];
  int n;

  sl_status_t status = sl_wifi_get_mac_address(*wifi_interface, &mac);
  if (status != SL_STATUS_OK) {
    printf("Failed to get mac address: 0x%lx\r\n", status);
    return;
  }
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
        n = sprintf(pos, ":");
        pos += n;
    }
    n = sprintf(pos, "%02X", mac.octet[i]);
    pos += n;
  }
}

static void perform_wifi_scan(char *scan_result_buffer)
{
  sl_wifi_interface_t wifi_interface = SL_WIFI_AP_INTERFACE;
  scan_complete = 0;
  scan_result_buffer[0] = '\0';
  if (device_id[0] == '\0') {
    read_mac_address(&wifi_interface);
  }

  sl_wifi_set_scan_callback(wlan_app_scan_callback_handler, (void *)scan_result_buffer);
  sl_status_t status = sl_wifi_start_scan(wifi_interface, NULL, &default_wifi_scan_configuration);
  if (SL_STATUS_IN_PROGRESS == status) {
    // Wait for scan completion or timeout
    uint32_t end = osKernelGetTickCount() + SCAN_TIMEOUT_MS;
    while (!scan_complete && osKernelGetTickCount() < end) {
      osThreadYield();
    }
    status = scan_complete ? callback_status : SL_STATUS_TIMEOUT;
  }

  if (status != RSI_SUCCESS) {
    printf("WLAN Scan failed %lx. Please make sure the latest connectivity firmware is used.\r\n", status);
  }
  printf("Scan: %s\r\n", scan_result_buffer);
}

static void get_rss_scan(char *buf)
{
  sl_net_interface_t net_interface            = SL_NET_WIFI_AP_INTERFACE;
  sl_net_profile_id_t profile                 = SL_NET_DEFAULT_WIFI_AP_PROFILE_ID;
  const sl_wifi_device_configuration_t config = ap_config;

  if (init_up_net(&net_interface, &profile, &config) == 0) {
    perform_wifi_scan(buf);
    deinit_net(&net_interface);
  }
}

/******************************************************
 *                    MQTT Functions
 ******************************************************/

void mqtt_client_cleanup()
{
  SL_CLEANUP_MALLOC(client_credentails);
  client_credentails = NULL;
  is_execution_completed = 1;
}

void mqtt_client_error_event_handler(void *client, sl_mqtt_client_error_status_t *error)
{
  UNUSED_PARAMETER(client);
  printf("Terminating program, Error: %d\r\n", *error);
  mqtt_client_cleanup();
}

void mqtt_client_event_handler(void *client, sl_mqtt_client_event_t event, void *event_data, void *context)
{
  switch (event) {
    case SL_MQTT_CLIENT_CONNECTED_EVENT: {
      sl_status_t status = sl_mqtt_client_publish(client, &message_to_be_published, 0, &message_to_be_published);
      if (status != SL_STATUS_IN_PROGRESS) {
        printf("Failed to publish message: 0x%lx\r\n", status);
        mqtt_client_cleanup();
        return;
      }
      break;
    }

    case SL_MQTT_CLIENT_MESSAGE_PUBLISHED_EVENT: {
      sl_mqtt_client_message_t *published_message = (sl_mqtt_client_message_t *)context;
      printf("Published message successfully on topic: %.*s\r\n",
             published_message->topic_length,
             (char *)published_message->topic);
      sl_mqtt_client_disconnect(client, 0);
      break;
    }

    case SL_MQTT_CLIENT_DISCONNECTED_EVENT: {
      printf("Disconnected from MQTT broker\r\n");
      mqtt_client_cleanup();
      break;
    }

    case SL_MQTT_CLIENT_ERROR_EVENT: {
      mqtt_client_error_event_handler(client, (sl_mqtt_client_error_status_t *)event_data);
      break;
    }

    default:
      break;
  }
}

sl_status_t mqtt_example()
{
  sl_status_t status;
  printf("Sending '%s'\r\n", message_to_be_published.content);

  if (ENCRYPT_CONNECTION) {
    // Load SSL CA certificate
    status = sl_net_set_credential(SL_NET_TLS_SERVER_CREDENTIAL_ID(CERTIFICATE_INDEX),
                                   SL_NET_SIGNING_CERTIFICATE,
                                   cacert,
                                   sizeof(cacert) - 1);
    if (status != SL_STATUS_OK) {
      printf("Loading TLS CA certificate in to FLASH Failed, Error Code : 0x%lX\r\n", status);
      return status;
    }
    printf("Load TLS CA certificate at index %d Success\r\n", 0);
  }

  if (SEND_CREDENTIALS) {
    uint16_t username_length, password_length;

    username_length = strlen(USERNAME);
    password_length = strlen(PASSWORD);

    uint32_t malloc_size = sizeof(sl_mqtt_client_credentials_t) + username_length + password_length;

    client_credentails = malloc(malloc_size);
    if (client_credentails == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
    memset(client_credentails, 0, malloc_size);
    client_credentails->username_length = username_length;
    client_credentails->password_length = password_length;

    memcpy(&client_credentails->data[0], USERNAME, username_length);
    memcpy(&client_credentails->data[username_length], PASSWORD, password_length);

    status = sl_net_set_credential(SL_NET_MQTT_CLIENT_CREDENTIAL_ID(0),
                                   SL_NET_MQTT_CLIENT_CREDENTIAL,
                                   client_credentails,
                                   malloc_size);

    if (status != SL_STATUS_OK) {
      mqtt_client_cleanup();
      printf("Failed to set credentials: 0x%lx\r\n ", status);
      return status;
    }
    printf("Set credentials Success\r\n");

    free(client_credentails);
    mqtt_client_configuration.credential_id = SL_NET_MQTT_CLIENT_CREDENTIAL_ID(0);
  }

  status = sl_mqtt_client_init(&client, mqtt_client_event_handler);
  if (status != SL_STATUS_OK) {
    printf("Failed to init mqtt client: 0x%lx\r\n", status);

    mqtt_client_cleanup();
    return status;
  }
  printf("Init mqtt client Success\r\n");

#ifdef SLI_SI91X_ENABLE_IPV6
  unsigned char hex_addr[SL_IPV6_ADDRESS_LENGTH] = { 0 };
  status                                         = sl_inet_pton6(MQTT_BROKER_IP,
                         MQTT_BROKER_IP + strlen(MQTT_BROKER_IP),
                         hex_addr,
                         (unsigned int *)mqtt_broker_configuration.ip.ip.v6.value);
  if (status != 0x1) {
    printf("\r\nIPv6 conversion failed.\r\n");
    mqtt_client_cleanup();
    return status;
  }
  mqtt_broker_configuration.ip.type = SL_IPV6;
#else
  status = sl_net_inet_addr(MQTT_BROKER_IP, &mqtt_broker_configuration.ip.ip.v4.value);
  if (status != SL_STATUS_OK) {
    printf("Failed to convert IP address\r\n");

    mqtt_client_cleanup();
    return status;
  }
  mqtt_broker_configuration.ip.type = SL_IPV4;
#endif

  status =
    sl_mqtt_client_connect(&client, &mqtt_broker_configuration, &last_will_message, &mqtt_client_configuration, 0);
  if (status != SL_STATUS_IN_PROGRESS) {
    printf("Failed to connect to mqtt broker: 0x%lx\r\n", status);
    mqtt_client_cleanup();
    return status;
  }
  printf("MQTT connection in progress\r\n");
  while (!is_execution_completed) {
    osThreadYield();
  }

  return SL_STATUS_OK;
}

void send_mqtt_msg(const char *msg)
{
  sl_net_interface_t net_interface            = SL_NET_WIFI_CLIENT_INTERFACE;
  sl_net_profile_id_t profile                 = SL_NET_DEFAULT_WIFI_CLIENT_PROFILE_ID;
  const sl_wifi_device_configuration_t config = wifi_mqtt_client_configuration;

  is_execution_completed = 0;
  message_to_be_published.content = (uint8_t *)msg;
  message_to_be_published.content_length = strlen(msg);

  init_up_net(&net_interface, &profile, &config);
  mqtt_example();
  deinit_net(&net_interface);

  message_to_be_published.content = NULL;
  message_to_be_published.content_length = 0;
}

/******************************************************
 *                    Main Functions
 ******************************************************/

static void application_start(void *argument)
{
  UNUSED_PARAMETER(argument);
  char scan_buf[SCAN_RESULT_BUFFER_SIZE] = "Missing scan";

  printf("\r\nCombain demo started.\r\n");

  for (int i = 0; i < 2; i++) {
    get_rss_scan(scan_buf);
    send_mqtt_msg(scan_buf);
    osDelay(SCAN_INTERVAL_MS);
  }

  printf("Combain demo done.\r\n");
  sleep();
}

void app_init(void)
{
  osThreadNew((osThreadFunc_t)application_start, NULL, &thread_attributes);
}
