#include "SwitchHandling.h"


#define BTN_DEBOUNCE_TIME 200u
#define CMD_DECODING_TIME 800u

#define ABS_VAL_SUBTRACT(a, b)   ((a > b) ? (a - b) : (b - a))
#define IS_BUTTON_PRESSED(a, b)  (ABS_VAL_SUBTRACT(a, b) > BTN_DEBOUNCE_TIME ? TRUE : FALSE)
#define IS_COMMAND_AUTO_MAN(a, b)(ABS_VAL_SUBTRACT(a, b) > CMD_DECODING_TIME ? TRUE : FALSE)


void Set_START_COUNT_Button1(void);
void Set_START_COUNT_Button2(void);
void SetCommand_START_COUNT_Button1(void);
void SetCommand_START_COUNT_Button2(void);


typedef struct SwitchHandling_DATA{
		volatile bool isIrqBTN_1_ON;
		volatile bool isIrqBTN_2_ON;
		volatile bool prev_isIrqBTN_1_ON;
		volatile bool prev_isIrqBTN_2_ON;

		volatile bool is_BTN_1_PRESSED;
		volatile bool is_BTN_2_PRESSED;
		volatile bool prev_is_BTN_1_PRESSED;
		volatile bool prev_is_BTN_2_PRESSED;

		volatile bool Get_BTN_pulses;
		volatile bool ContinueMovement;
		/* irq timers for button presses de-bouncing */
		volatile uint32_t irq_Millis_BTN_1_START;
		volatile uint32_t irq_Millis_BTN_1_COUNT;
		volatile uint32_t irq_Millis_BTN_2_START;
		volatile uint32_t irq_Millis_BTN_2_COUNT;

		/* timers for command decoding - manual or auto */
		volatile uint32_t cmd_Millis_BTN_1_START;
		volatile uint32_t cmd_Millis_BTN_1_COUNT;
		volatile uint32_t cmd_Millis_BTN_2_START;
		volatile uint32_t cmd_Millis_BTN_2_COUNT;


		E_BTN_Commands_Type SWITCH_1_COMMAND; /* processed raw button command from BTN_1 */
		E_BTN_Commands_Type SWITCH_2_COMMAND; /* processed raw button command from BTN_2 */
}SwitchHandling_DATA;

SwitchHandling_DATA DATA_SwitchHandling;



/* init function for module variables */
void SwitchHandling_Init(void)
{
	DATA_SwitchHandling.isIrqBTN_1_ON = OFF;
	DATA_SwitchHandling.isIrqBTN_2_ON = OFF;
	DATA_SwitchHandling.prev_isIrqBTN_1_ON = OFF;
	DATA_SwitchHandling.prev_isIrqBTN_2_ON = OFF;

	DATA_SwitchHandling.is_BTN_1_PRESSED = OFF;
	DATA_SwitchHandling.is_BTN_2_PRESSED = OFF;
	DATA_SwitchHandling.prev_is_BTN_1_PRESSED = OFF;
	DATA_SwitchHandling.prev_is_BTN_2_PRESSED = OFF;
	DATA_SwitchHandling.Get_BTN_pulses = OFF;
	DATA_SwitchHandling.ContinueMovement = OFF;

	DATA_SwitchHandling.irq_Millis_BTN_1_START = 0u;
	DATA_SwitchHandling.irq_Millis_BTN_1_COUNT = 0u;
	DATA_SwitchHandling.irq_Millis_BTN_2_START = 0u;
	DATA_SwitchHandling.irq_Millis_BTN_2_COUNT = 0u;

	DATA_SwitchHandling.cmd_Millis_BTN_1_START = 0u;
	DATA_SwitchHandling.cmd_Millis_BTN_1_COUNT = 0u;
	DATA_SwitchHandling.cmd_Millis_BTN_2_START = 0u;
	DATA_SwitchHandling.cmd_Millis_BTN_2_COUNT = 0u;

	DATA_SwitchHandling.SWITCH_1_COMMAND = BTN_Command_OFF;
	DATA_SwitchHandling.SWITCH_2_COMMAND = BTN_Command_OFF;
}


