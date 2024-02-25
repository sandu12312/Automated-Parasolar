#include "main.h"
#include "GPIO.h"
#include "HallSensorsCtrl.h"
#include "SwitchHandling.h"


bool enable_value = OFF;

/* function that writes enable pin on the motor driver */
void EnableDisableMotor(bool EN)
{
	if(EN)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		enable_value = ON;
	}
	else if(!EN)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		enable_value = OFF;
	}
	else
	{
		/* MISRA */
	}
}

bool dir_value = OFF;

/* function that writes direction pin on the motor driver */
void SetMotorDirection(bool DIR)
{
	if(MOTOR_DIRECTION_CW == DIR)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
		dir_value = MOTOR_DIRECTION_CW;
	}
	else if(MOTOR_DIRECTION_CCW == DIR)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		dir_value = MOTOR_DIRECTION_CCW;
	}
	else
	{
		/* MISRA */
	}
}

bool GetPin_Direction(void)
{
	return dir_value;
}

bool GetEnableMotorState(void)
{
	return enable_value;
}
