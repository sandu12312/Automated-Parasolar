/*
 * MotorCtrl.c
 *
 *  Created on: Oct 5, 2023
 *      Author: uif32608
 */

#include "MotorCtrl_Main.h"
#include "SwitchHandling.h"
#include "GPIO.h"
#include "HallSensorsCtrl.h"
#include "MotionHandling.h"
#include "CommandCtrl.h"


#define MOTOR_DIRECTION_CCW 0u /* counter clockwise */
#define MOTOR_DIRECTION_CW  1u /* clockwise */
#define MOTOR_NO_DIRECTION  2u /* there's no direction set when motor is off */

#define REVERSE_TIME   500000 /* microseconds */
#define REVERSE_PWM    50u /* PWM for reverse motion */


void SetMotorCommand(void);
void SetSpeedForDenormedMotor(void);
void SetSpeedForReverseMotion(void);
void Set_MAN_OPN_NormedMotor(void);
void Set_MAN_CLS_NormedMotor(void);
void Set_REVERSE_OPEN_Command(void);
void Set_REVERSE_CLOSE_Command(void);
void Set_AUT_CLS_Command(void);
void Set_AUT_OPN_Command(void);
void Set_MAN_CLS_Command(void);
void Set_MAN_OPN_Command(void);


typedef struct MotorCtrl_DATA
{
	bool flag_reverse; /* for setting the reverse motion interval */
	bool dir_feedback;
	int current_time;
	int reverse_interval;
	int current_pos;

	MOTOR_Commands_Type MOTOR_COMMAND; /* BTN_1_COMMAND x BTN_2_COMMAND => MOTOR_CMD */

}MotorCtrl_DATA;


MotorCtrl_DATA DATA_MotorCtrl;

void Init_MotorCtrl_DATA(void)
{
	DATA_MotorCtrl.flag_reverse      = OFF; /* for setting the reverse motion interval */
	DATA_MotorCtrl.dir_feedback      = OFF;
	DATA_MotorCtrl.current_time      = 0;
	DATA_MotorCtrl.reverse_interval  = 0;
	DATA_MotorCtrl.current_pos   	 = 0;

	DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_OFF; /* BTN_1_COMMAND x BTN_2_COMMAND => MOTOR_CMD */
}





/* function that initializes the state of the sensors when motor starts */
void MotorCtrl_Init(void)
{
	bool hall_A_pin;
	bool hall_B_pin;

	hall_A_pin = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10);
	hall_B_pin = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_11);

	Set_HallSensorA_state_prev(hall_A_pin);
	Set_HallSensorB_state_prev(hall_B_pin);
}


/* function for controlling the motor by motor commands */
void MotorCtrl(void)
{
	ButtonPressedToCommandDecoder();
	SetMotorCommand();
}

bool flag = OFF;

/* function that sets reverse motion command depending on the motor direction */
void SetRelaxMotorCommand(void)
{
	DATA_MotorCtrl.dir_feedback = GetPin_Direction();

	if(MOTOR_DIRECTION_CW == DATA_MotorCtrl.dir_feedback) /* OPEN motion */
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_REVERSE_CLOSE;
	}
	else if(MOTOR_DIRECTION_CCW == DATA_MotorCtrl.dir_feedback) /* CLOSE motion */
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_REVERSE_OPEN;
	}
	else
	{

	}
}

void SetSpeedForDenormedMotor(void)
{
	TIM1->CCR1 = MAX_PWM;
}

void SetSpeedForReverseMotion(void)
{
	TIM1->CCR1 = REVERSE_PWM;
}

/* function that manages the manual open command when motor is normed */
void Set_MAN_OPN_NormedMotor(void)
{
	DATA_MotorCtrl.current_pos = GetCurrentPosition();

	if((DATA_MotorCtrl.current_pos > (OPEN_POSITION - POSITION_CATCH_RANGE)) &&
	   (DATA_MotorCtrl.current_pos < (OPEN_POSITION + POSITION_CATCH_RANGE)))
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
	}
	else
	{
		StartMotion(MOTOR_DIRECTION_CW);
	}
}

