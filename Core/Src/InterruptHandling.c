#include "main.h"
#include "stdio.h"
#include "HallSensorsCtrl.h"
#include "SwitchHandling.h"


__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if((GPIO_Pin == GPIO_PIN_10) || (GPIO_Pin == GPIO_PIN_11))
	{
		Set_IrqHALLSensorStates(SENSOR_HALL_A_STATE, SENSOR_HALL_B_STATE);
	}

	if((GPIO_Pin == GPIO_PIN_1) || (GPIO_Pin == GPIO_PIN_2))
	{
		Set_IrqButtons_States(SWITCH_BUTTON_1_STATE, SWITCH_BUTTON_2_STATE);
	}
}
