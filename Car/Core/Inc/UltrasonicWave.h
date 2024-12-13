#ifndef __ULTRASONICWAVE_H
#define __ULTRASONICWAVE_H

#include "Server.h"

extern TIM_HandleTypeDef htim2;
extern float ultrasonicWaveDist;

void UltrasonicWave_StartMeasure(void);
float front_detection();
float left_detection();
float right_detection();

#endif
