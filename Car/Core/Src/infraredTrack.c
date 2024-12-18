#include "infraredTrack.h"
#include "motor.h"
#include "main.h"

#define RIGHT_IR HAL_GPIO_ReadPin(RIGHT_GPIO_Port, RIGHT_Pin)
#define MID_IR HAL_GPIO_ReadPin(MID_GPIO_Port, MID_Pin)
#define LEFT_IR HAL_GPIO_ReadPin(LEFT_GPIO_Port, LEFT_Pin)

extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim1;
extern char direction;

void SearchRun(void)
{
	if(LEFT_IR == WHITE_AREA && RIGHT_IR == WHITE_AREA
			&& MID_IR == BLACK_AREA)
	{
		moveForwardslow(&htim4);
	}

	if(LEFT_IR == WHITE_AREA && RIGHT_IR == WHITE_AREA
			&& MID_IR == WHITE_AREA)
	{
		if(direction == 'R'){
			moveTurnRightslow(&htim4);
		}
		if(direction == 'L'){
			moveTurnLeftslow(&htim4);
		}
	}

	if(LEFT_IR == BLACK_AREA && RIGHT_IR == BLACK_AREA)
	{
		moveForwardslow(&htim4);
	}

	if(LEFT_IR == BLACK_AREA && RIGHT_IR == WHITE_AREA)
	{
		if(MID_IR == BLACK_AREA){
			direction = 'L';
			moveForwardslow(&htim4);
		}
		else if(MID_IR == WHITE_AREA){
			moveTurnLeftslow(&htim4);
		}
	}

	if(LEFT_IR == WHITE_AREA && RIGHT_IR == BLACK_AREA)
	{
		if(MID_IR == BLACK_AREA){
			direction = 'R';
			moveForwardslow(&htim4);
		}
		else if(MID_IR == WHITE_AREA){
			moveTurnRightslow(&htim4);
		}

	}

}
