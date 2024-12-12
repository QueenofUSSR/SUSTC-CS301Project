电机控制 motor.c 和 motor.h 在 Core/System 中
APIs:
moveFarward(&htim4):前进
moveBackward(&htim4):后退
moveLeft(&htim4):左平移
moveRight(&htim4):右平移
moveTurnLeft(&htim4):左转
moveTurnRight(&htim4):右转
其他 user code 都在 main.c 和 stm32fxx.it.c 中