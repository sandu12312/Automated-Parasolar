#include "HallSensorsCtrl.h"
#include "GPIO.h"
#include "MotorCtrl.h"
#include "MotionHandling.h"
#include "MotorCtrl_Main.h"

#define FALSE 0u
#define TRUE  (!FALSE)

#define OFF FALSE
#define ON  TRUE

#define LOW  0u
#define HIGH 1u

#define MOTOR_DIRECTION_CCW  0u /* counter clockwise */
#define MOTOR_DIRECTION_CW   1u /* clockwise */

typedef enum HallStates
{
	HALL_SENSOR_A_LOW_B_LOW   = 0u,
	HALL_SENSOR_A_LOW_B_HIGH  = 1u,
	HALL_SENSOR_A_HIGH_B_HIGH = 2u,
	HALL_SENSOR_A_HIGH_B_LOW  = 3u

}E_HallSensorStates_Type;


typedef struct HallSesorsCtrl_DATA
{
	bool DIR;
	bool invalid_pulse_A_flag; /* used for error tracking */
	bool invalid_pulse_B_flag; /* used for error tracking */
	bool filtered_SensorA_pulse;
	bool filtered_SensorB_pulse;
	bool reset_block_detection_flag;

	int position_tracking_sensor;
	int error_tracking;

	uint32_t timestamp_A_HighState;
	uint32_t timestamp_A_LowState;
	uint32_t timestamp_B_HighState;
	uint32_t timestamp_B_LowState;

	volatile bool HallSensorA_state_prev;
	volatile bool HallSensorB_state_prev;
	volatile bool HallSensorA_state;
	volatile bool HallSensorB_state;
	volatile bool InitialPulses;
	E_HallSensorStates_Type prev_HallSensor_State;
	E_HallSensorStates_Type HallSensor_State;


}HallSesorsCtrl_DATA;


HallSesorsCtrl_DATA DATA_HallSensorCtrl;

void Init_HallSensorsCtrl(void)
{
	DATA_HallSensorCtrl.DIR 				       = OFF;
	DATA_HallSensorCtrl.invalid_pulse_A_flag       = OFF; /* used for error tracking */
	DATA_HallSensorCtrl.invalid_pulse_B_flag       = OFF; /* used for error tracking */
	DATA_HallSensorCtrl.filtered_SensorA_pulse	   = OFF;
	DATA_HallSensorCtrl.filtered_SensorB_pulse	   = OFF;
	DATA_HallSensorCtrl.reset_block_detection_flag = OFF;

	DATA_HallSensorCtrl.position_tracking_sensor = 0;
	DATA_HallSensorCtrl.error_tracking           = 0;

	DATA_HallSensorCtrl.timestamp_A_HighState = 0u;
	DATA_HallSensorCtrl.timestamp_A_LowState  = 0u;
	DATA_HallSensorCtrl.timestamp_B_HighState = 0u;
	DATA_HallSensorCtrl.timestamp_B_LowState  = 0u;

	DATA_HallSensorCtrl.HallSensorA_state_prev       = OFF;
	DATA_HallSensorCtrl.HallSensorB_state_prev 	 	 = OFF;
	DATA_HallSensorCtrl.HallSensorA_state 			 = OFF;
	DATA_HallSensorCtrl.HallSensorB_state 			 = OFF;
	DATA_HallSensorCtrl.InitialPulses                = TRUE;
	DATA_HallSensorCtrl.prev_HallSensor_State = HALL_SENSOR_A_LOW_B_LOW;
	DATA_HallSensorCtrl.HallSensor_State      = HALL_SENSOR_A_LOW_B_LOW;
}


/* function that sets previous state of Hall sensor A when starting the motor */
void Set_HallSensorA_state_prev(bool hall_A_pin_state)
{
	DATA_HallSensorCtrl.HallSensorA_state_prev = hall_A_pin_state;
}


/* function that sets previous state of Hall sensor B when starting the motor */
void Set_HallSensorB_state_prev(bool hall_B_pin_state)
{
	DATA_HallSensorCtrl.HallSensorB_state_prev = hall_B_pin_state;
}


