#include "CommandCtrl.h"
#include "stdbool.h"
#include "MotorCtrl.h"
#include "HallSensorsCtrl.h"
#include "MotionHandling.h"


void Check_MOTOR_Command_STOP_Movement(void);
void Check_MOTOR_Command_OFF_Movement(void);
void Check_MOTOR_Command_MAN_OPN_Movement(void);
void Check_MOTOR_Command_AUT_OPN_Movement(void);
void Check_MOTOR_Command_MAN_CLS_Movement(void);
void Check_MOTOR_Command_AUT_CLS_Movement(void);



typedef struct CommandCtrl_DATA{
		bool btn_1_state;
		bool btn_2_state;
		bool motor_norming_state;
		bool flag_first_btn_press_AUTO_OPEN; /* differentiate auto presses on OPEN */
		bool flag_first_btn_press_AUTO_CLOSE; /* differentiate auto presses on CLOSE */

		E_BTN_Commands_Type BTN_1_COMMAND; /* processed raw button command from BTN_1 */
		E_BTN_Commands_Type BTN_2_COMMAND; /* processed raw button command from BTN_2 */
		MOTOR_Commands_Type DECODED_MOTOR_COMMAND; /* BTN_1_COMMAND x BTN_2_COMMAND => MOTOR_CMD */
}CommandCtrl_DATA;

CommandCtrl_DATA DATA_CommandCtrl;



/* init function for module variables */
void CommandCtrl_Init(void)
{
	DATA_CommandCtrl.btn_1_state = OFF;
	DATA_CommandCtrl.btn_2_state = OFF;
	DATA_CommandCtrl.motor_norming_state = OFF;
	DATA_CommandCtrl.flag_first_btn_press_AUTO_OPEN = OFF;
	DATA_CommandCtrl.flag_first_btn_press_AUTO_CLOSE = OFF;

	DATA_CommandCtrl.BTN_1_COMMAND = BTN_Command_OFF;
	DATA_CommandCtrl.BTN_2_COMMAND = BTN_Command_OFF;
	DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_OFF;
}


/* function that returns set motor command */
MOTOR_Commands_Type GetMotorCommand(void)
{
	return DATA_CommandCtrl.DECODED_MOTOR_COMMAND;
}


/* transition from STOP_MOTOR_COMMAND depending on switches commands */
void Check_MOTOR_Command_STOP_Movement(void)
{
	if((DATA_CommandCtrl.BTN_1_COMMAND == BTN_Command_OFF) && (DATA_CommandCtrl.BTN_2_COMMAND == BTN_Command_OFF))
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_OFF;
	}
}


/* transition from OFF_MOTOR_COMMAND depending on switches commands */
void Check_MOTOR_Command_OFF_Movement(void)
{
	if(DATA_CommandCtrl.BTN_1_COMMAND == BTN_Command_AUT_OPN)
	{
		if(!DATA_CommandCtrl.motor_norming_state)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_MAN_OPN;
		}
		else
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_AUT_OPN;
			DATA_CommandCtrl.flag_first_btn_press_AUTO_OPEN = TRUE;
		}
	}

	if(DATA_CommandCtrl.BTN_2_COMMAND == BTN_Command_AUT_CLS)
	{
		if(!DATA_CommandCtrl.motor_norming_state)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_MAN_CLS;
		}
		else
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_AUT_CLS;
			DATA_CommandCtrl.flag_first_btn_press_AUTO_CLOSE = TRUE;
		}
	}
}


/* transition from MANUAL_OPEN_MOTOR_COMMAND depending on switches commands */
void Check_MOTOR_Command_MAN_OPN_Movement(void)
{
	if(DATA_CommandCtrl.BTN_1_COMMAND == BTN_Command_OFF)
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_OFF;
	}

	if(DATA_CommandCtrl.BTN_2_COMMAND != BTN_Command_OFF)
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_STOP;
	}
}


