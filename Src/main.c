 /* File name: STM32_Project_Game_Tetris
 *
 * Description:xây dựng game tetris trên LCD
 *
 *
 * Last Changed By:  $Author: $
 * Revision:         $Revision: $
 * Last Changed:     $Date: $April 15, 2022
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <timer.h>
#include <system_stm32f4xx.h>
#include <lightsensor.h>
#include <Ucglib.h>
#include <stm32f401re_gpio.h>
#include <stm32f401re_rcc.h>
#include "stm32f401re_syscfg.h"
#include "stm32f401re_exti.h"



/*------------------------------------------------------------Define button-------------------------------------------------------------------------*/
#define BUTTON_LEFT_B2_PORT			GPIOB
#define BUTTON_LEFT_B2_PIN			GPIO_Pin_3

#define BUTTON_RIGHT_B4_PORT		GPIOB
#define BUTTON_RIGHT_B4_PIN			GPIO_Pin_0

#define BUTTON_UP_B1_PORT			GPIOB
#define BUTTON_UP_B1_PIN			GPIO_Pin_5

#define BUTTON_DOWN_B5_PORT			GPIOB
#define BUTTON_DOWN_B5_PIN			GPIO_Pin_4

#define BUTTON_PLAY_B3_PORT			GPIOA
#define BUTTON_PLAY_B3_PIN			GPIO_Pin_4


#define SCREEN_WIDTH 12
#define SCREEN_HEIGHT 24

/*--------------------------------------------------------------------------------------------------------------------------------------------------*/



ucg_t ucg;
static int x_axis = 10;
static uint32_t idTimer = NO_TIMER;
uint32_t idTimerStopGame = NO_TIMER;
uint8_t eCurrentState;
static int size = 5;
static int currentX = 10;
static int currentY = -9;
int checkRow = 0;
int checkColumn = 0;
int deleteRow = 0;
static int screen[SCREEN_HEIGHT][SCREEN_WIDTH];
static char str1[10] = "";
static int currentRotation = 1;
int currentBlock[4][4];
int nextBlock[4][4];
int blockIndex;
int (*block)[4] = currentBlock;
static int score = 0;
static int maxx;
static int minx;
/*------------------------------------------------------------------Function Prototype----------------------------------------------------------*/

static void AppInitCommon(void);
void LoadConfiguration(void);
void InitScreen(void);
void InitBlock(void);
void ChooseBlock(void);
void DrawBlock(int x, int y,  int block[4][4]);
void EraseBlock(int x, int y, int block[4][4]);
int Collision (int x, int y, int block[4][4]);
void FixBlock(int x, int y, int block[4][4]);
void FallingBlock(int block[4][4]);
void Falling(int block[4][4]);
void CheckFullRow(void);
void CheckFullColumn(void);
void UpdateScore(void);
int CalculateBlockMaxWidth(int block[4][4], int x0);
int CalculateBlockWidth(int block[4][4]);
void RotateBlock(void);

static void BUTTON_Interrupt_Init_B1(void);
static void BUTTON_Interrupt_Init_B2(void);
static void BUTTON_Interrupt_Init_B3(void);
static void BUTTON_Interrupt_Init_B4(void);
//static void BUTTON_Interrupt_Init_B5(void);

/*------------------------------------------------------------------Block Init-----------------------------------------------------------*/

