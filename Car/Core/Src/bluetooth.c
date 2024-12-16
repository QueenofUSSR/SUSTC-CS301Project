#include "stm32f1xx_hal.h"
#include "motor.h"
#include "UltrasonicWave.h"
#include "usart.h"
#include <string.h>
#include "bluetooth.h"
#include "gpio.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim4;

// Mode management
static uint8_t current_mode = MODE_MANUAL;

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
    pkg.data[0] = ACK_MC; // Status type: mode change ack
    pkg.data[1] = mode; // New mode
    pkg.checksum = CalculateChecksum(&pkg);
    
    UART_SendPackage(&pkg);
}

void SendNavAck(void){
	UART_Package_t pkg;

	pkg.header = FRAME_HEADER;
	pkg.cmd_type = CMD_STATUS;
	pkg.data_len = 1;
	pkg.data[0] = ACK_NAV_PKT; // Status type: Auto navigation packet ack
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

// Functions for Car Board
void ProcessReceivedPackage(UART_Package_t* pkg) {
    if (VerifyChecksum(pkg) != 0) {
        return; // Invalid package
    }
    
    switch(pkg->cmd_type) {
        case CMD_MOTION:
            if (current_mode == MODE_MANUAL) {
                ProcessMotionCmd(pkg->data[0]);
            }
            break;
        case CMD_NAV:
            if (current_mode == MODE_AUTO_NAV) {
                ProcessNavCmd(pkg->data[0], &pkg->data[1], pkg->data_len - 1);
            }
            break;
        case CMD_MODE:
            SetOperationMode(pkg->data[0]);
            // Stop any ongoing motion when switching modes
            StopMotion();
            // Send mode change acknowledgment
            SendModeChangeAck(pkg->data[0]);
            break;
        default:
            break;
    }
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

void ProcessMotionCmd(uint8_t motion_type) {
    UART_Package_t pkg;
    pkg.header = FRAME_HEADER;
    pkg.cmd_type = CMD_MOTION;
    pkg.data_len = 1;
    pkg.data[0] = motion_type;
    pkg.checksum = CalculateChecksum(&pkg);

    switch(motion_type) {
        case MOTION_FORWARD:
            HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_SET);
            moveForward(&htim4);
            HAL_Delay(50);
            break;
            
        case MOTION_BACKWARD:
            HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, GPIO_PIN_SET);
            moveBackward(&htim4);
            HAL_Delay(50);
            break;
            
        case MOTION_LEFT:
            HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_SET);
            moveLeft(&htim4);
            HAL_Delay(50);
            break;
        case MOTION_TURN_L:
            HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_SET);
            moveTurnLeft(&htim4);
            HAL_Delay(50);
            break;
            
        case MOTION_RIGHT:
            HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_SET);
            moveRight(&htim4);
            HAL_Delay(50);
            break;
        case MOTION_TURN_R:
            HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, GPIO_PIN_SET);
            moveTurnRight(&htim4);
            HAL_Delay(50);
            break;
            
        case MOTION_STOP:
            HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, GPIO_PIN_RESET);
            motorBreak();
            break;
            
        default:
            break;
    }

    // Send confirmation package
    UART_SendPackage(&pkg);
}
void ProcessNavCmd(uint8_t nav_mode, uint8_t* path_data, uint8_t path_len){
		UART_Package_t pkg;

	    pkg.header = FRAME_HEADER;
	    pkg.cmd_type = CMD_NAV;
	    pkg.data_len = path_len + 1;
	    pkg.data[0] = nav_mode;
	    memcpy(&pkg.data[1], path_data, path_len);
	    pkg.checksum = CalculateChecksum(&pkg);

	    if (nav_mode==MODE_AUTO_NAV){
	    	uint8_t start_c=path_data[1];
	    	uint8_t end_c=path_data[path_len-1];
	    	uint8_t point_count=path_len/2;
	    	// 根据起点终点决定初始朝向
	    	// 如果起点在终点左边，朝右；如果在终点右边，朝左
	    	if(point_count<2){
	    		SendNavAck();
	    		return;
	    	}
	    	SendNavAck();
	    	int facing_right=1;
	    	if(start_c > end_c) {
	    		facing_right = 0; // 朝左
	    	}
	    	for(int i=0;i<point_count-1;i++){
	    		uint8_t r1 = path_data[i*2];
	    		uint8_t c1 = path_data[i*2 + 1];
	    		uint8_t r2 = path_data[(i+1)*2];
	    		uint8_t c2 = path_data[(i+1)*2 + 1];
	    		int dr=r2-r1;
	    		int dc=c2-c1;
	    			if(dc>0){
	    				for(int step=0;step<dc;step++){
	    					if(facing_right) moveForward(&htim4);
	    					else moveBackward(&htim4);
	    					HAL_Delay(700);
	    					motorBreak();
	    				}
	    			}
	    			else if(dc<0){
	    				for(int step=0;step<(-dc);step++){
	    					if(facing_right) moveBackward(&htim4);
	    					else moveForward(&htim4);
	    					HAL_Delay(700);
	    					motorBreak();
	    				}
	    			}
	    			if(dr>0){
	    				for(int step=0;step<dr;step++){
	    					if(facing_right) moveRight(&htim4);
	    					else moveLeft(&htim4);
	    					HAL_Delay(700);
	    					motorBreak();
	    				}
	    			}
	    			else if(dr<0){
	    				for(int step=0;step<(-dr);step++){
	    					if(facing_right) moveLeft(&htim4);
	    					else moveRight(&htim4);
	    					HAL_Delay(700);
	    					motorBreak();
	    				}
	    			}
	    			HAL_Delay(1000);
	    	}
	    }
	    else {
	    	return;
	    }
}


void StopMotion(void) {
    HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, GPIO_PIN_RESET);
    motorBreak();
}
