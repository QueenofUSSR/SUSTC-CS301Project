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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

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

#include "bluetooth.h"
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
unsigned char DATA_TO_SEND[800];
int state_num = 0;
u8 STATE[30];

uint8_t rxBuffer[38];
// UART_HandleTypeDef huart1;
uint8_t map[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
int mode;
extern uint8_t current_mode;
uint8_t change_mode;
uint8_t flag;
extern const unsigned char gImage_obs[6728];
extern const unsigned char gImage_HLINE[6728];
extern const unsigned char gImage_VLINE[6728];
extern const unsigned char gImage_WN[6728];
extern const unsigned char gImage_EN[6728];
extern const unsigned char gImage_WS[6728];
extern const unsigned char gImage_ES[6728];
extern const unsigned char gImage_start[6728];
extern const unsigned char gImage_end[6728];
extern const unsigned char gImage_left[6728];
extern const unsigned char gImage_right[6728];
extern const unsigned char gImage_forward[6728];
extern const unsigned char gImage_back[6728];
extern const unsigned char gImage_turnleft[6728];
extern const unsigned char gImage_turnright[6728];
extern const unsigned char gImage_stop[6728];

void draw_map(uint8_t map[4][4]); //绘制地图，显示方格中的内容（障碍物、起始点、路径）
void full_control(); //通过上位机完全控制小车移动的GUI。目前功能实现为：点击某一个方格，在屏幕上显示该方格的坐标
void path_processing(); // 路径规划的GUI。目前功能为：点击某一个方格，在方格上显示一个障碍物
void detect_obs(); //自动避障的GUI，目前功能为显示所有种类的图标

////////////////////////////////////////////////////////////////////////// 路径规划相关定义
//四个阶段
typedef enum {
    STAGE_SET_START,
    STAGE_SET_END,
    STAGE_SET_OBSTACLE,
    STAGE_DRAW_PATH,
	STAGE_FINISH
} Stage;

static Stage currentStage = STAGE_SET_START;

//起点与终点坐标
static int8_t start_r = -1,start_c = -1;
static int8_t end_r = -1, end_c = -1;

//屏幕底部预留一个高度区域，用来放两个按钮
#define BTN_CONFIRM_X1 0
#define BTN_CONFIRM_Y1 290
#define BTN_CONFIRM_X2 100
#define BTN_CONFIRM_Y2 310

#define BTN_EDIT_X1 110
#define BTN_EDIT_Y1 290
#define BTN_EDIT_X2 210
#define BTN_EDIT_Y2 310
#define BTN_START_X1 0
#define BTN_START_Y1 0
#define BTN_START_X2 100
#define BTN_START_Y2 20
#define MAX_PATH_POINTS 10000
//路径绘制数据结构
typedef struct{
	u16 x,y;
} PixelPoint;

static PixelPoint userLine[MAX_PATH_POINTS];
static int userLineLen=0;
static int drawingLine=0; // 是否正在画线中（按住屏幕）
u8 grid_path[32]; //规划路径，以格子为单位
u8 grid_len=0;
u8 nav_pkt=0; //小车是否接收到路径规划信息

void reset_all(); //重置地图与数据
void draw_buttons(); //Confirm 和 Edit 和 Start按钮
void handle_confirm(); // Confirm按钮
void handle_edit(); //Edit按钮
int touch_in_rect(u16 tx, u16 ty, u16 x1, u16 y1, u16 x2, u16 y2); //检测是否点击按钮
int screen_to_grid(int sx, int sy, int* gr, int* gc); //将屏幕坐标转为网格坐标
void draw_dashed_trajectory(PixelPoint *userLine, int userLineLen, int onLength, int offLength); //画线函数

//////////////////////////////////////////////////////////////////////////////
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
	LCD_ShowPicture(3, 41, 58, 58, gImage_turnleft);
	LCD_ShowPicture(62, 41, 58, 58, gImage_forward);
	LCD_ShowPicture(121, 41, 58, 58, gImage_turnright);
	LCD_ShowPicture(3, 100, 58, 58, gImage_left);
	LCD_ShowPicture(62, 100, 58, 58, gImage_stop);
	LCD_ShowPicture(121, 100, 58, 58, gImage_right);
	LCD_ShowPicture(62, 159, 58, 58, gImage_back);
	if(tp_dev.sta&TP_PRES_DOWN)
	{
		LCD_Clear(WHITE);
		if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
		{
			int touchRow = (tp_dev.y[0]-40)/59;
			int touchCol = (tp_dev.x[0]-2)/59;
			LCD_ShowNum(0, 0, touchRow, 1, 16);
			LCD_ShowNum(20, 0, touchCol, 1, 16);
			if(touchRow == 0 && touchCol == 0){
				UART_SendMotionCmd(MOTION_TURN_L, 100);
			} else if (touchRow == 0 && touchCol == 1){
				UART_SendMotionCmd(MOTION_FORWARD, 100);
			} else if (touchRow == 0 && touchCol == 2){
				UART_SendMotionCmd(MOTION_TURN_R, 100);
			} else if (touchRow == 1 && touchCol == 0){
				UART_SendMotionCmd(MOTION_LEFT, 100);
			} else if (touchRow == 1 && touchCol == 1){
				// Pass
			} else if (touchRow == 1 && touchCol == 2){
				UART_SendMotionCmd(MOTION_RIGHT, 100);
			} else if (touchRow == 2 && touchCol == 1){
				UART_SendMotionCmd(MOTION_BACKWARD, 100);
			}
		}
	} else {
		delay_ms(10);
	}
}