static int block_T1[4][4] =
{
	{0,1,0,0},
	{1,1,1,0},
	{0,0,0,0},
	{0,0,0,0}
};
static int block_T2[4][4] =
{
	{0,1,0,0},
	{1,1,0,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_T3[4][4] =
{
	{0,0,0,0},
	{1,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_T4[4][4] =
{
	{0,1,0,0},
	{0,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};

static int block_J1[4][4] =
{
	{0,1,0,0},
	{0,1,0,0},
	{1,1,0,0},
	{0,0,0,0}
};
static int block_J2[4][4] =
{
	{0,0,0,0},
	{1,1,1,0},
	{0,0,1,0},
	{0,0,0,0}
};
static int block_J3[4][4] =
{
	{0,1,1,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_J4[4][4] =
{
	{1,0,0,0},
	{1,1,1,0},
	{0,0,0,0},
	{0,0,0,0}
};
static int block_L1[4][4] =
{
	{0,1,0,0},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};
static int block_L2[4][4] =
{
	{0,0,1,0},
	{1,1,1,0},
	{0,0,0,0},
	{0,0,0,0}
};
static int block_L3[4][4] =
{
	{1,1,0,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_L4[4][4] =
{
	{0,0,0,0},
	{1,1,1,0},
	{1,0,0,0},
	{0,0,0,0}
};
static int block_Z1[4][4] =
{
	{0,0,0,0},
	{1,1,0,0},
	{0,1,1,0},
	{0,0,0,0}
};
static int block_Z2[4][4] =
{
	{0,0,1,0},
	{0,1,1,0},
	{0,1,0,0},
	{0,0,0,0}
};
static int block_Z3[4][4] =
{
	{0,0,0,0},
	{0,1,1,0},
	{1,1,0,0},
	{0,0,0,0}
};
static int block_Z4[4][4] =
{
	{0,1,0,0},
	{0,1,1,0},
	{0,0,1,0},
	{0,0,0,0}
};
static int block_I1[4][4] =
{
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0},
	{0,1,0,0}
};
static int block_I2[4][4] =
{
	{0,0,0,0},
	{1,1,1,1},
	{0,0,0,0},
	{0,0,0,0}
};

static int block_O[4][4] =
{
	{0,0,0,0},
	{0,1,1,0},
	{0,1,1,0},
	{0,0,0,0}
};

/*--------------------------------------------------------------------Define Button-------------------------------------------------------*/
static void BUTTON_Interrupt_Init_B1(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_UP_B1_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	EXTI_InitStructure.EXTI_Line =  EXTI_Line5;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

static void BUTTON_Interrupt_Init_B2(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_LEFT_B2_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
	EXTI_InitStructure.EXTI_Line =  EXTI_Line3;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

static void BUTTON_Interrupt_Init_B3(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_PLAY_B3_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
	EXTI_InitStructure.EXTI_Line =  EXTI_Line4;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

static void BUTTON_Interrupt_Init_B4(void){
	GPIO_InitTypeDef	GPIO_InitStructure;
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = BUTTON_RIGHT_B4_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	EXTI_InitStructure.EXTI_Line =  EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}
//static void BUTTON_Interrupt_Init_B5(void){
//	GPIO_InitTypeDef	GPIO_InitStructure;
//	EXTI_InitTypeDef	EXTI_InitStructure;
//	NVIC_InitTypeDef	NVIC_InitStructure;
//
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_Pin = BUTTON_DOWN_B5_PIN;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);
//	EXTI_InitStructure.EXTI_Line =  EXTI_Line4;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//
//	EXTI_Init(&EXTI_InitStructure);
//
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_Init(&NVIC_InitStructure);
//}

/*----------------------------------------------------------------------Function Of Game----------------------------------------------------------------------*/

// load game screen
void LoadConfiguration(void) {
	ucg_DrawFrame(&ucg, 0, 0, 62, 122);
	ucg_DrawString(&ucg, 65, 12, 0, "Tetris Game");
	ucg_DrawString(&ucg, 65, 48, 0, "Score: ");
}

// load matrix game tetris
void InitScreen(void) {
	for (int i = -4; i < SCREEN_HEIGHT; i++) {
	        for (int j = 0; j < SCREEN_WIDTH; j++) {
	            screen[i][j] = 0;
	        }
	    }
}
//init 19 blocks of game
int (*blockTypes[])[4] = {  block_T1, block_T2, block_T3, block_T4,
							block_J1, block_J2, block_J3, block_J4,
							block_Z1, block_Z2, block_Z3, block_Z4,
							block_L1, block_L2, block_L3, block_L4,
							block_I1, block_I2, block_O};
int (*block_T[])[4] = {block_T1, block_T4, block_T3, block_T2};
int (*block_J[])[4] = {block_J1, block_J3, block_J3, block_J2};
int (*block_Z[])[4] = {block_Z1, block_Z2, block_Z3, block_Z4};
int (*block_L[])[4] = {block_L1, block_L4, block_L3, block_L2};
int (*block_I[])[4] = {block_I1, block_I2};




// random block falling
void ChooseBlock(void) {
    int randomIndex = LightSensor_MeasureUseDMAMode() % 19;
    int (*randomBlock)[4] = blockTypes[randomIndex];
    blockIndex = randomIndex;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            nextBlock[i][j] = randomBlock[i][j];
        }
    }
}

void InitBlock(void) {
	ChooseBlock();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            currentBlock[i][j] = nextBlock[i][j];
        }
    }
}

void DrawBlock(int x, int y,  int block[4][4]) {
	ucg_SetColor(&ucg,0,255,255,255);
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1) {
				int px = x + j*size+1;
				int py = y + i*size;
				ucg_DrawBox(&ucg, px, py, size, size);
			}
		}
	}
}

void EraseBlock(int x, int y, int block[4][4]) {
	ucg_SetColor(&ucg, 0, 0, 0, 0);
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1) {
				int px = x + j*size+1;
				int py = y + i*size;
				ucg_DrawBox(&ucg, px, py, size, size);
			}
		}
	}
}

