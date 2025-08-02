/***************************************************************
 * can_comm.c
 * CAN Communication module for VDU_ESP32
 * Handles CAN TX/RX with ID 0x301
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#include "can_comm.h"
#include "can.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <string.h>

static const char *TAG = "CAN_COMM";

/* CAN Communication handle */
static bool can_comm_initialized = false;
static TaskHandle_t can_rx_task_handle = NULL;
static QueueHandle_t can_rx_queue = NULL;

/* CAN ID for this VDU */
#define VDU_CAN_ID 0x301

/* CAN message structure for VDU */
typedef struct {
    uint8_t speed;         /* Vehicle speed (0-255 km/h) */
    uint8_t rpm_high;      /* Engine RPM high byte */
    uint8_t rpm_low;       /* Engine RPM low byte */
    uint8_t temp;          /* Engine temperature (0-255°C) */
    uint8_t fuel_level;    /* Fuel level (0-100%) */
    uint8_t status;        /* Status bits */
    uint8_t checksum;      /* Simple checksum */
    uint8_t reserved;      /* Reserved for future use */
} vdu_can_data_t;



/* Function prototypes */
static void can_rx_task(void *pvParameters);
static uint8_t calculate_checksum(const uint8_t *data, uint8_t length);
static void process_received_message(const can_rx_msg_t *msg);

can_comm_status_t can_comm_init(void)
{
    if (can_comm_initialized) {
        ESP_LOGW(TAG, "CAN communication already initialized");
        return CAN_COMM_OK;
    }

    /* Initialize CAN bus with default configuration (500 kbps) */
    can_status_t can_status = can_init_default();
    if (can_status != CAN_OK) {
        ESP_LOGE(TAG, "Failed to initialize CAN bus: %d", can_status);
        return CAN_COMM_ERROR_INIT;
    }

    /* Create queue for CAN RX messages */
    can_rx_queue = xQueueCreate(50, sizeof(can_rx_msg_t));
    if (can_rx_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create CAN RX queue");
        can_deinit();
        return CAN_COMM_ERROR_QUEUE;
    }

    /* Create CAN RX task */
    BaseType_t task_created = xTaskCreate(
        can_rx_task,
        "can_rx_task",
        4096,
        NULL,
        5,
        &can_rx_task_handle
    );

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to create CAN RX task");
        vQueueDelete(can_rx_queue);
        can_deinit();
        return CAN_COMM_ERROR_TASK;
    }

    can_comm_initialized = true;
    ESP_LOGI(TAG, "CAN communication initialized successfully");
    ESP_LOGI(TAG, "VDU CAN ID: 0x%03X", VDU_CAN_ID);
    
    return CAN_COMM_OK;
}

void can_comm_deinit(void)
{
    if (!can_comm_initialized) {
        return;
    }

    /* Delete RX task */
    if (can_rx_task_handle != NULL) {
        vTaskDelete(can_rx_task_handle);
        can_rx_task_handle = NULL;
    }

    /* Delete RX queue */
    if (can_rx_queue != NULL) {
        vQueueDelete(can_rx_queue);
        can_rx_queue = NULL;
    }

    /* Deinitialize CAN bus */
    can_deinit();
    
    can_comm_initialized = false;
    ESP_LOGI(TAG, "CAN communication deinitialized");
}

can_comm_status_t can_comm_send_vehicle_data(const vehicle_data_t *data)
{
    if (!can_comm_initialized) {
        ESP_LOGE(TAG, "CAN communication not initialized");
        return CAN_COMM_ERROR_NOT_INIT;
    }

    if (!data) {
        ESP_LOGE(TAG, "Invalid vehicle data pointer");
        return CAN_COMM_ERROR_INVALID_DATA;
    }

    /* Prepare CAN message data */
    vdu_can_data_t can_data = {0};
    
    /* Pack vehicle data */
    can_data.speed = (uint8_t)(data->speed & 0xFF);
    can_data.rpm_high = (uint8_t)((data->rpm >> 8) & 0xFF);
    can_data.rpm_low = (uint8_t)(data->rpm & 0xFF);
    can_data.temp = (uint8_t)(data->temperature & 0xFF);
    can_data.fuel_level = (uint8_t)(data->fuel_level & 0xFF);
    
    /* Status bits */
    can_data.status = 0;
    if (data->engine_running) can_data.status |= 0x01;
    if (data->check_engine) can_data.status |= 0x02;
    if (data->low_fuel) can_data.status |= 0x04;
    if (data->door_open) can_data.status |= 0x08;
    
    /* Calculate checksum */
    can_data.checksum = calculate_checksum((uint8_t*)&can_data, 7);
    
    /* Prepare CAN message */
    can_message_t can_msg = {
        .id = VDU_CAN_ID,
        .length = 8,
        .is_extended = false,
        .is_remote = false
    };
    
    /* Copy data to CAN message */
    memcpy(can_msg.data, &can_data, 8);

    /* Send CAN message */
    can_status_t can_status = can_send(&can_msg);
    if (can_status != CAN_OK) {
        ESP_LOGE(TAG, "Failed to send CAN message: %d", can_status);
        return CAN_COMM_ERROR_TX;
    }

    ESP_LOGD(TAG, "CAN message sent: ID=0x%03X, Speed=%d, RPM=%d", 
             VDU_CAN_ID, data->speed, data->rpm);
    
    return CAN_COMM_OK;
}

