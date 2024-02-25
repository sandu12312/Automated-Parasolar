#ifndef _HEADER_SWITCHHANDLING_
#define _HEADER_SWITCHHANDLING_


#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "MotorCtrl.h"
#include "HallSensorsCtrl.h"

#define FALSE 0u
#define TRUE (!FALSE)

#define OFF FALSE
#define ON TRUE

#define SWITCH_BUTTON_1_STATE	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)
#define SWITCH_BUTTON_2_STATE	HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)

typedef enum BTN_Commands
{
	BTN_Command_OFF     = 0u,
	BTN_Command_MAN_OPN = 1u,
	BTN_Command_MAN_CLS = 2u,
	BTN_Command_AUT_OPN = 3u,
	BTN_Command_AUT_CLS = 4u
}E_BTN_Commands_Type;


void SwitchHandling_Init(void);

void UpdateSwitchStates(void);
void Set_IrqButtons_States(bool statusBTN_1, bool statusBTN_2);

void DebounceButton1(void);
void DebounceButton2(void);

bool Get_is_BTN_1_PRESSED(void);
bool Get_is_BTN_2_PRESSED(void);

void Decode_Button1_AUTO_MAN_Command(void);
void Decode_Button2_AUTO_MAN_Command(void);

E_BTN_Commands_Type Get_Switch_1_Decoded_Command(void);
E_BTN_Commands_Type Get_Switch_2_Decoded_Command(void);


#endif