// check overlap
int Collision (int x, int y, int block[4][4]) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1){
				int px = x + j*size;
				int py = y + i*size;
				if(py >= SCREEN_HEIGHT*size) return 1;
				if(screen[py/5][px/5] == 1) return 1;
			}
		}
	}
	return 0;
}

//stop falling when overlap
void FixBlock(int x, int y, int block[4][4]) {
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			if(block[i][j] == 1){
				int px = x + j*size;
				int py = y + i*size;
				screen[py/5][px/5] = 1;
			}
		}
	}
}

// calculate score
void UpdateScore(){
	ucg_SetColor(&ucg, 0, 255, 255, 255);
	memset(str1,0,sizeof(str1));
	sprintf(str1,"Score: %d",score);
	ucg_DrawString(&ucg, 65, 48, 0, str1);
}

//if full row, game delete row, update score and the blocks above will fall down
void CheckFullRow(){
	ucg_SetColor(&ucg, 0, 0, 0, 0);
	for(int i = 0; i < SCREEN_HEIGHT; i++){
		for(int j = 0; j < SCREEN_WIDTH; j++){
			if(screen[i][j] == 1){
				checkRow++;
				if(checkRow == SCREEN_WIDTH){
					score+=5;
					ucg_SetColor(&ucg, 0, 0, 0, 0);
					ucg_DrawBox(&ucg,1,i*5+1,SCREEN_WIDTH*5,5);
					for(int k = 0; k<SCREEN_WIDTH; k++){
						screen[i][k] = 0;
					}


					ucg_SetColor(&ucg, 0, 0, 0, 0);
					for(int ii = 4; ii <= i; ii++){
						for(int jj = 0; jj < SCREEN_WIDTH; jj++) {
							if(screen[ii][jj] == 1) {
								int px =  jj*size+1;
								int py =  ii*size+1;
								ucg_DrawBox(&ucg, px, py, size, size);
							}
						}
					}

					for(int new_height = i; new_height >= 5; new_height--){
						for(int new_width = SCREEN_WIDTH - 1; new_width >= 0; new_width--){
							screen[new_height][new_width] = screen[new_height - 1][new_width];//neu day man hinh thi code nay sai}
						}
					}

					ucg_SetColor(&ucg,0,255,255,255);
					for(int ii = 4; ii <= i; ii++){
						for(int jj = 0; jj < SCREEN_WIDTH; jj++) {
							if(screen[ii][jj] == 1) {
								int px =  jj*size+1;
								int py =  ii*size+1;
								ucg_DrawBox(&ucg, px, py, size, size);
							}
						}
					}

					checkRow = 0;
				}
			}
			else{
				checkRow = 0;
				break;
			}
		}
	}
}

