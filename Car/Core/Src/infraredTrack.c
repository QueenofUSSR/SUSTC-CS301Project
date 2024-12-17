#include "infraredTrack.h"
#include "motor.h"
#include "main.h"

#define RIGHT_IR HAL_GPIO_ReadPin(RIGHT_GPIO_Port, RIGHT_Pin)
#define MID_IR HAL_GPIO_ReadPin(MID_GPIO_Port, MID_Pin)
#define LEFT_IR HAL_GPIO_ReadPin(LEFT_GPIO_Port, LEFT_Pin)

extern TIM_HandleTypeDef htim4;

void SearchRun(void)
{
	int OUTflag = 0;	//中间探头超出路线标志
	if(LEFT_IR == WHITE_AREA && RIGHT_IR == WHITE_AREA)	//左右探头未压线
	{
		if(MID_IR == BLACK_AREA)	//中间探头在线
		{
			OUTflag = 0;
			moveForwardslow(&htim4);	//直走(快速)
		}else if(MID_IR == WHITE_AREA)	//中间探头出线
		{
			OUTflag = 1;
			moveForwardslow(&htim4);	//直走(慢速)
		}
	}

	if(LEFT_IR == BLACK_AREA && RIGHT_IR == WHITE_AREA)	//左探头压线，车身右偏，需左转
	{
		while(OUTflag == 1){ //左转，直到中间探头回到线上
			moveLeftslow(&htim4);	//左转
			if(MID_IR == 1)
			{
				break;
			}
		}
	}

	if(LEFT_IR == WHITE_AREA && RIGHT_IR == BLACK_AREA)	//右探头压线，车身左偏，需右转
	{
		while(OUTflag == 1){ //右转，直到中间探头回到线上
			moveRightslow(&htim4); //右转
			if(MID_IR == 1)
			{
				break;
			}
		}
	}

	if(LEFT_IR == BLACK_AREA && RIGHT_IR == BLACK_AREA)	//交叉路段，不做处理
	{

	}

//	if(LEFT_IR == WHITE_AREA && MID_IR == WHITE_AREA && RIGHT_IR == WHITE_AREA)
//	{
//		motorBreak();
//	}
}
