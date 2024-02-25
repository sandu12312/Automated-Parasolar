/*
 * MotorCtrl_Main.c
 *
 *  Created on: Nov 9, 2023
 *      Author: uif32608
 */
#include "MotorCtrl_Main.h"
#include "MotorCtrl.h"
#include "SwitchHandling.h"
#include "HallSensorsCtrl.h"
#include "MotionHandling.h"
#include "CommandCtrl.h"
#include "HallSensorsCtrl.h"


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;


typedef struct MotorCtrl_Main_DATA
{
	volatile uint32_t elapsed_microseconds;
	uint32_t current_feedback;

}MotorCtrl_Main_DATA;

MotorCtrl_Main_DATA DATA_MotorCtrl_Main;

void Init_MotorCtrl_Main_DATA(void)
{
	DATA_MotorCtrl_Main.elapsed_microseconds = 0u;
	DATA_MotorCtrl_Main.current_feedback = 0u;
}

/* function that counts microseconds (called in timer interrupt) */
void MotorCtrl_Main_MicroSecond_IRQ_Handler(void)
{
	/* should be sufficient for ~2 months continuous counting -> don't bother checking overflow */
	DATA_MotorCtrl_Main.elapsed_microseconds += 10;
}


uint32_t MotorCtrl_Main_GetMicros(void)
{
	return DATA_MotorCtrl_Main.elapsed_microseconds;
}


/* init function */
void MotorCtrl_Main_Init(void)
{
	MotionHandling_Init();
	CommandCtrl_Init();
	Init_HallSensorsCtrl();
	Init_MotorCtrl_Main_DATA();
	Init_MotorCtrl_DATA();
	SwitchHandling_Init();
	MotorCtrl_main_StartTim2();
	MotorCtrl_Main_PWM_Init();
	Init_MotorCtrl_DATA();
}


/* main task function */
void MotorCtrl_Main_Task(void)
{
	HallSensorError();
	ErrorFiltering();
	Position_Track();
	UpdateSwitchStates();
	GetAnalogValue();
	MotorCtrl();
	BlockDetection();
}


/* function for starting the timer */
void MotorCtrl_main_StartTim2(void)
{
	HAL_TIM_Base_Start_IT(&htim2);
}


/* function that starts the PWM channel */
void MotorCtrl_Main_PWM_Init(void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}


/* function that gets current feedback in milliamps from the motor driver */
void GetAnalogValue(void)
{
	HAL_ADC_Start(&hadc1);

	if(HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
	{
		DATA_MotorCtrl_Main.current_feedback = HAL_ADC_GetValue(&hadc1);
	}

	HAL_ADC_Stop(&hadc1);
}


/* return current in milliamps */
uint32_t ReadCurrent(void)
{
	return DATA_MotorCtrl_Main.current_feedback;
}