/* transition from AUTO_OPEN_MOTOR_COMMAND depending on switches commands */
void Check_MOTOR_Command_AUT_OPN_Movement(void)
{
	if(DATA_CommandCtrl.flag_first_btn_press_AUTO_OPEN)
	{
		if(DATA_CommandCtrl.BTN_1_COMMAND == BTN_Command_AUT_OPN)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_AUT_OPN;
		}
		else if(DATA_CommandCtrl.BTN_1_COMMAND == BTN_Command_MAN_OPN)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_MAN_OPN;
			DATA_CommandCtrl.flag_first_btn_press_AUTO_OPEN = FALSE;
		}
		else if(DATA_CommandCtrl.BTN_1_COMMAND == BTN_Command_OFF)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_AUT_OPN;
			DATA_CommandCtrl.flag_first_btn_press_AUTO_OPEN = FALSE;
		}
	}
	else
	{
		if(DATA_CommandCtrl.BTN_1_COMMAND != BTN_Command_OFF)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_STOP;
		}
	}

	if(DATA_CommandCtrl.BTN_2_COMMAND != BTN_Command_OFF)
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_STOP;
		DATA_CommandCtrl.flag_first_btn_press_AUTO_OPEN = FALSE;
	}
}


/* transition from MANUAL_CLOSE_MOTOR_COMMAND depending on switches commands */
void Check_MOTOR_Command_MAN_CLS_Movement(void)
{
	if(DATA_CommandCtrl.BTN_1_COMMAND != BTN_Command_OFF)
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_STOP;
	}

	if(DATA_CommandCtrl.BTN_2_COMMAND == BTN_Command_OFF)
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_OFF;
	}
}


/* transition from AUTO_CLOSE_MOTOR_COMMAND depending on switches commands */
void Check_MOTOR_Command_AUT_CLS_Movement(void)
{
	if(DATA_CommandCtrl.BTN_1_COMMAND != BTN_Command_OFF)
	{
		DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_STOP;
		DATA_CommandCtrl.flag_first_btn_press_AUTO_CLOSE = FALSE;
	}

	if(DATA_CommandCtrl.flag_first_btn_press_AUTO_CLOSE)
	{
		if(DATA_CommandCtrl.BTN_2_COMMAND == BTN_Command_AUT_CLS)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_AUT_CLS;
		}
		else if(DATA_CommandCtrl.BTN_2_COMMAND == BTN_Command_MAN_CLS)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_MAN_CLS;
			DATA_CommandCtrl.flag_first_btn_press_AUTO_CLOSE = FALSE;
		}
		else if(DATA_CommandCtrl.BTN_2_COMMAND == BTN_Command_OFF)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_AUT_CLS;
			DATA_CommandCtrl.flag_first_btn_press_AUTO_CLOSE = FALSE;
		}
	}
	else
	{
		if(DATA_CommandCtrl.BTN_2_COMMAND != BTN_Command_OFF)
		{
			DATA_CommandCtrl.DECODED_MOTOR_COMMAND = MOTOR_Command_STOP;
		}
	}
}


/* Function that makes transitions from one motor command to another motor command depending on button commands */
void ButtonPressedToCommandDecoder(void)
{
	DATA_CommandCtrl.motor_norming_state = isMotorNormed();
	DATA_CommandCtrl.BTN_1_COMMAND = Get_Switch_1_Decoded_Command();
	DATA_CommandCtrl.BTN_2_COMMAND = Get_Switch_2_Decoded_Command();

	switch(DATA_CommandCtrl.DECODED_MOTOR_COMMAND)
	{
		case MOTOR_Command_STOP:
		{
			Check_MOTOR_Command_STOP_Movement();
			break;
		}

		case MOTOR_Command_OFF:
		{
			Check_MOTOR_Command_OFF_Movement();
			break;
		}

		case MOTOR_Command_MAN_OPN:
		{
			Check_MOTOR_Command_MAN_OPN_Movement();
			break;
		}

		case MOTOR_Command_AUT_OPN:
		{
			Check_MOTOR_Command_AUT_OPN_Movement();
			break;
		}

		case MOTOR_Command_MAN_CLS:
		{
			Check_MOTOR_Command_MAN_CLS_Movement();
			break;
		}

		case MOTOR_Command_AUT_CLS:
		{
			Check_MOTOR_Command_AUT_CLS_Movement();
			break;
		}

		case MOTOR_Command_REVERSE_OPEN:
		{
			break;
		}

		case MOTOR_Command_REVERSE_CLOSE:
		{

			break;
		}

		default:
		{
			break;
		}
	}
}