/* function that manages the manual close command when motor is normed */
void Set_MAN_CLS_NormedMotor(void)
{
	DATA_MotorCtrl.current_pos = GetCurrentPosition();

	if((DATA_MotorCtrl.current_pos > (CLOSE_POSITION - POSITION_CATCH_RANGE)) &&
	   (DATA_MotorCtrl.current_pos < (CLOSE_POSITION + POSITION_CATCH_RANGE)))
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
	}
	else
	{
		StartMotion(MOTOR_DIRECTION_CCW);
	}
}



void Set_REVERSE_OPEN_Command(void)
{
	SetMotorDirection(MOTOR_DIRECTION_CW);
	DATA_MotorCtrl.current_time = MotorCtrl_Main_GetMicros();

	if(!DATA_MotorCtrl.flag_reverse)
	{
		DATA_MotorCtrl.reverse_interval = DATA_MotorCtrl.current_time + REVERSE_TIME;
		DATA_MotorCtrl.flag_reverse = ON;
	}
	else
	{

	}

	if(DATA_MotorCtrl.flag_reverse)
	{
		if(DATA_MotorCtrl.current_time < DATA_MotorCtrl.reverse_interval)
		{
			SetSpeedForReverseMotion();
		}
		else /* end reverse motion */
		{
			//ResetBlockDetected_flag();
			DATA_MotorCtrl.flag_reverse = OFF;
			DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
		}
	}
}


/* function that defines the reverse close command */
void Set_REVERSE_CLOSE_Command(void)
{
	SetMotorDirection(MOTOR_DIRECTION_CCW);
		DATA_MotorCtrl.current_time = MotorCtrl_Main_GetMicros();

		if(!DATA_MotorCtrl.flag_reverse)
		{
			DATA_MotorCtrl.reverse_interval = DATA_MotorCtrl.current_time + REVERSE_TIME;
			DATA_MotorCtrl.flag_reverse = ON;
		}
		else
		{

		}

		if(DATA_MotorCtrl.flag_reverse)
		{
			if(DATA_MotorCtrl.current_time < DATA_MotorCtrl.reverse_interval)
			{
				SetSpeedForReverseMotion();
			}
			else /* end reverse motion */
			{
				//ResetBlockDetected_flag();
				DATA_MotorCtrl.flag_reverse = OFF;
				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
			}
		}
}

/* function that defines auto close command */
void Set_AUT_CLS_Command(void)
{
	EnableDisableMotor(ON);
	SetMotorDirection(MOTOR_DIRECTION_CCW); /* counter clockwise */

	DATA_MotorCtrl.current_pos = GetCurrentPosition();

	if((DATA_MotorCtrl.current_pos > (CLOSE_POSITION - POSITION_CATCH_RANGE)) &&
	   (DATA_MotorCtrl.current_pos < (CLOSE_POSITION + POSITION_CATCH_RANGE)))
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
	}
	else
	{
		StartMotion(MOTOR_DIRECTION_CCW);
	}
}

/* function that defines auto open command */
void Set_AUT_OPN_Command(void)
{
	EnableDisableMotor(ON);
	SetMotorDirection(MOTOR_DIRECTION_CW); /* clockwise */

	DATA_MotorCtrl.current_pos = GetCurrentPosition();

	if((DATA_MotorCtrl.current_pos > (OPEN_POSITION - POSITION_CATCH_RANGE)) &&
	   (DATA_MotorCtrl.current_pos < (OPEN_POSITION + POSITION_CATCH_RANGE)))
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
	}
	else
	{
		StartMotion(MOTOR_DIRECTION_CW);
	}
}