void draw_buttons()
{
    // Confirm按钮
    LCD_DrawRectangle(BTN_CONFIRM_X1, BTN_CONFIRM_Y1, BTN_CONFIRM_X2, BTN_CONFIRM_Y2);
    LCD_ShowString(BTN_CONFIRM_X1+10, BTN_CONFIRM_Y1+3, 80,16,16,(unsigned char*)"Confirm");
    // Edit按钮
    LCD_DrawRectangle(BTN_EDIT_X1, BTN_EDIT_Y1, BTN_EDIT_X2, BTN_EDIT_Y2);
    LCD_ShowString(BTN_EDIT_X1+10, BTN_EDIT_Y1+3, 80,16,16,(unsigned char*)"Edit");
    // Start按钮
    LCD_DrawRectangle(BTN_START_X1, BTN_START_Y1, BTN_START_X2, BTN_START_Y2);
    LCD_ShowString(BTN_START_X1+10, BTN_START_Y1+3, 80,16,16,(unsigned char*)"Start");
}

void path_processing()
{

	tp_dev.scan(0);
	POINT_COLOR=BLACK;
	u8 touch_lock=0;
	//显示当前状态
	unsigned char stageInfo[20];
	    switch(currentStage) {
	    	case STAGE_SET_START:
	    		sprintf((char*)stageInfo,"Set Start");
	        break;
	    	case STAGE_SET_END:
	    		sprintf((char*)stageInfo,"Set End");
	        break;
	    	case STAGE_SET_OBSTACLE:
	    		sprintf((char*)stageInfo,"Set Obstacles");
	        break;
	    	case STAGE_DRAW_PATH:
	    		sprintf((char*)stageInfo,"Draw Path");
	        break;
	    	case STAGE_FINISH:
	    		sprintf((char*)stageInfo,"Finish");
	    }
	LCD_ShowString(130, 25, 200,16,16,stageInfo);

	unsigned char str[20];
	sprintf(str, "mode%d %d %d %d %d", mode,start_r,start_c,end_r,end_c);
	LCD_ShowString(130, 0, 200, 16, 16, str);
	draw_map(map);
	draw_buttons();
	if(tp_dev.sta&TP_PRES_DOWN)
	{
		u16 tx = tp_dev.x[0];
		u16 ty = tp_dev.y[0];
		LCD_Clear(WHITE);

		if(tx<lcddev.width&&ty<lcddev.height)
		{
			//按下Confirm
			if(touch_in_rect(tx,ty,BTN_CONFIRM_X1,BTN_CONFIRM_Y1,BTN_CONFIRM_X2,BTN_CONFIRM_Y2)&&!touch_lock){
				touch_lock=1; //手指抬起才会解锁
				handle_confirm();
			}
			//按下Edit
			else if(touch_in_rect(tx,ty,BTN_EDIT_X1,BTN_EDIT_Y1,BTN_EDIT_X2,BTN_EDIT_Y2)&&!touch_lock){
				touch_lock=1;
				handle_edit();
			}
			//按下Start
			else if(touch_in_rect(tx,ty,BTN_START_X1,BTN_START_Y1,BTN_START_X2,BTN_START_Y2)&&!touch_lock){
				nav_pkt=0;
				UART_SendNavCmd(MODE_AUTO_NAV,grid_path,grid_len);
				delay_ms(500);
				while(!nav_pkt){UART_SendNavCmd(MODE_AUTO_NAV,grid_path,grid_len);delay_ms(500);}
			}
			else{
				if(currentStage==STAGE_SET_START){
					int gr,gc;
					if(screen_to_grid(tx,ty,&gr,&gc)){
						if(start_r>=0&&start_c>=0){map[start_r][start_c]=EMPTY_BLOCK;}
						start_r=gr;start_c=gc;
						map[gr][gc]=START_POINT;
					}
				}
				else if(currentStage==STAGE_SET_END){
					int gr,gc;
					if(screen_to_grid(tx,ty,&gr,&gc)){
						if(end_r>=0&&end_c>=0){map[end_r][end_c]=EMPTY_BLOCK;}
						//不能和起点重合
						if(!(gr==start_r&&gc==start_c)){
							end_r=gr;end_c=gc;
							map[gr][gc]=END_POINT;
						}
					}
				}
				else if(currentStage==STAGE_SET_OBSTACLE){
					int gr,gc;
					if(screen_to_grid(tx,ty,&gr,&gc)){
						if(map[gr][gc]==EMPTY_BLOCK&&map[gr][gc]!=START_POINT&&map[gr][gc]!=END_POINT){map[gr][gc]=OBSTACLE;}
						else if(map[gr][gc]==OBSTACLE){map[gr][gc]=EMPTY_BLOCK;}
					}
				}
				else if(currentStage==STAGE_DRAW_PATH){
						if(userLineLen<MAX_PATH_POINTS){
							if(userLineLen<2){
								userLine[userLineLen].x=tx;
								userLine[userLineLen].y=ty;
								userLineLen++;

							}
							else{
								if((userLine[userLineLen-1].x!=tx||userLine[userLineLen-1].y!=ty)&&(userLine[userLineLen-2].x!=tx||userLine[userLineLen-2].y!=ty)){
									userLine[userLineLen].x=tx;
									userLine[userLineLen].y=ty;
									userLineLen++;
								}
							}
						}
						int gr,gc;
						if(screen_to_grid(tx,ty,&gr,&gc)){
							if(grid_path[grid_len-1]!=gc||grid_path[grid_len-2]!=gr){
								grid_path[grid_len++]=gr;
								grid_path[grid_len++]=gc;
							}
						}
						if(userLineLen>1){
							draw_dashed_trajectory(userLine,userLineLen,8,4);
						}
				}
			}
		}
	} else {
		if(currentStage==STAGE_DRAW_PATH){
			LCD_ShowString(0,0,200,16,16,(unsigned char*)"Path Recorded!");
		}
		delay_ms(10);
		touch_lock=0;
	}
}

