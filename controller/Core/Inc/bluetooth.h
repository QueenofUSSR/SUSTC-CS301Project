#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

// Package Header (1 byte)
#define FRAME_HEADER    0xAA

// Command Types (1 byte)
#define CMD_MOTION     0x01  // Motion control commands
#define CMD_DISTANCE   0x02  // Distance data
#define CMD_NAV        0x03  // Navigation control
#define CMD_STATUS     0x04  // Status information
#define CMD_MODE       0x05  // Mode switch command

// Operation Modes
#define MODE_MANUAL    0x01  // Manual control mode
#define MODE_AUTO_NAV  0x02  // Auto navigation mode
#define MODE_OBS_NAV   0x03  // Obs navigation mode
#define MODE_LINE_TRACK 0x00 // Line tracking mode

// Motion Commands (1 byte)
#define MOTION_FORWARD  0x01
#define MOTION_BACKWARD 0x02
#define MOTION_LEFT     0x03
#define MOTION_RIGHT    0x04
#define MOTION_TURN_L   0x05
#define MOTION_TURN_R   0x06
#define MOTION_STOP     0x07

//ACK Types(1 byte)
#define ACK_MC 0x01
#define ACK_NAV_PKT 0x02

// Package Structure (for sending)
typedef struct {
    uint8_t header;     // Frame header (0xAA)
    uint8_t cmd_type;   // Command type
    uint8_t data_len;   // Data length
    uint8_t data[32];   // Data buffer
    uint8_t checksum;   // Simple checksum
} UART_Package_t;


uint8_t GetCurrentMode(void);
void SetOperationMode(uint8_t mode);
uint8_t CalculateChecksum(UART_Package_t* pkg);
HAL_StatusTypeDef UART_SendPackage(UART_Package_t* pkg);
float HandleDistanceData(UART_Package_t* pkg);
void ProcessReceivedPackage(UART_Package_t* pkg);
void UART_SendModeCmd(uint8_t mode);
void UART_SendMotionCmd(uint8_t motion_type, uint8_t speed);
void UART_SendNavCmd(uint8_t nav_mode, uint8_t* path_data, uint8_t path_len);
#endif /* __BLUETOOTH_H */ 
