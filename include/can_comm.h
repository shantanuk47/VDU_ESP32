/***************************************************************
 * can_comm.h
 * CAN Communication header for VDU_ESP32
 * Handles CAN TX/RX with ID 0x301
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#ifndef VDU_CAN_COMM_H
#define VDU_CAN_COMM_H

#include <stdint.h>
#include <stdbool.h>

/* CAN Communication Status */
typedef enum {
    CAN_COMM_OK = 0,
    CAN_COMM_ERROR_INIT,
    CAN_COMM_ERROR_NOT_INIT,
    CAN_COMM_ERROR_TX,
    CAN_COMM_ERROR_RX,
    CAN_COMM_ERROR_BUS,
    CAN_COMM_ERROR_QUEUE,
    CAN_COMM_ERROR_TASK,
    CAN_COMM_ERROR_INVALID_DATA,
    CAN_COMM_ERROR_TIMEOUT
} can_comm_status_t;

/* Vehicle Data Structure */
typedef struct {
    uint8_t speed;           /* Vehicle speed in km/h (0-255) */
    uint16_t rpm;            /* Engine RPM (0-65535) */
    uint8_t temperature;     /* Engine temperature in °C (0-255) */
    uint8_t fuel_level;      /* Fuel level percentage (0-100) */
    bool engine_running;     /* Engine running status */
    bool check_engine;       /* Check engine light */
    bool low_fuel;           /* Low fuel warning */
    bool door_open;          /* Door open status */
} vehicle_data_t;

/* CAN RX Message Structure */
typedef struct {
    uint32_t id;             /* CAN ID */
    uint8_t data[8];         /* CAN data */
    uint8_t length;          /* Data length */
    uint32_t timestamp;      /* Timestamp in milliseconds */
} can_rx_msg_t;

/* Function Declarations */

/**
 * @brief Initialize CAN communication module
 * @return CAN_COMM_OK on success, error code on failure
 */
can_comm_status_t can_comm_init(void);

/**
 * @brief Deinitialize CAN communication module
 */
void can_comm_deinit(void);

/**
 * @brief Send vehicle data via CAN bus (ID 0x301)
 * @param data Pointer to vehicle data structure
 * @return CAN_COMM_OK on success, error code on failure
 */
can_comm_status_t can_comm_send_vehicle_data(const vehicle_data_t *data);

/**
 * @brief Send simple vehicle data (speed, RPM, temperature)
 * @param speed Vehicle speed in km/h
 * @param rpm Engine RPM
 * @param temp Engine temperature in °C
 * @return CAN_COMM_OK on success, error code on failure
 */
can_comm_status_t can_comm_send_simple_message(uint8_t speed, uint16_t rpm, uint8_t temp);

/**
 * @brief Get received CAN message from queue
 * @param msg Pointer to store received message
 * @param timeout_ms Timeout in milliseconds
 * @return CAN_COMM_OK on success, error code on failure
 */
can_comm_status_t can_comm_get_received_message(can_rx_msg_t *msg, uint32_t timeout_ms);

/**
 * @brief Check if CAN message is available in queue
 * @return true if message available, false otherwise
 */
bool can_comm_message_available(void);

/**
 * @brief Get CAN communication status
 * @return CAN_COMM_OK if healthy, error code otherwise
 */
can_comm_status_t can_comm_get_status(void);

/* CAN ID Definitions */
#define VDU_CAN_ID_TX        0x301    /* VDU transmit ID */
#define VDU_CAN_ID_RX_ENGINE 0x100    /* Engine data receive ID */
#define VDU_CAN_ID_RX_STATUS 0x200    /* Vehicle status receive ID */
#define VDU_CAN_ID_RX_DASH   0x300    /* Dashboard data receive ID */

/* CAN Message Data Format for ID 0x301:
 * Byte 0: Speed (0-255 km/h)
 * Byte 1: RPM High byte
 * Byte 2: RPM Low byte
 * Byte 3: Temperature (0-255°C)
 * Byte 4: Fuel Level (0-100%)
 * Byte 5: Status bits (bit0: engine, bit1: check_engine, bit2: low_fuel, bit3: door)
 * Byte 6: Checksum (XOR of bytes 0-5)
 * Byte 7: Reserved
 */

#endif /* VDU_CAN_COMM_H */ 