int screen_to_grid(int sx, int sy, int* gr, int* gc)
{
    int row = (sy-40)/59;
    int col = (sx-2)/59;
    if(row>=0 && row<4 && col>=0 && col<4) {
        *gr = row;
        *gc = col;
        return 1;
    }
    return 0;
}


void reset_all()
{
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            map[i][j] = EMPTY_BLOCK;
        }
    }
    start_r=-1;start_c=-1;
    end_r=-1;end_c=-1;
    userLineLen=0;
    drawingLine=0;
    currentStage=STAGE_SET_START;
    grid_len=0;
    LCD_Clear(WHITE);
}
void draw_dashed_trajectory(PixelPoint *userLine, int userLineLen, int onLength, int offLength)
{
	POINT_COLOR=RED;
    int totalPattern = onLength + offLength;
    static int patternIndex = 0;
    int drawing = 0;
    int segmentStartX = 0;
    int segmentStartY = 0;
    for(int i=0; i<userLineLen-1; i++) {
        int x1 = userLine[i].x;
        int y1 = userLine[i].y;
        int x2 = userLine[i+1].x;
        int y2 = userLine[i+1].y;
        float dx = (float)(x2 - x1);
        float dy = (float)(y2 - y1);
        float length = sqrtf(dx*dx + dy*dy);
        float ux = dx / length;
        float uy = dy / length;

        float dist = 0.0f;
        while(dist < length) {
            float curXf = x1 + ux * dist;
            float curYf = y1 + uy * dist;
            int curX = (int)(curXf+0.5f);
            int curY = (int)(curYf+0.5f);
            int posInCycle = patternIndex % totalPattern;
            int nowDrawing = (posInCycle < onLength) ? 1 : 0;

            if(nowDrawing && !drawing) {
                drawing = 1;
                segmentStartX = curX;
                segmentStartY = curY;
            } else if(!nowDrawing && drawing) {
                drawing = 0;
                LCD_DrawLine((u16)segmentStartX, (u16)segmentStartY, (u16)curX, (u16)curY);
            }


            dist += 1.0f;
            patternIndex++;
        }

        if(drawing) {
            LCD_DrawLine((u16)segmentStartX, (u16)segmentStartY, (u16)x2, (u16)y2);
            drawing = 0;
        }
    }
}
int touch_in_rect(u16 tx, u16 ty, u16 x1,u16 y1,u16 x2,u16 y2)
{
    return (tx>=x1 && tx<=x2 && ty>=y1 && ty<=y2);
}



