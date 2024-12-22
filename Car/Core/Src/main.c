/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file : main.c
 * @brief : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "Server.h"
#include "UltrasonicWave.h"
#include "bluetooth.h"
#include "MPU6050.h"
#include "infraredTrack.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
uint8_t rxBuffer[38]; // Add receive buffer
char direction;
int yaw;
float current_pitch, current_roll, current_yaw;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <stdbool.h>
#define SIZE 4

int detectgrid[SIZE][SIZE] = {0}; // 最终返回检测到的障碍物. 0: 未检测 1：障碍物 2：空格 3:不可达
int grid[SIZE][SIZE] = {0};       // 0:未检测 1:已探测到的障碍物 2：标记一条可能的路径（下次迭代时删除）
int realgrid[SIZE][SIZE] = {
    {0, 0, 1, 0},
    {1, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 0}};                      // 模拟真实的障碍物位置
char directionGrid[SIZE][SIZE] = {'O'}; // 计算后的一次路径
char currentDir = '^';
bool visited[SIZE][SIZE] = {false}; // 一次dfs
int startX, startY, endX, endY;
int currentX, currentY;
int obstacle_detect_start, obstacle_detect_end;

int directions[4][2] = {
    {1, 0},  // 下
    {-1, 0}, // 上
    {0, 1},  // 右
    {0, -1}  // 左
};

char directionChars[4] = {'v', '^', '>', '<'};

bool canMove(int x, int y)
{

    return (x >= 0 && x < SIZE && y >= 0 && y < SIZE && grid[x][y] == 0 && !visited[x][y]);
}
bool dfs(int x, int y)
{ // 根据一对起点和终点计算一条路径
    if (x == endX && y == endY)
    {
        return true;
    }

    visited[x][y] = true;

    for (int i = 0; i < 4; i++)
    {
        int newX = x + directions[i][0];
        int newY = y + directions[i][1];

        if (canMove(newX, newY))
        {
            grid[newX][newY] = 2;

            directionGrid[x][y] = directionChars[i];

            if (dfs(newX, newY))
            {
                return true;
            }

            grid[newX][newY] = 0;
            directionGrid[newX][newY] = 'O';
        }
    }

    visited[x][y] = false;
    return false;
}

void initializeGrid()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            directionGrid[i][j] = 'O';
            visited[i][j] = false;
            if (detectgrid[i][j] == 1)
            {
                grid[i][j] = 1;
            }
            else
                grid[i][j] = 0;
        }
    }
}

void printGrid()
{
    printf("网格状态:\n");
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (grid[i][j] == 1)
            {
                printf("X "); // 障碍物
            }
            else if (grid[i][j] == 2)
            {
                printf("%c ", directionGrid[i][j]);
            }
            else
            {
                printf("O "); // 空格
            }
        }
        printf("\n");
    }
}

bool detectionComplete()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (detectgrid[i][j] == 0)
                return false;
        }
    }
    return true;
}

void adaptHead(char dir)
{
    if (dir == '^')
    {
        switch (currentDir)
        {
        case '<': // turnRight90
            break;
        case '>': // turnLeft90
            break;
        case 'v': // turnRight180
            break;
        }
    }
    else if (dir == 'v')
    {
        switch (currentDir)
        {
        case '<': // turnLeft90
            break;
        case '>': // turnRight90
            break;
        case '^': // turnRight180
            break;
        }
    }
    else if (dir == '<')
    {
        switch (currentDir)
        {
        case 'v': // turnRight90
            break;
        case '>': // turnRight180
            break;
        case '^': // turnLeft90
            break;
        }
    }
    else if (dir == '>')
    {
        switch (currentDir)
        {
        case '<': // turnRight180
            break;
        case 'v': // turnLeft90
            break;
        case '^': // turnRight90
            break;
        }
    }
    currentDir = dir;
}

int detect(int nextX, int nextY)
{
    // 原本是超声波测距，先用数组代替测距结果
    if (realgrid[nextX][nextY] == 1)
    {
        return 50;
    }
    else
        return 100;
}

void mark_path()
{
    while (SIZE * currentX + currentY != SIZE * endX + endY)
    {
        adaptHead(directionGrid[currentX][currentY]);
        int nextX, nextY;
        switch (currentDir)
        {
        case '<':
            nextX = currentX;
            nextY = currentY - 1;
            break;
        case '>':
            nextX = currentX;
            nextY = currentY + 1;
            break;
        case '^':
            nextX = currentX - 1;
            nextY = currentY;
            break;
        case 'v':
            nextX = currentX + 1;
            nextY = currentY;
            break;
        }
        int dis = detect(nextX, nextY);
        if (dis < 80)
        {
            detectgrid[nextX][nextY] = 1;
            // 传输一格探测结果到上位机
            SendEnvDetectData(SIZE * nextX + nextY, 1);
            break;
        }
        detectgrid[nextX][nextY] = 2;
        // 传输一格探测结果到上位机
        SendEnvDetectData(SIZE * nextX + nextY, 2);
        // TODO: moveForward() 向前移动80cm
        currentX = nextX;
        currentY = nextY;
    }
    return;
}

void obstacle_detect(uint8_t start, uint8_t end)
{
    // TODO: 在这里收到起点和终点坐标
    currentX = start / 4;
    currentY = start % 4;
    endX = end / 4;
    endY = end % 4;
    printf("start: %d, %d\n", currentX, currentY);
    printf("end: %d, %d\n", endX, endY);
    detectgrid[currentX][currentY] = 2;
    detectgrid[endX][endY] = 2;

    while (!detectionComplete())
    {

        initializeGrid();
        bool found = false;
        for (int i = 0; i < SIZE; i++)
        {
            if (found)
                break;
            for (int j = 0; j < SIZE; j++)
            {
                if (detectgrid[i][j] == 0)
                {
                    endX = i;
                    endY = j;
                    found = true;
                    break;
                }
            }
        }
        startX = currentX;
        startY = currentY;
        if (dfs(startX, startY))
        {
            mark_path();
        }
        else
        {
            detectgrid[endX][endY] = 3;
        }
    }
    // 找到所有障碍物，移动到终点
    startX = currentX;
    startY = currentY;
    endX = obstacle_detect_end / 4;
    endY = obstacle_detect_end % 4;
    initializeGrid();
    dfs(startX, startY);
    mark_path();
}
/* USER CODE END 0 */

/**
 * @brief The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM4_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_USART2_UART_Init();
    MX_I2C2_Init();
    MX_TIM1_Init();
    /* USER CODE BEGIN 2 */
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
    motorInit();
    motorBreak();
    int ret = 0;
    do
    {
        ret = MPU6050_DMP_init();
    } while (ret); // 初始化mpu 直到初始化完�???
    HAL_UART_Receive_IT(&huart2, rxBuffer, 1); // Start interrupt receive

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        // HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, GPIO_PIN_RESET);
        // HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, GPIO_PIN_RESET);
        // motorBreak();
        // uint8_t distance = (uint8_t)front_detection();
        SendDistanceData(front_detection());
        HAL_Delay(200);
        int mode = GetCurrentMode();
        if (mode == MODE_LINE_TRACK)
        {
            SearchRun();
        }
        else if (mode == MODE_MANUAL)
        {
            SendDistanceData(front_detection());
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        // SearchRun();
        // TurnRight_angle(&htim4, 90);
        // HAL_Delay(3000);
        // TurnLeft_angle(&htim4, 60);
        // HAL_Delay(3000);
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
