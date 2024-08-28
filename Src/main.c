 /* File name: TouchSwitch_MCU_V1.0.0
 *
 * Description:
 *
-Câu 1: Hiển thị dòng text “IOT Programming by Lumi Smarthome” lên màn hình LCD
của Kit mở rộng khi bắt đầu cắm nguồn cho thiết bị.
-Câu 2: Nhấn nút B3 năm lần khi đó tất cả các Led trên Kit mở rộng sẽ nháy năm lần
màu GREEN với cường độ sáng là 50% và hiển thị thông tin sau thiết bị lên màn hình
LCD:
-Câu 3: Nhấn các nút B1, B2, B5, B4 một lần để điều khiển bật/tắt màu tương ứng RED,
GREEN, BLUE, WHITE của tất cả các led RGB trên Kit mở rộng và còi sẽ kêu một bíp
một lần.
-Câu 4: Nhấn giữ B1/B5 để điều khiển tăng/giảm cường độ sáng của led RGB:
-Câu 5: Cập nhật thông số nhiệt độ, độ ẩm và cường độ ánh sáng của các cảm biến lên
màn hình LCD.
 *
 * Last Changed By:  $Author: Phong $
 * Revision:         $Revision: 1.0 $
 * Last Changed:     $Date: 8/22/2024 $
 *
 * Code sample:
 ******************************************************************************/
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/
#include "system_stm32f4xx.h"
#include "timer.h"
#include "eventman.h"
#include "led.h"
#include "melody.h"
#include "lightsensor.h"
#include "temhumsensor.h"
#include "eventbutton.h"
#include "button.h"
#include "Ucglib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/******************************************************************************/
/*                     PRIVATE TYPES and DEFINITIONS                         */
/******************************************************************************/
int led_level = 50;
static ucg_t ucg;
static char srcTemp[20] = "";
static char srcHumi[20] = "";
static char srcLight[20] = "";
typedef enum {
	EVENT_EMPTY,
	EVENT_APP_INIT,
	EVENT_APP_FLUSHMEM_READY,
} event_api_t, *event_api_p;

typedef enum {
	STATE_APP_STARTUP,
	STATE_APP_IDLE,
	STATE_APP_RESET
} state_app_t;


/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/
static state_app_t eCurrentState = STATE_APP_IDLE;
static void AppInitCommon();
static void MultiSensorScan(void);
void DeviceStateMachine(uint8_t event);
static void LoadConfiguration(void);
static void AppStateManager(uint8_t event);
static void SetStateApp(state_app_t state);
static state_app_t GetStateApp(void);
void increase(void* led_level);
void decrease(void* led_level);

int main(void) {
	AppInitCommon();
	SetStateApp(STATE_APP_STARTUP);
	EventSchedulerAdd(EVENT_APP_INIT);
	while(1){
		processTimerScheduler();
		processEventScheduler();

	}
}
/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/**
 * @func   AppCommon
 * @brief  Init everything
 * @param  None
 * @retval None
 */
static void AppInitCommon(){
	SystemCoreClockUpdate();
	TimerInit();
	EventSchedulerInit(AppStateManager);
	Button_Init();
	EventButton_Init();
	BuzzerControl_Init();
	LedControl_Init();
	LightSensor_Init(0);
	TemHumSensor_Init();
	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
	ucg_ClearScreen(&ucg);
	ucg_SetFont(&ucg, ucg_font_ncenR12_hr);
	ucg_SetColor(&ucg, 0, 255, 255, 255);
	ucg_SetRotate180(&ucg);
}

/**
 * @func   AppStateManager
 * @brief  Manage state application
 * @param  None
 * @retval None
 */

static void AppStateManager(uint8_t event){
	switch (GetStateApp()){
	case STATE_APP_STARTUP:
		if(event == EVENT_APP_INIT){
			LoadConfiguration();


			SetStateApp(STATE_APP_IDLE);
		} break;

	case STATE_APP_IDLE:
		    MultiSensorScan();
		    DeviceStateMachine(event);
		  break;

	case STATE_APP_RESET:
	      break;

	default:
		  break;
	}
}

/**
 * @func   SetStateApp
 * @brief  Set state of application
 * @param  state: State of application
 * @retval None
 */
static void SetStateApp(state_app_t state){

	eCurrentState = state;
}

/**
 * @func   GetStateApp
 * @brief  Get state of application
 * @param  None
 * @retval State of application
 */
static state_app_t GetStateApp(void){

	return eCurrentState;
}

/**
 * @func   LoadConfiguration
 * @brief  None
 * @param  None
 * @retval None
 */

static void LoadConfiguration(void){
	Ucglib4WireSWSPI_begin(&ucg, UCG_FONT_MODE_SOLID);
		int width = ucg_GetWidth(&ucg);
		int heightRemain = (ucg_GetHeight(&ucg) - 3)/2;
		ucg_ClearScreen(&ucg);
		ucg_SetFont(&ucg, ucg_font_helvR08_tr);
		ucg_SetRotate180(&ucg);
		ucg_DrawString(&ucg, (width - ucg_GetStrWidth(&ucg, "IOT"))/2, heightRemain - 14, 0, "IOT");
		ucg_DrawString(&ucg, (width - ucg_GetStrWidth(&ucg, "Programming by"))/2, heightRemain, 0, "Programming by");
		ucg_DrawString(&ucg, (width - ucg_GetStrWidth(&ucg, "Lumi Smarthome"))/2, heightRemain + 14, 0, "Lumi Smarthome");
}

