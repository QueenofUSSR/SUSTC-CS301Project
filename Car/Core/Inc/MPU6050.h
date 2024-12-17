/*
 * MPU6050.h
 *
 *  Created on: Aug 25, 2024
 *      Author: 王滋行
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#define ERROR_MPU_INIT      -1
#define ERROR_SET_SENSOR    -2
#define ERROR_CONFIG_FIFO   -3
#define ERROR_SET_RATE      -4
#define ERROR_LOAD_MOTION_DRIVER    -5
#define ERROR_SET_ORIENTATION       -6
#define ERROR_ENABLE_FEATURE        -7
#define ERROR_SET_FIFO_RATE         -8
#define ERROR_SELF_TEST             -9
#define ERROR_DMP_STATE             -10

#define DEFAULT_MPU_HZ  100
#define Q30  1073741824.0f

int MPU6050_DMP_init(void);
int MPU6050_DMP_Get_Date(float *pitch, float *roll, float *yaw);

#endif /* INC_MPU6050_H_ */
