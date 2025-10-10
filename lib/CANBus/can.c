/***************************************************************
 * can.c
 * CAN Bus library implementation for ESP32 with MCP2551 transceiver
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "can.h"
#include "driver/gpio.h"
#include "driver/twai.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

static const char *TAG = "CAN";

/* TWAI handle */
static bool can_initialized = false;

/* Predefined CAN configurations */
const can_config_t CAN_CONFIG_500KBPS = {
    .baudrate = 500000,
    .loopback = false,
    .silent = false
};

const can_config_t CAN_CONFIG_250KBPS = {
    .baudrate = 250000,
    .loopback = false,
    .silent = false
};

const can_config_t CAN_CONFIG_125KBPS = {
    .baudrate = 125000,
    .loopback = false,
    .silent = false
};

can_status_t can_init(const can_config_t *config)
{
    if (can_initialized) {
        ESP_LOGW(TAG, "CAN already initialized");
        return CAN_OK;
    }

    if (!config) {
        ESP_LOGE(TAG, "Invalid CAN configuration");
        return CAN_ERROR_INIT;
    }

    /* TWAI timing configuration */
    twai_timing_config_t timing_config = TWAI_TIMING_CONFIG_500KBITS();

    /* TWAI filter configuration */
    twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    /* TWAI general configuration */
    twai_general_config_t general_config = TWAI_GENERAL_CONFIG_DEFAULT(
        VDU_CAN_TX, VDU_CAN_RX, TWAI_MODE_NORMAL
    );

    /* Set loopback mode if requested */
    if (config->loopback) {
        general_config.mode = TWAI_MODE_NO_ACK;
        ESP_LOGW(TAG, "TWAI loopback mode enabled");
    }

    /* Set silent mode if requested */
    if (config->silent) {
        general_config.mode = TWAI_MODE_LISTEN_ONLY;
        ESP_LOGW(TAG, "TWAI silent mode enabled");
    }

    /* Install TWAI driver */
    esp_err_t ret = twai_driver_install(&general_config, &timing_config, &filter_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install TWAI driver: %s", esp_err_to_name(ret));
        return CAN_ERROR_INIT;
    }

    /* Start TWAI driver */
    ret = twai_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start TWAI driver: %s", esp_err_to_name(ret));
        twai_driver_uninstall();
        return CAN_ERROR_INIT;
    }

    can_initialized = true;
    ESP_LOGI(TAG, "CAN initialized successfully at %lu bps", (unsigned long)config->baudrate);
    return CAN_OK;
}

can_status_t can_init_default(void)
{
    return can_init(&CAN_CONFIG_500KBPS);
}

void can_deinit(void)
{
    if (!can_initialized) {
        return;
    }

    /* Stop TWAI driver */
    twai_stop();
    
    /* Uninstall TWAI driver */
    twai_driver_uninstall();
    
    can_initialized = false;
    ESP_LOGI(TAG, "CAN deinitialized");
}

can_status_t can_send(const can_message_t *msg)
{
    if (!can_initialized) {
        ESP_LOGE(TAG, "CAN not initialized");
        return CAN_ERROR_INIT;
    }

    if (!msg) {
        ESP_LOGE(TAG, "Invalid message pointer");
        return CAN_ERROR_TX;
    }

    /* Prepare TWAI message */
    twai_message_t esp_msg = {0};
    esp_msg.identifier = msg->id;
    esp_msg.data_length_code = msg->length;
    esp_msg.flags = 0;
    if (msg->is_extended) esp_msg.flags |= TWAI_MSG_FLAG_EXTD;
    if (msg->is_remote) esp_msg.flags |= TWAI_MSG_FLAG_RTR;
    
    /* Copy data */
    for (int i = 0; i < msg->length && i < 8; i++) {
        esp_msg.data[i] = msg->data[i];
    }

    /* Send message */
    esp_err_t ret = twai_transmit(&esp_msg, pdMS_TO_TICKS(100));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send TWAI message: %s", esp_err_to_name(ret));
        return CAN_ERROR_TX;
    }

    ESP_LOGD(TAG, "CAN message sent: ID=0x%03lX, Length=%d", (unsigned long)msg->id, msg->length);
    return CAN_OK;
}