/* function that sets sets Hall sensor A and Hall sensor B states from interrupts */
void Set_IrqHALLSensorStates(bool statusHall_A, bool statusHall_B)
{
	DATA_HallSensorCtrl.HallSensorA_state = statusHall_A;
	DATA_HallSensorCtrl.HallSensorB_state = statusHall_B;
}

void GetInitialPulses(void)
{
	DATA_HallSensorCtrl.InitialPulses = TRUE;
}

bool return_InitialPulses(void)
{
	return DATA_HallSensorCtrl.InitialPulses;
}

/* function that returns the state of sensor A */
bool GetHallSensorA_state(void)
{
	return DATA_HallSensorCtrl.HallSensorA_state;
}


/* function that returns the state of sensor B */
bool GetHallSensorB_state(void)
{
	return DATA_HallSensorCtrl.HallSensorB_state;
}


/* function that sets the initial close position after norming the motor */
void SetClosePosition(void)
{
	DATA_HallSensorCtrl.position_tracking_sensor = CLOSE_BLOCK;
}


/* function that return motor direction */
bool GetMotorDirection(void)
{
	return DATA_HallSensorCtrl.DIR;
}


/* function that implements error filtering on pulses from Hall sensors A and B
 * by checking the length of the pulse using timestamps to see if it is a valid pulse or a spike that gets filtered */
