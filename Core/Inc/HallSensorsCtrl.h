#ifndef _HEADER_HALLSENSORSCTRL_
#define _HEADER_HALLSENSORSCTRL_

#include "main.h"
#include "stdio.h"
#include "stdbool.h"


#define SENSOR_HALL_A_STATE 	(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_10))
#define SENSOR_HALL_B_STATE		(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11))

#define PULSEWIDTH_THRESHOLD 500 /* microseconds */

void Init_HallSensorsCtrl(void);
bool GetMotorDirection(void);
bool GetHallSensorA_state(void);
bool GetHallSensorB_state(void);

int GetCurrentPosition(void);
uint32_t GetNumberOfPulses(void);

void Set_HallSensorA_state_prev(bool hall_A_pin_state);
void Set_HallSensorB_state_prev(bool hall_B_pin_state);
void Set_IrqHALLSensorStates(bool statusHall_A, bool statusHall_B);
void SetClosePosition(void);
void Position_Track(void);
void ErrorFiltering(void);
void GetInitialPulses(void);

#endif