//if full column, the game ends
void CheckFullColumn(){
	for(int i = 0 ; i < SCREEN_WIDTH;i++){
		if(screen[0][i] == 1){
			ucg_SetColor(&ucg, 0, 0, 255, 255);
			ucg_DrawString(&ucg, 4 ,60, 0, "Game Over");
			TimerStop(idTimer);
		}
	}
}

void displayBlock(int block[4][4]){
	if(!Collision(currentX, currentY + size, block)) {
		EraseBlock(currentX, currentY, block);
		currentY += size;
	}
}
// falling 1 block in 1 time
void FallingBlock(int block[4][4]){
	DrawBlock(currentX, currentY, block);
	CheckFullRow();
	if(!Collision(currentX, currentY + size, block)) {
//		EraseBlock(currentX, currentY, block);
//		currentY += size;
	}
	else {
		FixBlock(currentX, currentY, block);
		currentX = 25;
		currentY = -9;
		x_axis = currentX;
		ChooseBlock();
	    for (int i = 0; i < 4; i++) {
	        for (int j = 0; j < 4; j++) {
	            currentBlock[i][j] = nextBlock[i][j];
	        }
	    }
		CheckFullColumn();
	}
	minx = (CalculateBlockMaxWidth(block, x_axis) - CalculateBlockWidth(block))*size;
	maxx = CalculateBlockMaxWidth(block, x_axis)*size;
	if(x_axis != currentX) {
		currentX = x_axis;
	}
	UpdateScore();
}

//call function FallingBlock to falling every time each 350ms
void Falling(int block[4][4]) {
	if (idTimer != NO_TIMER) {
		TimerStop(idTimer);
	}


	TimerStart("Fall", 100, TIMER_REPEAT_FOREVER, (void*) displayBlock, block);
	idTimer = TimerStart("Fall", 1000, TIMER_REPEAT_FOREVER, (void*) FallingBlock, block);
}

//Measure the distance of the blocks to the left and right borders so that the blocks do not go beyond the screen
int CalculateBlockMaxWidth(int block[4][4], int x0) {
    int max_width = -1;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (block[i][j] == 1) {
                int cellX = x0 + j;
                if (cellX > max_width) {
                    max_width = cellX;
                }
            }
        }
    }

    return max_width - abs(x0) + 1;
}

int CalculateBlockWidth(int block[4][4]) {
    int width = 0;

    for (int i = 0; i < 4; i++) {
        int rowWidth = 0;
        for (int j = 0; j < 4; j++) {
            if (block[i][j] == 1) {
                rowWidth++;
            }
        }
        if (rowWidth > width) {
            width = rowWidth;
        }
    }

    return width;
}