void ErrorFiltering(void)
{
	bool sensor_A_state;
	bool sensor_B_state;

	static uint32_t initial_timestamp_A_RisingEdge;
	static uint32_t initial_timestamp_A_FallingEdge;
	static uint32_t initial_timestamp_B_RisingEdge;
	static uint32_t initial_timestamp_B_FallingEdge;

	sensor_A_state = GetHallSensorA_state();
	sensor_B_state = GetHallSensorB_state();

	/* getting initial timestamps when sensor A states change */
	if((!DATA_HallSensorCtrl.HallSensorA_state_prev) && (sensor_A_state)) /* LOW-> HIGH */
	{
		DATA_HallSensorCtrl.reset_block_detection_flag = OFF;
		if(DATA_HallSensorCtrl.invalid_pulse_A_flag) /* previous pulse not valid */
		{
			DATA_HallSensorCtrl.error_tracking++;
		}
		initial_timestamp_A_RisingEdge = MotorCtrl_Main_GetMicros();
		DATA_HallSensorCtrl.HallSensorA_state_prev = sensor_A_state;
	}
	else if((DATA_HallSensorCtrl.HallSensorA_state_prev) && (!sensor_A_state)) /* HIGH -> LOW */
	{
		DATA_HallSensorCtrl.reset_block_detection_flag = OFF;
		if(DATA_HallSensorCtrl.invalid_pulse_A_flag) /* previous pulse not valid */
		{
			DATA_HallSensorCtrl.error_tracking++;
		}
		initial_timestamp_A_FallingEdge = MotorCtrl_Main_GetMicros();
		DATA_HallSensorCtrl.HallSensorA_state_prev = sensor_A_state;
	}
	else
	{

	}

	/* getting initial timestamps when sensor B states change */
	if((!DATA_HallSensorCtrl.HallSensorB_state_prev) && (sensor_B_state)) /* LOW-> HIGH */
	{
		DATA_HallSensorCtrl.reset_block_detection_flag = OFF;
		if(DATA_HallSensorCtrl.invalid_pulse_B_flag) /* previous pulse not valid */
		{
			DATA_HallSensorCtrl.error_tracking++;
		}
		initial_timestamp_B_RisingEdge = MotorCtrl_Main_GetMicros();
		DATA_HallSensorCtrl.HallSensorB_state_prev = sensor_B_state;
	}
	else if((DATA_HallSensorCtrl.HallSensorB_state_prev) && (!sensor_B_state)) /* HIGH -> LOW */
	{
		DATA_HallSensorCtrl.reset_block_detection_flag = OFF;
		if(DATA_HallSensorCtrl.invalid_pulse_B_flag) /* previous pulse not valid */
		{
			DATA_HallSensorCtrl.error_tracking++;
		}
		initial_timestamp_B_FallingEdge = MotorCtrl_Main_GetMicros();
		DATA_HallSensorCtrl.HallSensorB_state_prev = sensor_B_state;
	}
	else
	{

	}

	if(sensor_A_state)
	{
		GetInitialPulses();

		DATA_HallSensorCtrl.timestamp_A_HighState = MotorCtrl_Main_GetMicros(); /* timestamps on HIGH pulse */
		if((DATA_HallSensorCtrl.timestamp_A_HighState - initial_timestamp_A_RisingEdge) > PULSEWIDTH_THRESHOLD)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, (GPIO_PinState)sensor_A_state); /* valid pulses */


			/* flag used for calling function only when the pulse is being validated and not how long the pulse is valid for */
			if(!DATA_HallSensorCtrl.reset_block_detection_flag)
			{
				Reset_block_detection_flag();
				Reset_no_pulses_flag();
				DATA_HallSensorCtrl.reset_block_detection_flag = ON;
			}

			DATA_HallSensorCtrl.filtered_SensorA_pulse = sensor_A_state;
			DATA_HallSensorCtrl.invalid_pulse_A_flag = OFF;
		}
		else /* invalid pulse */
		{
			DATA_HallSensorCtrl.invalid_pulse_A_flag = ON;
		}
	}
	else /* LOW pulse */
	{
		DATA_HallSensorCtrl.timestamp_A_LowState = MotorCtrl_Main_GetMicros(); /* timestamps on LOW pulse */
		if((DATA_HallSensorCtrl.timestamp_A_LowState - initial_timestamp_A_FallingEdge) > PULSEWIDTH_THRESHOLD)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, (GPIO_PinState)sensor_A_state); /* valid pulses */

			if(!DATA_HallSensorCtrl.reset_block_detection_flag)
			{
				Reset_block_detection_flag();
				Reset_no_pulses_flag();
				DATA_HallSensorCtrl.reset_block_detection_flag = ON;
			}

			DATA_HallSensorCtrl.filtered_SensorA_pulse = sensor_A_state;
			DATA_HallSensorCtrl.invalid_pulse_A_flag = OFF;
		}
		else /* invalid pulse */
		{
			DATA_HallSensorCtrl.invalid_pulse_A_flag = ON;
		}
	}

	if(sensor_B_state)
	{
		GetInitialPulses();
		DATA_HallSensorCtrl.timestamp_B_HighState = MotorCtrl_Main_GetMicros(); /* timestamps on HIGH pulse */
		if((DATA_HallSensorCtrl.timestamp_B_HighState - initial_timestamp_B_RisingEdge) > PULSEWIDTH_THRESHOLD)
		{
			if(!DATA_HallSensorCtrl.reset_block_detection_flag)
			{
				Reset_block_detection_flag();
				Reset_no_pulses_flag();
				DATA_HallSensorCtrl.reset_block_detection_flag = ON;
			}

			DATA_HallSensorCtrl.filtered_SensorB_pulse = sensor_B_state;
			DATA_HallSensorCtrl.invalid_pulse_B_flag = OFF;
		}
		else
		{
			DATA_HallSensorCtrl.invalid_pulse_B_flag = ON;
		}
	}
	else
	{
		DATA_HallSensorCtrl.timestamp_B_LowState = MotorCtrl_Main_GetMicros(); /* timestamps on LOW pulse */
		if((DATA_HallSensorCtrl.timestamp_B_LowState - initial_timestamp_B_FallingEdge) > PULSEWIDTH_THRESHOLD)
		{
			if(!DATA_HallSensorCtrl.reset_block_detection_flag)
			{
				Reset_block_detection_flag();
				Reset_no_pulses_flag();
				DATA_HallSensorCtrl.reset_block_detection_flag = ON;
			}

			DATA_HallSensorCtrl.filtered_SensorB_pulse = sensor_B_state;
			DATA_HallSensorCtrl.invalid_pulse_B_flag = OFF;
		}
		else
		{
			DATA_HallSensorCtrl.invalid_pulse_B_flag = ON;
		}
	}
}