void handle_confirm()
{
    switch(currentStage) {
    case STAGE_SET_START:
        if(start_r>=0 && start_c>=0) {
            currentStage=STAGE_SET_END;
        }
        break;
    case STAGE_SET_END:
        if(end_r>=0 && end_c>=0) {
            currentStage=STAGE_SET_OBSTACLE;
        }
        break;
    case STAGE_SET_OBSTACLE:
        currentStage=STAGE_DRAW_PATH;
        userLineLen=0;grid_len=0;
        memset(userLine,-1,sizeof(userLine));
        memset(grid_path,0,sizeof(grid_path));
        grid_path[grid_len++]=start_r;grid_path[grid_len++]=start_c; //设置起点
        break;
    case STAGE_DRAW_PATH:
        // TODO:路径绘制完成,确认后发送数据给小车
        // 通过蓝牙发送 start_r,start_c,end_r,end_c,map中的障碍等信息
        // 以及用户绘制的userLine路径数据
        LCD_ShowString(0,25,200,16,16,(unsigned char*)"Path Confirmed!");
        currentStage=STAGE_FINISH;
        if(grid_path[grid_len-1]!=end_c||grid_path[grid_len-2]!=end_r){
        	grid_path[grid_len++]=end_r;grid_path[grid_len++]=end_c;//设置终点
        }
        break;
    case STAGE_FINISH:
    	reset_all();
    	break;
    }
}

void handle_edit()
{
    // 根据当前阶段返回上一个阶段
    switch(currentStage) {
    case STAGE_SET_START:
        break;
    case STAGE_SET_END:
        currentStage=STAGE_SET_START;
        break;
    case STAGE_SET_OBSTACLE:
        currentStage=STAGE_SET_END;
        break;
    case STAGE_DRAW_PATH:
        currentStage=STAGE_SET_OBSTACLE;
        break;
    case STAGE_FINISH:
    	currentStage=STAGE_DRAW_PATH;
    	LCD_Clear(WHITE);
    	userLineLen=0;grid_len=0;
    	memset(userLine,-1,sizeof(userLine));
    	memset(grid_path,0,sizeof(grid_path));
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

void track_line()
{
	tp_dev.scan(0);
	POINT_COLOR=BLACK;
	unsigned char str[6];
	sprintf(str, "mode%d", mode);
	LCD_ShowString(180, 0, 200, 16, 16, str);

	LCD_DrawRectangle(50, 100, 190, 140);
	LCD_ShowString(60, 110, 120, 30, 18, (unsigned char*)"Track line");
	LCD_DrawRectangle(50, 180, 190, 220);
	LCD_ShowString(60, 190, 120, 30, 18, (unsigned char*)"Full control");

	if(tp_dev.sta&TP_PRES_DOWN
			&& tp_dev.x[0] > 50 && tp_dev.x[0] < 190)
	{
		if(tp_dev.y[0] > 100 && tp_dev.y[0] < 140)		//track_line
		{
			mode = 0;
			flag = 1;
			LCD_Clear(WHITE);

		}else if(tp_dev.y[0] > 180 && tp_dev.y[0] < 220)//Full_control
		{
			mode = 1;
			flag = 1;
			LCD_Clear(WHITE);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
//5个触控点的颜�?(电容触摸屏用)
const u16 POINT_COLOR_TBL[5]={RED,GREEN,BLUE,BROWN,GRED};


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, rxBuffer, 1);
  mode = FULL_CONTROL;
  flag = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  	while(1)
  	{
  		u8 key;
  		key=KEY_Scan(0);
  		change_mode=0;
  		if(flag){ //flag为1，发送模式切换信号
  			UART_SendModeCmd(mode);
  			delay_ms(500);
  			while(!change_mode){UART_SendModeCmd(mode);delay_ms(500);}
  			flag=0;
  		}

  		switch(mode)
  		{
  		case FULL_CONTROL:full_control(); break;
  		case PATH_PROCESSING:path_processing(); break;
  		case DETECT_OBS:detect_obs(); break;
  		case TRACK_LINE:track_line(); break;
  		}
		SetOperationMode(mode);

  		if(key==WKUP_PRES)	//KEY0按下,切换模式
  		{
  			flag=1; // 模式切换标志位
  			mode++;
  			mode%=4;
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
