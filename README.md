### API reference

#### Car

电机控制 `motor.c` 和 `motor.h` 在 `Core/System` 中

`moveFarward(&htim4)`:前进
`moveBackward(&htim4)`:后退
`moveLeft(&htim4)`:左平移
`moveRight(&htim4)`:右平移
`moveTurnLeft(&htim4)`:左转
`moveTurnRight(&htim4)`:右转
其他 user code 都在 `main.c` 和 `stm32fxx.it.c` 中



舵机控制`Server.c`和`Server.h`分别在`Core/System` 和`Core/Inc` 中

`SetJointAngle(float angle)`设置舵机的角度：取值`[3, 180]`（正右：3，正前：85，正左：180）



超声波模块`UltrasonicWave.c`和`UltrasonicWave.h`分别在`Core/System` 和`Core/Inc` 中

`UltrasonicWave_StartMeasure()`:发出触发信号

`front_detection()`:向前方测量距离

`left_detection()`:向左方测量距离

`right_detection()`:向右方测量距离

`stm32fxx.it.c` 中接收端接受到反弹的超声波后，计数并计算距离

#### Controlller

- `void draw_map(uint8_t map[4][4])`：绘制地图，显示方格中的内容（障碍物、起讫点、路径）
- `void full_control()`: 通过上位机完全控制小车移动的GUI。目前功能实现为：点击某一个方格，在屏幕上显示该方格的坐标
- `void path_processing()`: 路径规划的GUI。目前功能为：点击某一个方格，在方格上显示一个障碍物。
- `void detect_obs()`: 自动避障的GUI，目前功能为显示所有种类的图标。
