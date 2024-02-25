#ifndef _HEADER_GPIO_
#define _HEADER_GPIO_


#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "HallSensorsCtrl.h"

#define MOTOR_DIRECTION_CCW  0u /* counter clockwise */
#define MOTOR_DIRECTION_CW   1u /* clockwise */

bool GetEnableMotorState(void);
bool GetPin_Direction(void);
void EnableDisableMotor(bool EN);
void SetMotorDirection(bool DIR);


#endif