/* function that gets buttons states from interrupts */
void Set_IrqButtons_States(bool statusBTN_1, bool statusBTN_2)
{
	DATA_SwitchHandling.isIrqBTN_1_ON = statusBTN_1;
	DATA_SwitchHandling.isIrqBTN_2_ON = statusBTN_2;
}


/* function that updates the state of switches after debouncing them */
void UpdateSwitchStates(void)
{
	DebounceButton1();
	DebounceButton2();
}


/* function that gets the actual press of button 1 -> debouncing */
void DebounceButton1(void)
{
	Set_START_COUNT_Button1();

	if((DATA_SwitchHandling.isIrqBTN_1_ON) && (DATA_SwitchHandling.prev_isIrqBTN_1_ON))
	{
		DATA_SwitchHandling.is_BTN_1_PRESSED = IS_BUTTON_PRESSED(DATA_SwitchHandling.irq_Millis_BTN_1_COUNT, DATA_SwitchHandling.irq_Millis_BTN_1_START);

	}
	else
	{
		DATA_SwitchHandling.is_BTN_1_PRESSED = OFF;
	}
	if(DATA_SwitchHandling.is_BTN_1_PRESSED && DATA_SwitchHandling.Get_BTN_pulses)
	{
		DATA_SwitchHandling.ContinueMovement = TRUE;
	}
	else
	{
		DATA_SwitchHandling.Get_BTN_pulses = OFF;
		DATA_SwitchHandling.ContinueMovement = FALSE;
	}
	Decode_Button1_AUTO_MAN_Command();
}


/* function that gets the actual press of button 2 -> debouncing */
void DebounceButton2(void)
{
	Set_START_COUNT_Button2();

	if((DATA_SwitchHandling.isIrqBTN_2_ON) && (DATA_SwitchHandling.prev_isIrqBTN_2_ON))
	{
		DATA_SwitchHandling.is_BTN_2_PRESSED = IS_BUTTON_PRESSED(DATA_SwitchHandling.irq_Millis_BTN_2_COUNT, DATA_SwitchHandling.irq_Millis_BTN_2_START);
	}
	else
	{
		DATA_SwitchHandling.is_BTN_2_PRESSED = OFF;
	}

	Decode_Button2_AUTO_MAN_Command();
}


/* function that updates timestamps for debouncing button 1 */
void Set_START_COUNT_Button1(void)
{
	if(!DATA_SwitchHandling.prev_isIrqBTN_1_ON)
	{
		if(DATA_SwitchHandling.isIrqBTN_1_ON)
		{
			DATA_SwitchHandling.irq_Millis_BTN_1_START = HAL_GetTick();
			DATA_SwitchHandling.prev_isIrqBTN_1_ON = ON;
		}
	}
	else
	{
		DATA_SwitchHandling.irq_Millis_BTN_1_COUNT= HAL_GetTick();
		GetInitialPulses();
		if(!DATA_SwitchHandling.isIrqBTN_1_ON)
		{
			DATA_SwitchHandling.prev_isIrqBTN_1_ON = OFF;
		}
	}
}


/* function that updates timestamps for debouncing button 2 */
void Set_START_COUNT_Button2(void)
{
	if(!DATA_SwitchHandling.prev_isIrqBTN_2_ON)
	{
		if(DATA_SwitchHandling.isIrqBTN_2_ON)
		{
			DATA_SwitchHandling.irq_Millis_BTN_2_START = HAL_GetTick();
			DATA_SwitchHandling.prev_isIrqBTN_2_ON = ON;
		}
	}
	else
	{
		DATA_SwitchHandling.irq_Millis_BTN_2_COUNT = HAL_GetTick();
		GetInitialPulses();
		if(!DATA_SwitchHandling.isIrqBTN_2_ON)
		{
			DATA_SwitchHandling.prev_isIrqBTN_2_ON = OFF;
		}
	}
}