/**
 * @func    DeviceStateMachine
 * @brief   State machine of the device
 * @param   ev
 * @retval  None
 */
void DeviceStateMachine(
    uint8_t event
)
{
    switch (event)
    {
        case EVENT_OF_BUTTON_3_PRESS_5_TIMES:
        {

        				int height = ucg_GetHeight(&ucg)/10;
        				char *string[] = {	"Device:",
        									"Board STM32 NUCLEO",
        									"Code:",
        									"STM32F401RE NUCLEO",
        									"Manufacturer:",
        									"STMicroelectronics",
        									"Kit expansion:",
        									"Lumi Smarthome",
        									"Project:",
        									"Simulator touch switch" };
        				LedControl_BlinkStart(LED_ALL_ID, BLINK_GREEN, 10, 1000, 1);
        				ucg_ClearScreen(&ucg);
        				ucg_SetFont(&ucg, ucg_font_helvR08_tr);
        				ucg_SetRotate180(&ucg);
        				for(int i = 0; i < 5; i++) {
        					int j = 2*i;
        					ucg_DrawString(&ucg, 0, ((j+1) * height), 0, string[j]);
        					ucg_DrawString(&ucg, 0, ((j+2) * height), 0, string[j + 1]);
        				}
        }break;
        case EVENT_OF_BUTTON_1_PRESS_LOGIC:
        {
        	LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_RED, 50);
        	LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_RED, 50);
        	BuzzerControl_SetMelody(pbeep);

        }break;
        case EVENT_OF_BUTTON_2_PRESS_LOGIC:
        {
        	LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_GREEN, 50);
        	LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_GREEN, 50);
        	BuzzerControl_SetMelody(pbeep);

        }break;
        case EVENT_OF_BUTTON_4_PRESS_LOGIC:
        {
        	LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_WHITE, 50);
        	LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_WHITE, 50);
        	BuzzerControl_SetMelody(pbeep);

        }break;
        case EVENT_OF_BUTTON_5_PRESS_LOGIC:
        {
        	LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_BLUE, 50);
        	LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_BLUE, 50);
        	BuzzerControl_SetMelody(pbeep);

        }break;
        case EVENT_OF_BUTTON_1_HOLD_1S:
                {
                	TimerStart("Timer1", 100, 255, increase, &led_level);

                }break;
        case EVENT_OF_BUTTON_5_HOLD_1S:
                {
                	TimerStart("Timer2", 100, 255, decrease, &led_level);

                }break;
        case EVENT_OF_BUTTON_0_RELEASED_1S:
        case EVENT_OF_BUTTON_4_RELEASED_1S:
                {
                	TimerStop(NO_TIMER);

                }break;

        default:
            break;
    }
}

/**
 * @func    Increase
 * @brief   Increase Brightness
 * @param   pLedLevel
 * @retval  None
 */
void increase(void* pLedLevel) {
    uint8_t* led_level = (uint8_t*)pLedLevel; // Correctly cast the pointer to the proper type
    if (*led_level < 100) {
        (*led_level)++; // Dereference and increment the value
        LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_RED, *led_level); // Use the updated value
        LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_RED, *led_level); // Use the updated value
    }
}

/**
 * @func    Decrease
 * @brief   Decrease Brightness
 * @param   pLedLevel
 * @retval  None
 */
void decrease(void* pLedLevel) {
    uint8_t* led_level = (uint8_t*)pLedLevel; // Correctly cast the pointer to the proper type
    if (*led_level > 0) {
        (*led_level)--; // Dereference and decrement the value
        LedControl_SetColorGeneral(LED_KIT_ID0, LED_COLOR_RED, *led_level); // Use the updated value
        LedControl_SetColorGeneral(LED_KIT_ID1, LED_COLOR_RED, *led_level); // Use the updated value
    }
}

/**
 * @func    Task_multiSensorScan
 * @brief   Scan sensor values
 * @param   None
 * @retval  None
 */
static void Task_multiSensorScan(void){
	uint32_t temp = (uint32_t) TemHumSensor_GetTemp()/100;
	    uint32_t humi = (uint32_t) TemHumSensor_GetHumi()/100;
	    uint16_t light = LightSensor_MeasureUseDMAMode();
	    ucg_ClearScreen(&ucg);
	    ucg_SetFont(&ucg, ucg_font_helvR10_hr);
	    ucg_SetRotate180(&ucg);
	    memset(srcTemp, 0, sizeof(srcTemp));
		sprintf(srcTemp, " Temp = %d oC", temp);
		ucg_DrawString(&ucg, 0, 52, 0, srcTemp);

		memset(srcHumi, 0, sizeof(srcHumi));
		sprintf(srcHumi, " Humi = %3d %%", humi);
		ucg_DrawString(&ucg, 0, 72, 0, srcHumi);

		memset(srcLight, 0, sizeof(srcLight));
		sprintf(srcLight, " Light = %lu lx", light);
		ucg_DrawString(&ucg, 0, 92, 0, srcLight);
}


static void MultiSensorScan(void){
	GetMilSecTick();
	Task_multiSensorScan();
}

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

/******************************************************************************/
