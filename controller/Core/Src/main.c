/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "delay.h"
//#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
//#include "usmart.h"
#include "touch.h"
#include "24cxx.h"
#include "24l01.h" //通信驱动 基于spi进行通信
//#include "remote.h" 红外遥控驱动
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
unsigned char DATA_TO_SEND[800];
int state_num = 0;
u8 STATE[30];
UART_HandleTypeDef huart1;
uint8_t map[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
int mode;
extern const unsigned char gImage_obs[6728];
extern const unsigned char gImage_HLINE[6728];
extern const unsigned char gImage_VLINE[6728];
extern const unsigned char gImage_WN[6728];
extern const unsigned char gImage_EN[6728];
extern const unsigned char gImage_WS[6728];
extern const unsigned char gImage_ES[6728];
extern const unsigned char gImage_start[6728];
extern const unsigned char gImage_end[6728];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//清空屏幕并在右上角显�?"RST"

////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部�?
//画水平线
//x0,y0:坐标
//len:线长�?
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++)
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside
		{
 			if (x>imax)
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}
//两个数之差的绝对�?
//x1,x2：需取差值的两个�?
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{
	if(x1>x2)return x1-x2;
	else return x2-x1;
}
//画一条粗�?
//(x1,y1),(x2,y2):线条的起始坐�?
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	if(x1<size|| x2<size||y1<size|| y2<size)return;
	delta_x=x2-x1; //计算坐标增量
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向
	else if(delta_x==0)incx=0;//垂直�?
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;//水平�?
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标�?
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )//画线输出
	{
		gui_fill_circle(uRow,uCol,size,color);//画点
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void draw_obstacle(u16 X, u16 Y)
{
	LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_obs);
	return;
}
void draw_path(u16 X, u16 Y, u8 type)
{
	switch(type){
		case HLINE:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_HLINE);
			break;
		case VLINE:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_VLINE);
			break;
		case WN:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_WN);
			break;
		case EN:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_EN);
			break;
		case WS:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_WS);
			break;
		case ES:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_ES);
			break;
		case START_POINT:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_start);
			break;
		case END_POINT:
			LCD_ShowPicture(X+1, Y+1, 58, 58, gImage_end);
			break;
		default :
			break;
		}
}
void draw_map(uint8_t map[4][4])
{
	POINT_COLOR=BLACK;
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			int X = 2+59*j;
			int Y = 40+59*i;
			LCD_DrawRectangle(X, Y, X+59, Y+59);
			if(map[i][j]==OBSTACLE)
			{
				draw_obstacle(X,Y);
			} else if(map[i][j]==EMPTY_BLOCK) {
				continue;
			} else draw_path(X, Y, (uint8_t)map[i][j]);

		}
	}
}

void full_control()
{
	tp_dev.scan(0);
	POINT_COLOR=BLACK;
	unsigned char str[6];
	sprintf(str, "mode%d", mode);
	LCD_ShowString(180, 0, 200, 16, 16, str);
	draw_map(map);
	if(tp_dev.sta&TP_PRES_DOWN)
	{
		LCD_Clear(WHITE);
		if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
		{
			int touchRow = (tp_dev.y[0]-40)/59;
			int touchCol = (tp_dev.x[0]-2)/59;
			LCD_ShowNum(0, 0, touchRow, 1, 16);
			LCD_ShowNum(20, 0, touchCol, 1, 16);
		}
	} else {
		delay_ms(10);
	}
}

void path_processing()
{
	tp_dev.scan(0);
	POINT_COLOR=BLACK;
	unsigned char str[6];
	sprintf(str, "mode%d", mode);
	LCD_ShowString(180, 0, 200, 16, 16, str);
	draw_map(map);
	if(tp_dev.sta&TP_PRES_DOWN)
	{
		LCD_Clear(WHITE);
		if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
		{
			int touchRow = (tp_dev.y[0]-40)/59;
			int touchCol = (tp_dev.x[0]-2)/59;
			LCD_ShowNum(0, 0, touchRow, 1, 16);
			LCD_ShowNum(20, 0, touchCol, 1, 16);
			map[touchRow][touchCol] = OBSTACLE;
		}
	} else {
		delay_ms(10);
	}
}

void detect_obs()
{
//	tp_dev.scan(0);
	POINT_COLOR=BLACK;
	unsigned char str[6];
	sprintf(str, "mode%d", mode);
	LCD_ShowString(180, 0, 200, 16, 16, str);
	map[0][0] = ES;
	map[0][1] = HLINE;
	map[0][2] = WS;
	map[1][0] = VLINE;
	map[1][1] = OBSTACLE;
	map[1][2] = VLINE;
	map[2][0] = EN;
	map[2][1] = HLINE;
	map[2][2] = WN;
	map[3][0] = START_POINT;
	map[3][1] = HLINE;;
	map[3][2] = END_POINT;
	map[3][3] = OBSTACLE;
	draw_map(map);
}
////////////////////////////////////////////////////////////////////////////////
//5个触控点的颜�?(电容触摸屏用)
const u16 POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED};


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
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
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函�?
//	uart_init(115200);					//初始化串�?
//	usmart_dev.init(84); 		  	  	//初始化USMART
	LED_Init();							//初始化LED
	KEY_Init();							//初始化按�?
	LCD_Init();							//初始化LCD
	tp_dev.init();				   		//触摸屏初始化
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  mode = FULL_CONTROL;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	while(1)
  	{
  		u8 key;
  		key=KEY_Scan(0);

  		switch(mode)
  		{
  		case FULL_CONTROL:full_control(); break;
  		case PATH_PROCESSING:path_processing(); break;
  		case DETECT_OBS:detect_obs(); break;
  		}

  		if(key==WKUP_PRES)	//KEY0按下,切换模式
  		{
  			mode++;
  			mode%=3;
  			for(int i=0;i<4;i++){
  				for(int j=0;j<4;j++){
  					map[i][j] = EMPTY_BLOCK;
  				}
  			}
  			LCD_Clear(WHITE);	//清屏
  		}
  	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
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
