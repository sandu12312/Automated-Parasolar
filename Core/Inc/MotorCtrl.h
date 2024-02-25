/*
 * MotorCtrl.h
 *
 *  Created on: Oct 5, 2023
 *      Author: uif32608
 */

#ifndef SRC_MOTORCTRL_H_
#define SRC_MOTORCTRL_H_


#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "SwitchHandling.h"

#define MOTOR_DIRECTION_TO_MECHANICAL_DIRECTION_INVERTED 	0u /* 0 -> CW -> OPEN  //  1 -> CW -> CLOSE */

typedef enum MOTOR_Commands
{
	MOTOR_Command_OFF     	    = 0u,
	MOTOR_Command_STOP          = 1u,
	MOTOR_Command_MAN_OPN 	    = 2u,
	MOTOR_Command_MAN_CLS 	    = 3u,
	MOTOR_Command_AUT_OPN 	    = 4u,
	MOTOR_Command_AUT_CLS 	    = 5u,
	MOTOR_Command_REVERSE_OPEN  = 6u,
	MOTOR_Command_REVERSE_CLOSE = 7u
}MOTOR_Commands_Type;


void Set_REVERSE_Command(void);
void Init_MotorCtrl_DATA(void);
void MotorCtrl(void);
void SetRelaxMotorCommand(void);

#endif