void RotateBlock(void)  {
	int nextRotation = (currentRotation + 1)%4;
	int tempBlock[4][4];
	if (blockIndex >= 0 && blockIndex < 4) {
		blockIndex = (blockIndex + currentRotation)%4;
	    int (*rotBlock)[4] = block_T[blockIndex];
	    if (!Collision(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            tempBlock[i][j] = rotBlock[i][j];
		        }
		    }
	    }
	    for (int i = 0; i < 4; i++) {
	        for (int j = 0; j < 4; j++) {
	            currentBlock[i][j] = tempBlock[i][j];
	        }
	    }
	    currentRotation = nextRotation;
	}
	if (blockIndex >= 4 && blockIndex < 8) {
		blockIndex = (blockIndex - 4 + currentRotation)%4;
	    int (*rotBlock)[4] = block_J[blockIndex];
	    if (!Collision(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            tempBlock[i][j] = rotBlock[i][j];
		        }
		    }
	    }
	    for (int i = 0; i < 4; i++) {
	        for (int j = 0; j < 4; j++) {
	            currentBlock[i][j] = tempBlock[i][j];
	        }
	    }
	    currentRotation = nextRotation;
	    blockIndex += 4;
	}
	if (blockIndex >= 8 && blockIndex < 12) {
		blockIndex = (blockIndex - 8 + currentRotation)%4;
	    int (*rotBlock)[4] = block_Z[blockIndex];
	    if (!Collision(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            tempBlock[i][j] = rotBlock[i][j];
		        }
		    }
	    }
	    for (int i = 0; i < 4; i++) {
	        for (int j = 0; j < 4; j++) {
	            currentBlock[i][j] = tempBlock[i][j];
	        }
	    }
	    currentRotation = nextRotation;
	    blockIndex += 8;
	}
	if (blockIndex >= 12 && blockIndex < 16) {
		blockIndex = (blockIndex - 12 + currentRotation)%4;
	    int (*rotBlock)[4] = block_L[blockIndex];
	    if (!Collision(currentX, currentY, rotBlock)) {
		    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            tempBlock[i][j] = rotBlock[i][j];
		        }
		    }
	    }
	    for (int i = 0; i < 4; i++) {
	        for (int j = 0; j < 4; j++) {
	            currentBlock[i][j] = tempBlock[i][j];
	        }
	    }
	    currentRotation = nextRotation;
	    blockIndex += 12;
	}
	if (blockIndex >= 16 && blockIndex < 18) {
			blockIndex = (blockIndex - 16 + currentRotation)%2;
		    int (*rotBlock)[4] = block_I[blockIndex];
		    if (!Collision(currentX, currentY, rotBlock)) {
			    for (int i = 0; i < 4; i++) {
			        for (int j = 0; j < 4; j++) {
			            tempBlock[i][j] = rotBlock[i][j];
			        }
			    }
		    }
		    for (int i = 0; i < 4; i++) {
		        for (int j = 0; j < 4; j++) {
		            currentBlock[i][j] = tempBlock[i][j];
		        }
		    }
		    currentRotation = nextRotation;
		    blockIndex += 16;
		}
}


/*-------------------------------------------------------------Button Use For Play-------------------------------------------------------------*/
void EXTI3_IRQHandler(void)//Button B2 for go left
{
	if(EXTI_GetFlagStatus(EXTI_Line3) == SET)
	{
		if (x_axis + minx >= size) {
			x_axis -= size;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}
void EXTI0_IRQHandler(void)//Button B4 for go right
{
	if(EXTI_GetFlagStatus(EXTI_Line0) == SET)
	{
		if (x_axis + maxx <= (SCREEN_WIDTH-1) * size) {
			x_axis += size;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}
void EXTI4_IRQHandler(void)//Button B3 for start game
{
	if(EXTI_GetFlagStatus(EXTI_Line4) == SET)
	{
		Falling(block);
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}
void EXTI9_5_IRQHandler(void)//Button B1 for rotate
{
	if(EXTI_GetFlagStatus(EXTI_Line5) == SET)
	{
		RotateBlock();
	}
	EXTI_ClearITPendingBit(EXTI_Line5);
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------*/

static void AppInitCommon(void){
	SystemCoreClockUpdate();
	TimerInit();
	InitBlock();
	LightSensor_Init(ADC_READ_MODE_DMA);
	BUTTON_Interrupt_Init_B1();
	BUTTON_Interrupt_Init_B2();
	BUTTON_Interrupt_Init_B4();
	BUTTON_Interrupt_Init_B3();
//	BUTTON_Interrupt_Init_B5();
	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID); //là hàm khởi tạo LCD.
	ucg_ClearScreen(&ucg);
	ucg_SetFont(&ucg, ucg_font_ncenR08_hr);
	ucg_SetColor(&ucg, 0, 255, 255, 255);
	ucg_SetColor(&ucg, 1, 0, 0, 0);
	ucg_SetRotate180(&ucg);
}


int main(void){
	AppInitCommon();
	LoadConfiguration();
	while(1){
		processTimerScheduler();
	}
		return 0;
}







