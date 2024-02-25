/*
 * MotorCtrl_Main.h
 *
 *  Created on: Nov 9, 2023
 *      Author: uif32608
 */

#ifndef MOTORCTRL_MAIN_H_
#define MOTORCTRL_MAIN_H_

#include "MotorCtrl.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_tim.h"

void MotorCtrl_Main_Init(void);
void MotorCtrl_Main_Task(void);

void MotorCtrl_Main_MicroSecond_IRQ_Handler(void);

void MotorCtrl_main_StartTim2(void);
void MotorCtrl_Main_PWM_Init(void);
void GetAnalogValue(void);
uint32_t ReadCurrent(void);
uint32_t MotorCtrl_Main_GetMicros(void);

#endif /* MOTORCTRL_MAIN_H_ */
