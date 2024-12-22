#include "main.h"
#include "UltrasonicWave.h"

float ultrasonicWaveDist;

void UltrasonicWave_StartMeasure(void)
{
	HAL_GPIO_WritePin(TRIG_PIN_GPIO_Port, TRIG_PIN_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(TRIG_PIN_GPIO_Port,TRIG_PIN_Pin, GPIO_PIN_RESET);
}

//超声波转头函数
float front_detection()
{
	float u_temp;
	SetJointAngle(85);
	HAL_Delay(100);
	UltrasonicWave_StartMeasure();
	u_temp = ultrasonicWaveDist;
	return u_temp;
}
float left_detection()
{
	float u_temp;
	SetJointAngle(175);
	HAL_Delay(100);
	UltrasonicWave_StartMeasure();
	u_temp = ultrasonicWaveDist;
	return u_temp;
}
float right_detection()
{
	float u_temp;
	SetJointAngle(3);
	HAL_Delay(100);
	UltrasonicWave_StartMeasure();
	u_temp = ultrasonicWaveDist;
	return u_temp;
}
