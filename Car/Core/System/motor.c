#include "motor.txt"

extern TIM_HandleTypeDef htim4;

//start PWM timer
void motorInit()
{
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

void setMotorSpeed(TIM_HandleTypeDef *htim, uint32_t Channel, int16_t speed)
{
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;

    uint16_t pwm_value = (7200 - abs(speed) * 72 );  // 计算 PWM 占空比

    //set the duty cycle
    __HAL_TIM_SET_COMPARE(htim, Channel, pwm_value);

    //direction control
/* 俯视图：
 	左1——————右1
	|		 |
	|        |
	|        |
	右2——————左2
 */
    switch(Channel)
    {

    case TIM_CHANNEL_1: //左1轮
    	if(speed>0)
    		HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
    	else
    		HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
    	break;

    case TIM_CHANNEL_2: //左2轮
    	if(speed>0)
    		HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
    	else
    		HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
    	break;

    case TIM_CHANNEL_3: //右1轮
    	if(speed>0)
    		HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
    	else
    		HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
    	break;

    case TIM_CHANNEL_4: //右2轮
    	if(speed>0)
    		HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
    	else
    		HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
    	break;
    default: break;
    }
}

//void motorRunTest(TIM_HandleTypeDef *htim)
//{
//	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 0);//L1满速前进
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 0);//L2满速前进
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 7200);//R1满速前进
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 7200);//L1满速前进
//}
//void motorBackTest(TIM_HandleTypeDef *htim)
//{
//	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 7200);
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 7200);
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 0);
//	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 0);
//}
//void motorRun(int16_t speed)
//{
//	int16_t u_speed = 100 - speed;
//	int16_t f_speed = - u_speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, speed);
//}

void motorBreak()
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 7200);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 7200);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
}


//void motorBack(int16_t speed)
//{
//	int16_t u_speed = 100 - speed;
//	int16_t f_speed = - u_speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, f_speed);
//}
//
//void motorTurnLeft(int16_t speed)
//{
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, speed);
//}
//
//
//void motorSpinLeft(int16_t speed)
//{
//	int16_t u_speed = 100 - speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, u_speed);
//}
//
//void motorTurnRight(int16_t speed)
//{
//	int16_t f_speed = - speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//void motorSpinRight(int16_t speed)
//{
//	int16_t u_speed = 100 - speed;
//	int16_t f_speed = - u_speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, f_speed);
//}
//
//void motorRunDelay(int16_t speed, int time)
//{
//	int16_t f_speed = - speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, speed);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//void motorBreakDelay(int time)
//{
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//void motorBackDelay(int16_t speed, int time)
//{
//	int16_t u_speed = 100 - speed;
//	int16_t f_speed = - u_speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, f_speed);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//void motorTurnLeftDelay(int16_t speed, int time)
//{
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, speed);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//
//void motorSpinLeftDelay(int16_t speed, int time)
//{
//	int16_t u_speed = 100 - speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, u_speed);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//void motorTurnRightDelay(int16_t speed, int time)
//{
//	int16_t f_speed = - speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
//
//void motorSpinRightDelay(int16_t speed, int time)
//{
//	int16_t u_speed = 100 - speed;
//	int16_t f_speed = - u_speed;
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, u_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, f_speed);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, f_speed);
//
//	HAL_Delay(time);
//
//	setMotorSpeed(&htim4, TIM_CHANNEL_1, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_2, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_3, 0);
//	setMotorSpeed(&htim4, TIM_CHANNEL_4, 0);
//}
void moveForward(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 0);//L1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 0);//L2满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 7200);//R1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 7200);//L1满速前进
}
void moveBackward(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 0);
}
void moveLeft(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 0);
}
void moveRight(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 7200);
}
void moveTurnLeft(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 7200);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 7200);
}
void moveTurnRight(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 0);//L1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 0);//L2满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 0);//R1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 0);//L1满速前进
}

void moveForwardslow(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 3600);//L1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 3600);//L2满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 3600);//R1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 3600);//L1满速前进
}
void moveBackwardslow(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 3600);
}
void moveLeftslow(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 3600);
}
void moveRightslow(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 3600);
}
void moveTurnLeftslow(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 3600);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 3600);
}
void moveTurnRightslow(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_WritePin(LEFT_MOTOR_GO_GPIO_Port, LEFT_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LEFT2_MOTOR_GO_GPIO_Port, LEFT2_MOTOR_GO_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RIGHT_MOTOR_GO_GPIO_Port, RIGHT_MOTOR_GO_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RIGHT2_MOTOR_GO_GPIO_Port, RIGHT2_MOTOR_GO_Pin, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1, 3600);//L1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, 3600);//L2满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_3, 3600);//R1满速前进
	__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_4, 3600);//L1满速前进
}

