#include "stm32f1xx_hal.h"
#include "usart.h"
#include <string.h>
#include "bluetooth.h"
#include "gpio.h"
#include "lcd.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;
extern uint8_t change_mode;
extern uint8_t nav_pkt;

// Mode management
uint8_t current_mode = MODE_MANUAL;

void SetOperationMode(uint8_t mode) {
    if (mode <= MODE_LINE_TRACK) {
        current_mode = mode;
    }
}

uint8_t GetCurrentMode(void) {
    return current_mode;
}

// Mode switching functions
void UART_SendModeCmd(uint8_t mode) {
    UART_Package_t pkg;
    
    pkg.header = FRAME_HEADER;
    pkg.cmd_type = CMD_MODE;
    pkg.data_len = 1;
    pkg.data[0] = mode;
    pkg.checksum = CalculateChecksum(&pkg);
    
    UART_SendPackage(&pkg);
}

void SendModeChangeAck(uint8_t mode) {
    UART_Package_t pkg;
    
    pkg.header = FRAME_HEADER;
    pkg.cmd_type = CMD_STATUS;
    pkg.data_len = 2;
    pkg.data[0] = 0x01; // Status type: mode change ack
    pkg.data[1] = mode; // New mode
    pkg.checksum = CalculateChecksum(&pkg);
    
    UART_SendPackage(&pkg);
}

// Functions for Controller Board
void UART_SendMotionCmd(uint8_t motion_type, uint8_t speed) {
    UART_Package_t pkg;
    
    pkg.header = FRAME_HEADER;
    pkg.cmd_type = CMD_MOTION;
    pkg.data_len = 2;
    pkg.data[0] = motion_type;
    pkg.data[1] = speed;    // TODO: Set Speed
    pkg.checksum = CalculateChecksum(&pkg);
    
    UART_SendPackage(&pkg);
}

void UART_SendNavCmd(uint8_t nav_mode, uint8_t* path_data, uint8_t path_len) {
    UART_Package_t pkg;
    
    pkg.header = FRAME_HEADER;
    pkg.cmd_type = CMD_NAV;
    pkg.data_len = path_len + 1;
    pkg.data[0] = nav_mode;
    memcpy(&pkg.data[1], path_data, path_len);
    pkg.checksum = CalculateChecksum(&pkg);
    
    UART_SendPackage(&pkg);
}

void SendDistanceData(float distance) {
    UART_Package_t pkg;
    
    pkg.header = FRAME_HEADER;
    pkg.cmd_type = CMD_DISTANCE;
    pkg.data_len = 4;  // Float is 4 bytes
    
    // Convert float to bytes
    uint8_t* float_bytes = (uint8_t*)&distance;
    pkg.data[0] = float_bytes[0];  // Byte 0 
    pkg.data[1] = float_bytes[1];  // Byte 1
    pkg.data[2] = float_bytes[2];  // Byte 2
    pkg.data[3] = float_bytes[3];  // Byte 3
    
    pkg.checksum = CalculateChecksum(&pkg);
    
    UART_SendPackage(&pkg);
}

// Utility Functions
uint8_t CalculateChecksum(UART_Package_t* pkg) {
    uint8_t sum = 0;
    sum += pkg->cmd_type;
    sum += pkg->data_len;
    for(int i = 0; i < pkg->data_len; i++) {
        sum += pkg->data[i];
    }
    return sum;
}

int VerifyChecksum(UART_Package_t* pkg) {
    uint8_t sum = CalculateChecksum(pkg);
    return (sum == pkg->checksum) ? 0 : -1;
}

void ProcessReceivedPackage(UART_Package_t* pkg) {
    if (VerifyChecksum(pkg) != 0) {
        return; // Invalid package
    }
    
    switch(pkg->cmd_type) {
        case CMD_MOTION:
            if (current_mode == MODE_MANUAL) {
                //ProcessMotionCmd(pkg->data[0]);
            }
            break;
        case CMD_NAV:
            if (current_mode == MODE_AUTO_NAV) {
                // ProcessNavCmd(pkg->data[0], &pkg->data[1], pkg->data_len - 1);
            }
            break;
        case CMD_MODE:
            SetOperationMode(pkg->data[0]);
            // Send mode change acknowledgment
            SendModeChangeAck(pkg->data[0]);
            break;
        case CMD_DISTANCE:
            LCD_ShowNum(0, 10, HandleDistanceData(pkg), 1, 16);
            break;
        case CMD_STATUS:
        	switch(pkg->data[0]){
        	case ACK_MC:
        		uint8_t mode=pkg->data[1];
        		char temp[20];
        		sprintf(temp,"Mode changed to %d",mode);
        		LCD_ShowString(0,25,200,16,16,temp);
        		change_mode=1;
        		break;
        	case ACK_NAV_PKT:
        		LCD_ShowString(0,25,200,16,16,(unsigned char*)"Navigation Data Received");
        		nav_pkt=1;
        		break;
        	default:
        		break;

        	}
        default:
            break;
    }
}

float HandleDistanceData(UART_Package_t* pkg) {
    float distance = 0.0f;
    
    // Check if this is a distance data package
    if (pkg->cmd_type != CMD_DISTANCE || pkg->data_len != 4) {
        return -1.0f;  // Invalid package
    }
    
    // Convert 4 bytes back to float
    uint8_t* float_bytes = (uint8_t*)&distance;
    float_bytes[0] = pkg->data[0];
    float_bytes[1] = pkg->data[1];
    float_bytes[2] = pkg->data[2];
    float_bytes[3] = pkg->data[3];
    
    return distance;
}

HAL_StatusTypeDef UART_SendPackage(UART_Package_t* pkg) {
    // 计算总的数据长度：header(1) + cmd_type(1) + data_len(1) + data(data_len) + checksum(1)
    uint8_t total_len = 3 + pkg->data_len + 1;
    
    // 创建发送缓冲区
    uint8_t tx_buffer[38]; // 最大包长度：3 + 32 + 1 = 36字节
    
    // 组装数据包
    tx_buffer[0] = pkg->header;
    tx_buffer[1] = pkg->cmd_type;
    tx_buffer[2] = pkg->data_len;
    memcpy(&tx_buffer[3], pkg->data, pkg->data_len);
    tx_buffer[3 + pkg->data_len] = pkg->checksum;
    
    // 使用HAL_UART_Transmit发送数据
    return HAL_UART_Transmit(&huart2, tx_buffer, total_len, 200); // 200ms超时
}
