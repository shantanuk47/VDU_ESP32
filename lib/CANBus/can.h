/***************************************************************
 * can.h
 * CAN Bus library for ESP32 with MCP2551 transceiver
 * Project: VDU_ESP32 (Vehicle Display Unit)
 * Author: Shantanu Kumar
 * Date: 2025-07-31
 ***************************************************************/

#ifndef VDU_CAN_H
#define VDU_CAN_H

#include <stdint.h>
#include <stdbool.h>
#include "pins.h"

/* CAN Message Structure */
typedef struct {
    uint32_t id;           /* CAN ID (11-bit or 29-bit) */
    uint8_t data[8];       /* CAN data payload */
    uint8_t length;        /* Data length (0-8 bytes) */
    bool is_extended;      /* Extended frame (29-bit ID) */
    bool is_remote;        /* Remote frame */
} can_message_t;

/* CAN Status */
typedef enum {
    CAN_OK = 0,
    CAN_ERROR_INIT,
    CAN_ERROR_TX,
    CAN_ERROR_RX,
    CAN_ERROR_BUSY,
    CAN_ERROR_TIMEOUT
} can_status_t;

/* CAN Configuration */
typedef struct {
    uint32_t baudrate;     /* CAN baudrate (e.g., 500000 for 500kbps) */
    bool loopback;         /* Loopback mode for testing */
    bool silent;           /* Silent mode (listen only) */
} can_config_t;

/* Function Declarations */

/**
 * @brief Initialize CAN bus with MCP2551 transceiver
 * @param config CAN configuration structure
 * @return CAN_OK on success, error code on failure
 */
can_status_t can_init(const can_config_t *config);

/**
 * @brief Initialize CAN bus with default configuration (500 kbps)
 * @return CAN_OK on success, error code on failure
 */
can_status_t can_init_default(void);

/**
 * @brief Deinitialize CAN bus
 */
void can_deinit(void);

/**
 * @brief Send CAN message
 * @param msg Pointer to CAN message structure
 * @return CAN_OK on success, error code on failure
 */
can_status_t can_send(const can_message_t *msg);

/**
 * @brief Receive CAN message
 * @param msg Pointer to CAN message structure to store received data
 * @param timeout_ms Timeout in milliseconds (0 for non-blocking)
 * @return CAN_OK on success, error code on failure
 */
can_status_t can_receive(can_message_t *msg, uint32_t timeout_ms);

/**
 * @brief Check if CAN message is available
 * @return true if message available, false otherwise
 */
bool can_message_available(void);

/**
 * @brief Get CAN bus status
 * @return CAN_OK if bus is healthy, error code otherwise
 */
can_status_t can_get_status(void);

/**
 * @brief Set CAN filter (accept only specific IDs)
 * @param id CAN ID to accept
 * @param mask ID mask (0 = exact match, 0x7FF = accept all)
 * @param is_extended Extended frame filter
 * @return CAN_OK on success, error code on failure
 */
can_status_t can_set_filter(uint32_t id, uint32_t mask, bool is_extended);

/**
 * @brief Clear CAN filter (accept all messages)
 * @return CAN_OK on success, error code on failure
 */
can_status_t can_clear_filter(void);

/* Predefined CAN configurations */
extern const can_config_t CAN_CONFIG_500KBPS;   /* 500 kbps (default) */
extern const can_config_t CAN_CONFIG_250KBPS;   /* 250 kbps */
extern const can_config_t CAN_CONFIG_125KBPS;   /* 125 kbps */

/* Default CAN configuration - 500 kbps */
#define CAN_CONFIG_DEFAULT CAN_CONFIG_500KBPS

#endif /* VDU_CAN_H */ 