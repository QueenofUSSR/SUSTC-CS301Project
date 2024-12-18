### API reference

#### Car

电机控制 `motor.c` 和 `motor.h` 在 `Core/System` 中

`moveFarward(&htim4)`:前进
`moveBackward(&htim4)`:后退
`moveLeft(&htim4)`:左平移
`moveRight(&htim4)`:右平移
`moveTurnLeft(&htim4)`:左转
`moveTurnRight(&htim4)`:右转

`TurnLeft_angle(TIM_HandleTypeDef *htim, **int** angle)`: 左转`angle`度

`TurnRight_angle(TIM_HandleTypeDef *htim, **int** angle)`: 右转`angle`度

---

左右转目标度数函数目前是基于小车初始位置的角度偏置（可我写的逻辑是基于目前位置的角度偏置，debug中）

---



其他 user code 都在 `main.c` 和 `stm32fxx.it.c` 中



舵机控制`Server.c`和`Server.h`分别在`Core/System` 和`Core/Inc` 中

`SetJointAngle(float angle)`设置舵机的角度：取值`[3, 180]`（正右：3，正前：85，正左：180）



超声波模块`UltrasonicWave.c`和`UltrasonicWave.h`分别在`Core/System` 和`Core/Inc` 中

`UltrasonicWave_StartMeasure()`:发出触发信号

`front_detection()`:向前方测量距离

`left_detection()`:向左方测量距离

`right_detection()`:向右方测量距离

`stm32fxx.it.c` 中接收端接受到反弹的超声波后，计数并计算距离



MPU6050:

`MPU6050_DMP_Get_Date(&pitch, &roll, &yaw);`获取欧拉角（yaw角左正右负）



`SearchRun()`:循迹模式

#### Controlller

- `void draw_map(uint8_t map[4][4])`：绘制地图，显示方格中的内容（障碍物、起讫点、路径）
- `void full_control()`: 通过上位机完全控制小车移动的GUI。目前功能实现为：点击某一个方格，在屏幕上显示该方格的坐标
- `void path_processing()`: 路径规划函数。
  - 设置起点、终点、障碍和规划路线，按下`Start`按钮将相应信息存储准备传输给小车。
  - `Confirm`进入下一个设置阶段，`Edit`回退到上一个设置阶段。
- `void detect_obs()`: 自动避障的GUI，目前功能为显示所有种类的图标。