/* function that updates timestamps for debouncing button 1 - AUTO / MANUAL command */
void SetCommand_START_COUNT_Button1(void)
{
	/* for valid(de-bounced) button presses, translate them to command types */
	if(!DATA_SwitchHandling.prev_is_BTN_1_PRESSED) /* OPEN BTN */
	{
		if(DATA_SwitchHandling.is_BTN_1_PRESSED)
		{
			DATA_SwitchHandling.cmd_Millis_BTN_1_START = HAL_GetTick();
			DATA_SwitchHandling.prev_is_BTN_1_PRESSED = ON;
		}
	}
	else
	{
		DATA_SwitchHandling.cmd_Millis_BTN_1_COUNT = HAL_GetTick();
		if(!DATA_SwitchHandling.is_BTN_1_PRESSED)
		{
			DATA_SwitchHandling.prev_is_BTN_1_PRESSED = OFF;
		}
	}
}


/* function that updates timestamps for debouncing button 2 - AUTO / MANUAL command */
void SetCommand_START_COUNT_Button2(void)
{
	/* for valid(de-bounced) button presses, translate them to command types */
	if(!DATA_SwitchHandling.prev_is_BTN_2_PRESSED) /* CLOSE BTN */
	{
		if(DATA_SwitchHandling.is_BTN_2_PRESSED)
		{
			DATA_SwitchHandling.cmd_Millis_BTN_2_START = HAL_GetTick();
			DATA_SwitchHandling.prev_is_BTN_2_PRESSED = ON;
		}
	}
	else
	{
		DATA_SwitchHandling.cmd_Millis_BTN_2_COUNT = HAL_GetTick();
		if(!DATA_SwitchHandling.is_BTN_2_PRESSED)
		{
			DATA_SwitchHandling.prev_is_BTN_2_PRESSED = OFF;
		}
	}
}


/* function that returns the state of the actual press of button 1 */
bool Get_is_BTN_1_PRESSED(void)
{
	return DATA_SwitchHandling.is_BTN_1_PRESSED;
}


/* function that returns the state of the actual press of button 2 */
bool Get_is_BTN_2_PRESSED(void)
{
	return DATA_SwitchHandling.is_BTN_2_PRESSED;
}


/* function that decodes button 1 commands between AUTO and MANUAL */
void Decode_Button1_AUTO_MAN_Command(void)
{
	SetCommand_START_COUNT_Button1();

	if(DATA_SwitchHandling.is_BTN_1_PRESSED)
	{
		/* OPEN CMD */
		DATA_SwitchHandling.SWITCH_1_COMMAND = (IS_COMMAND_AUTO_MAN(DATA_SwitchHandling.cmd_Millis_BTN_1_COUNT, DATA_SwitchHandling.cmd_Millis_BTN_1_START) ? BTN_Command_MAN_OPN : BTN_Command_AUT_OPN);
	}
	else
	{
		DATA_SwitchHandling.SWITCH_1_COMMAND = BTN_Command_OFF;
	}
}


/* function that decodes button 2 commands between AUTO and MANUAL */
void Decode_Button2_AUTO_MAN_Command(void)
{
	SetCommand_START_COUNT_Button2();

	if(DATA_SwitchHandling.is_BTN_2_PRESSED)
	{
		/* CLOSE CMD */
		DATA_SwitchHandling.SWITCH_2_COMMAND = (IS_COMMAND_AUTO_MAN(DATA_SwitchHandling.cmd_Millis_BTN_2_COUNT, DATA_SwitchHandling.cmd_Millis_BTN_2_START) ? BTN_Command_MAN_CLS : BTN_Command_AUT_CLS);
	}
	else
	{
		DATA_SwitchHandling.SWITCH_2_COMMAND = BTN_Command_OFF;
	}
}


/* function that returns AUTO or MANUAL command after switch 1 decoding */
E_BTN_Commands_Type Get_Switch_1_Decoded_Command(void)
{
	return DATA_SwitchHandling.SWITCH_1_COMMAND;
}


/* function that returns AUTO or MANUAL command after switch 2 decoding */
E_BTN_Commands_Type Get_Switch_2_Decoded_Command(void)
{
	return DATA_SwitchHandling.SWITCH_2_COMMAND;
}