/* function that defines manual close command */
void Set_MAN_CLS_Command(void)
{
	EnableDisableMotor(ON);
	SetMotorDirection(MOTOR_DIRECTION_CCW); /* counter clockwise */

	DATA_MotorCtrl.current_pos = GetCurrentPosition();

	if((DATA_MotorCtrl.current_pos > (CLOSE_POSITION - POSITION_CATCH_RANGE)) &&
	   (DATA_MotorCtrl.current_pos < (CLOSE_POSITION + POSITION_CATCH_RANGE)))
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
	}
	else
	{
		StartMotion(MOTOR_DIRECTION_CCW);
	}
}

/* function that defines manual open command */
void Set_MAN_OPN_Command(void)
{
	EnableDisableMotor(ON);
	SetMotorDirection(MOTOR_DIRECTION_CW); /* clockwise */

	DATA_MotorCtrl.current_pos = GetCurrentPosition();

	if((DATA_MotorCtrl.current_pos > (OPEN_POSITION - POSITION_CATCH_RANGE)) &&
	   (DATA_MotorCtrl.current_pos < (OPEN_POSITION + POSITION_CATCH_RANGE)))
	{
		DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
	}
	else
	{
		StartMotion(MOTOR_DIRECTION_CW);
	}
}

/* function that defines each motor command */
void SetMotorCommand(void)
{
	MOTOR_Commands_Type MOTOR_CONTROL_COMMAND = GetMotorCommand(); /* MOTOR_COMMAND set from switches commands */

	switch(DATA_MotorCtrl.MOTOR_COMMAND) /* actual motor command that gets executed */
	{
		case MOTOR_Command_STOP:
		{
			StopMotion();
			EnableDisableMotor(OFF);

			/* if we get OFF_MOTOR_COMMAND from switches it sets the actual motor command to OFF
			 * so that other motor commands get set and executed after
			 * until then it's set to remain on STOP and to not change with any other motor command */

			if(MOTOR_CONTROL_COMMAND == MOTOR_Command_OFF)
			{
				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_OFF;
			}
			break;
		}

		case MOTOR_Command_OFF:
		{
			/* this command and the others below are executed as we get them from switches commands */
			DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_CONTROL_COMMAND;
			EnableDisableMotor(OFF);
			break;
		}

		case MOTOR_Command_MAN_OPN:
		{
			if(GetBlockDetected_flag())
			{

				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_REVERSE_CLOSE;
				ResetBlockDetected_flag();
			}
			else
			{
				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_CONTROL_COMMAND;
				Set_MAN_OPN_Command();
			}

			break;
		}

		case MOTOR_Command_AUT_OPN:
		{
			if(GetBlockDetected_flag())
			{

				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_REVERSE_CLOSE;
				ResetBlockDetected_flag();
			}
			else
			{
				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_CONTROL_COMMAND;
				Set_AUT_OPN_Command();
			}

			break;
		}

		case MOTOR_Command_MAN_CLS:
		{
			if(GetBlockDetected_flag())
			{

				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_REVERSE_OPEN;
				ResetBlockDetected_flag();
			}
			else
			{
				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_CONTROL_COMMAND;
				Set_MAN_CLS_Command();
			}

			break;
		}

		case MOTOR_Command_AUT_CLS:
		{
			if(GetBlockDetected_flag())
			{

				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_REVERSE_OPEN;
				ResetBlockDetected_flag();

			}
			else
			{
				DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_CONTROL_COMMAND;
				Set_AUT_CLS_Command();
			}

			break;
		}

		case MOTOR_Command_REVERSE_OPEN:
		{
			/* REVRESE COMMANDs are not set from switches but from block detection so we don't need the motor command
			 * from switches */
			Set_REVERSE_OPEN_Command();
			break;
		}

		case MOTOR_Command_REVERSE_CLOSE:
		{
			Set_REVERSE_CLOSE_Command();
			break;
		}

		default:
		{
			DATA_MotorCtrl.MOTOR_COMMAND = MOTOR_Command_STOP;
			break;
		}
	}
}