/* function that manages the tracking position using Hall sensor pulses */
void Position_Track(void)
{
	if((!DATA_HallSensorCtrl.filtered_SensorA_pulse) && (!DATA_HallSensorCtrl.filtered_SensorB_pulse))
	{
		DATA_HallSensorCtrl.HallSensor_State = HALL_SENSOR_A_LOW_B_LOW;
	}
	else if((!DATA_HallSensorCtrl.filtered_SensorA_pulse) && (DATA_HallSensorCtrl.filtered_SensorB_pulse))
	{
		DATA_HallSensorCtrl.HallSensor_State = HALL_SENSOR_A_LOW_B_HIGH;
	}
	else if((DATA_HallSensorCtrl.filtered_SensorA_pulse) && (DATA_HallSensorCtrl.filtered_SensorB_pulse))
	{
		DATA_HallSensorCtrl.HallSensor_State = HALL_SENSOR_A_HIGH_B_HIGH;
	}
	else if((DATA_HallSensorCtrl.filtered_SensorA_pulse) && (!DATA_HallSensorCtrl.filtered_SensorB_pulse))
	{
		DATA_HallSensorCtrl.HallSensor_State = HALL_SENSOR_A_HIGH_B_LOW;
	}
	else
	{
		/* cannot get here */
	}

	switch(DATA_HallSensorCtrl.HallSensor_State)
	{
		case HALL_SENSOR_A_LOW_B_LOW:
		{
			switch(DATA_HallSensorCtrl.prev_HallSensor_State)
			{
				case HALL_SENSOR_A_LOW_B_HIGH: /* CCW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor--;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CCW;

					break;
				}

				case HALL_SENSOR_A_HIGH_B_HIGH: /* error: jumped over a step */
				{
					break;
				}

				case HALL_SENSOR_A_HIGH_B_LOW: /* CW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor++;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CW;

					break;
				}

				case HALL_SENSOR_A_LOW_B_LOW:
				default:
				{
					break;
				}
			}
			break;
		}

		case HALL_SENSOR_A_LOW_B_HIGH:
		{
			switch(DATA_HallSensorCtrl.prev_HallSensor_State)
			{
				case HALL_SENSOR_A_LOW_B_LOW: /* CW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor++;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CW;

					break;
				}

				case HALL_SENSOR_A_HIGH_B_HIGH: /*  CCW*/
				{
					DATA_HallSensorCtrl.position_tracking_sensor--;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CCW;

					break;
				}

				case HALL_SENSOR_A_HIGH_B_LOW: /* error: jumped over a step */
				{
					break;
				}

				case HALL_SENSOR_A_LOW_B_HIGH:
				default:
				{
					break;
				}
			}
			break;
		}

		case HALL_SENSOR_A_HIGH_B_HIGH:
		{
			switch(DATA_HallSensorCtrl.prev_HallSensor_State)
			{
				case HALL_SENSOR_A_LOW_B_LOW: /* error: jumped over a step */
				{
					break;
				}

				case HALL_SENSOR_A_LOW_B_HIGH: /* CW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor++;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CW;

					break;
				}

				case HALL_SENSOR_A_HIGH_B_LOW: /* CCW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor--;
				    DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CCW;

					break;
				}

				case HALL_SENSOR_A_HIGH_B_HIGH:
				default:
				{
					break;
				}
			}
			break;
		}

		case HALL_SENSOR_A_HIGH_B_LOW:
		{
			switch(DATA_HallSensorCtrl.prev_HallSensor_State)
			{
				case HALL_SENSOR_A_LOW_B_LOW: /* CCW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor--;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CCW;

					break;
				}

				case HALL_SENSOR_A_LOW_B_HIGH: /* error: jumped over a step */
				{
					break;
				}

				case HALL_SENSOR_A_HIGH_B_HIGH: /* CW */
				{
					DATA_HallSensorCtrl.position_tracking_sensor++;
					DATA_HallSensorCtrl.DIR = MOTOR_DIRECTION_CW;

					break;
				}

				case HALL_SENSOR_A_HIGH_B_LOW:
				default:
				{
					break;
				}
			}
			break;
		}
	}

	DATA_HallSensorCtrl.prev_HallSensor_State = DATA_HallSensorCtrl.HallSensor_State;
}


/* function that returns current position of the Hall sensor */
int GetCurrentPosition(void)
{
	return DATA_HallSensorCtrl.position_tracking_sensor;
}