can_status_t can_receive(can_message_t *msg, uint32_t timeout_ms)
{
    if (!can_initialized) {
        ESP_LOGE(TAG, "CAN not initialized");
        return CAN_ERROR_INIT;
    }

    if (!msg) {
        ESP_LOGE(TAG, "Invalid message pointer");
        return CAN_ERROR_RX;
    }

    /* Receive message */
    twai_message_t esp_msg;
    esp_err_t ret = twai_receive(&esp_msg, pdMS_TO_TICKS(timeout_ms));
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_TIMEOUT) {
            return CAN_ERROR_TIMEOUT;
        }
        ESP_LOGE(TAG, "Failed to receive TWAI message: %s", esp_err_to_name(ret));
        return CAN_ERROR_RX;
    }

    /* Convert to our message format */
    msg->id = esp_msg.identifier;
    msg->length = esp_msg.data_length_code;
    msg->is_extended = (esp_msg.flags & TWAI_MSG_FLAG_EXTD) != 0;
    msg->is_remote = (esp_msg.flags & TWAI_MSG_FLAG_RTR) != 0;
    
    /* Copy data */
    for (int i = 0; i < msg->length && i < 8; i++) {
        msg->data[i] = esp_msg.data[i];
    }

    ESP_LOGD(TAG, "CAN message received: ID=0x%03lX, Length=%d", (unsigned long)msg->id, msg->length);
    return CAN_OK;
}

bool can_message_available(void)
{
    if (!can_initialized) {
        return false;
    }

    /* Check if there are pending messages */
    uint32_t alerts;
    esp_err_t ret = twai_read_alerts(&alerts, 0);
    if (ret == ESP_OK && (alerts & TWAI_ALERT_RX_DATA)) {
        return true;
    }

    return false;
}

can_status_t can_get_status(void)
{
    if (!can_initialized) {
        return CAN_ERROR_INIT;
    }

    /* Read TWAI alerts */
    uint32_t alerts;
    esp_err_t ret = twai_read_alerts(&alerts, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read TWAI alerts: %s", esp_err_to_name(ret));
        return CAN_ERROR_BUSY;
    }

    /* Check for error conditions */
    if (alerts & TWAI_ALERT_BUS_ERROR) {
        ESP_LOGW(TAG, "TWAI bus error detected");
        return CAN_ERROR_BUSY;
    }

    if (alerts & TWAI_ALERT_ARB_LOST) {
        ESP_LOGW(TAG, "TWAI arbitration lost");
        return CAN_ERROR_BUSY;
    }

    if (alerts & TWAI_ALERT_TX_FAILED) {
        ESP_LOGW(TAG, "TWAI transmission failed");
        return CAN_ERROR_TX;
    }

    return CAN_OK;
}

can_status_t can_set_filter(uint32_t id, uint32_t mask, bool is_extended)
{
    if (!can_initialized) {
        ESP_LOGE(TAG, "CAN not initialized");
        return CAN_ERROR_INIT;
    }

    /* Stop TWAI driver to modify filter */
    twai_stop();

    /* Configure filter */
    twai_filter_config_t filter_config = {
        .acceptance_code = id << 21,  /* Shift ID to correct position */
        .acceptance_mask = mask << 21,
        .single_filter = true
    };

    /* Reinstall filter */
    esp_err_t ret = twai_driver_install(NULL, NULL, &filter_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set TWAI filter: %s", esp_err_to_name(ret));
        return CAN_ERROR_INIT;
    }

    /* Restart TWAI driver */
    ret = twai_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to restart TWAI after filter change: %s", esp_err_to_name(ret));
        return CAN_ERROR_INIT;
    }

    ESP_LOGI(TAG, "CAN filter set: ID=0x%03lX, Mask=0x%03lX", (unsigned long)id, (unsigned long)mask);
    return CAN_OK;
}

can_status_t can_clear_filter(void)
{
    if (!can_initialized) {
        ESP_LOGE(TAG, "CAN not initialized");
        return CAN_ERROR_INIT;
    }

    /* Stop TWAI driver */
    twai_stop();

    /* Configure filter to accept all */
    twai_filter_config_t filter_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    /* Reinstall filter */
    esp_err_t ret = twai_driver_install(NULL, NULL, &filter_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to clear TWAI filter: %s", esp_err_to_name(ret));
        return CAN_ERROR_INIT;
    }

    /* Restart TWAI driver */
    ret = twai_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to restart TWAI after filter clear: %s", esp_err_to_name(ret));
        return CAN_ERROR_INIT;
    }

    ESP_LOGI(TAG, "CAN filter cleared - accepting all messages");
    return CAN_OK;
} 