can_comm_status_t can_comm_send_simple_message(uint8_t speed, uint16_t rpm, uint8_t temp)
{
    vehicle_data_t data = {
        .speed = speed,
        .rpm = rpm,
        .temperature = temp,
        .fuel_level = 50,  /* Default 50% */
        .engine_running = true,
        .check_engine = false,
        .low_fuel = false,
        .door_open = false
    };
    
    return can_comm_send_vehicle_data(&data);
}

can_comm_status_t can_comm_get_received_message(can_rx_msg_t *msg, uint32_t timeout_ms)
{
    if (!can_comm_initialized) {
        return CAN_COMM_ERROR_NOT_INIT;
    }

    if (!msg) {
        return CAN_COMM_ERROR_INVALID_DATA;
    }

    if (xQueueReceive(can_rx_queue, msg, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
        return CAN_COMM_OK;
    }

    return CAN_COMM_ERROR_TIMEOUT;
}

bool can_comm_message_available(void)
{
    if (!can_comm_initialized || !can_rx_queue) {
        return false;
    }

    return uxQueueMessagesWaiting(can_rx_queue) > 0;
}

can_comm_status_t can_comm_get_status(void)
{
    if (!can_comm_initialized) {
        return CAN_COMM_ERROR_NOT_INIT;
    }

    can_status_t can_status = can_get_status();
    if (can_status != CAN_OK) {
        return CAN_COMM_ERROR_BUS;
    }

    return CAN_COMM_OK;
}

/* Private Functions */

static void can_rx_task(void *pvParameters)
{
    can_message_t can_msg;
    can_rx_msg_t rx_msg;
    
    ESP_LOGI(TAG, "CAN RX task started");
    
    while (1) {
        /* Receive CAN message */
        can_status_t can_status = can_receive(&can_msg, 1000); /* 1 second timeout */
        
        if (can_status == CAN_OK) {
            /* Convert to our internal format */
            rx_msg.id = can_msg.id;
            rx_msg.length = can_msg.length;
            rx_msg.timestamp = xTaskGetTickCount() * portTICK_PERIOD_MS;
            
            /* Copy data */
            for (int i = 0; i < 8; i++) {
                rx_msg.data[i] = can_msg.data[i];
            }
            
            /* Process the message */
            process_received_message(&rx_msg);
            
            /* Add to queue for main task */
            if (xQueueSend(can_rx_queue, &rx_msg, 0) != pdTRUE) {
                ESP_LOGW(TAG, "CAN RX queue full, dropping message");
                /* Log queue status for debugging */
                UBaseType_t queue_count = uxQueueMessagesWaiting(can_rx_queue);
                ESP_LOGW(TAG, "Queue status: %u messages waiting", queue_count);
            }
        }
        else if (can_status != CAN_ERROR_TIMEOUT) {
            ESP_LOGW(TAG, "CAN receive error: %d", can_status);
        }
        
        /* Small delay to prevent task starvation */
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

static uint8_t calculate_checksum(const uint8_t *data, uint8_t length)
{
    uint8_t checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

static void process_received_message(const can_rx_msg_t *msg)
{
    ESP_LOGD(TAG, "Received CAN message: ID=0x%03lX, Length=%d", msg->id, msg->length);
    
    /* Process different CAN IDs */
    switch (msg->id) {
        case 0x100: /* Engine data */
            ESP_LOGI(TAG, "Engine data received");
            break;
            
        case 0x200: /* Vehicle status */
            ESP_LOGI(TAG, "Vehicle status received");
            break;
            
        case 0x300: /* Dashboard data */
            ESP_LOGI(TAG, "Dashboard data received");
            break;
            
        default:
            ESP_LOGD(TAG, "Unknown CAN ID: 0x%03lX", msg->id);
            break;
    }
} 