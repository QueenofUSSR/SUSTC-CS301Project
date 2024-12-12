### API reference

#### Controlller

- `void draw_map(uint8_t map[4][4])`：绘制地图，显示方格中的内容（障碍物、起讫点、路径）
- `void full_control()`: 通过上位机完全控制小车移动的GUI。目前功能实现为：点击某一个方格，在屏幕上显示该方格的坐标
- `void path_processing()`: 路径规划的GUI。目前功能为：点击某一个方格，在方格上显示一个障碍物。
- `void detect_obs()`: 自动避障的GUI，目前功能为显示所有种类的图标。
