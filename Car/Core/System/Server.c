#include "Server.h"
#include "main.h"
#include "tim.h"

void SetJointAngle(float angle)
{
	angle=(uint16_t)(50.0*angle/9.0+249.0);
	TIM5->CCR1 = angle;
}
