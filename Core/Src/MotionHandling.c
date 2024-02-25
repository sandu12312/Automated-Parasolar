#include "MotionHandling.h"
#include "main.h"
#include "HallSensorsCtrl.h"
#include "SwitchHandling.h"
#include "GPIO.h"
#include "MotorCtrl.h"
#include "CommandCtrl.h"
#include "MotorCtrl_Main.h"
#include "HallSensorsCtrl.h"
#include "MotorCtrl.h"


#define MIN_CURRENT         100u    /* current in mA */
#define CURRENT_THRESHOLD   1500u
#define NO_PULSES_INTERVAL  200000u /* microseconds */

#define MOTOR_CURRENT_BLOCK 2100
#define MOTOR_STALL_TIME    200000u
#define MAX_CURRENT 		1500u
#define TIME_PASSED 		500u


typedef struct MotionHandling_DATA{
		int target_position_open_start;
		int target_position_close_start;
		int target_position_open_stop;
		int target_position_close_stop;
		int current_position;

		bool target_start_position_flag;
		bool target_stop_position_flag;
		bool motor_direction;
		bool isNormed;

		bool block_detection_flag;
		bool flag_current_changed;
		bool flag_motor_normed;
		bool block_detected;
		bool no_pulses_flag;
		bool Hall_sensor_error;

		uint32_t PWM_dutycycle;
		uint16_t maxCurrent;
		uint32_t timestamp_difference;
		uint32_t current_in_mA;
		uint32_t initial_timestamp;
		uint32_t current_timestamp;
		uint32_t prev_time;
		uint32_t current_time;
		uint32_t difference_time;

		uint32_t initial_t;
		uint32_t current_t;
		uint32_t current_value;
		uint32_t diff;

		MOTOR_Commands_Type check_motor_cmd;

}MotionHandling_DATA;

MotionHandling_DATA DATA_MotionHandling;


/* init function for module variables */
void MotionHandling_Init(void)
{
	DATA_MotionHandling.target_position_open_start  = 0;
	DATA_MotionHandling.target_position_close_start = 0;
	DATA_MotionHandling.target_position_open_stop   = 0;
	DATA_MotionHandling.target_position_close_stop  = 0;
	DATA_MotionHandling.current_position            = 0;

	DATA_MotionHandling.target_start_position_flag  = OFF;
	DATA_MotionHandling.target_stop_position_flag   = OFF;
	DATA_MotionHandling.motor_direction             = OFF;
	DATA_MotionHandling.isNormed 				    = OFF;

	DATA_MotionHandling.block_detection_flag 	    = OFF;
	DATA_MotionHandling.flag_current_changed 	    = OFF;
	DATA_MotionHandling.flag_motor_normed 	 	    = OFF;
	DATA_MotionHandling.block_detected 			    = OFF;
	DATA_MotionHandling.no_pulses_flag			    = OFF;
	DATA_MotionHandling.Hall_sensor_error 		    = OFF;

	DATA_MotionHandling.PWM_dutycycle 			= 0u;
	DATA_MotionHandling.maxCurrent 	         	= 0u;
	DATA_MotionHandling.timestamp_difference 	= 0u;
	DATA_MotionHandling.current_in_mA     	 	= 0u;
	DATA_MotionHandling.initial_timestamp    	= 0u;
	DATA_MotionHandling.current_timestamp   	= 0u;
	DATA_MotionHandling.prev_time            	= 0u;
	DATA_MotionHandling.current_time         	= 0u;
	DATA_MotionHandling.difference_time 	 	= 0u;

	DATA_MotionHandling.check_motor_cmd = MOTOR_Command_OFF;

	DATA_MotionHandling.initial_t 				= 0u;
	DATA_MotionHandling.current_t 				= 0u;
	DATA_MotionHandling.current_value 			= 0u;
	DATA_MotionHandling.diff 					= 0u;
}


/* this flag is reset in pulses interrupts to signal that we get pulses from Hall sensors
 * it is used in detection of Hall Sensor error */
void Reset_no_pulses_flag(void)
{
	 DATA_MotionHandling.no_pulses_flag = OFF;
}


/* function that signals Hall sensor error by checking if there is no pulse feedback from Hall sensors
 * when such error is detected, motor is denormed
 * this function is used in block detection in order to have reverse motion even when there are no pulses */
void HallSensorError(void)
{
	DATA_MotionHandling.current_value = ReadCurrent();

	if(((DATA_MotionHandling.current_value > MIN_CURRENT) && (DATA_MotionHandling.current_value < CURRENT_THRESHOLD)) && (DATA_MotionHandling.no_pulses_flag)) /* motor is running */
	{
		DATA_MotionHandling.current_t = MotorCtrl_Main_GetMicros();
		DATA_MotionHandling.diff = DATA_MotionHandling.current_t - DATA_MotionHandling.initial_t;

		if((DATA_MotionHandling.diff >= NO_PULSES_INTERVAL))
		{
			DATA_MotionHandling.Hall_sensor_error = ON;
			DATA_MotionHandling.block_detection_flag = OFF;
			DATA_MotionHandling.flag_motor_normed = OFF; /* denorming motor when we have no pulses */
		}
		else
		{
			DATA_MotionHandling.Hall_sensor_error = OFF;
		}
	}
	else if(!DATA_MotionHandling.no_pulses_flag)
	{
		DATA_MotionHandling.Hall_sensor_error = OFF;
		DATA_MotionHandling.initial_t = MotorCtrl_Main_GetMicros();
	}
	else
	{
		DATA_MotionHandling.initial_t = MotorCtrl_Main_GetMicros();
	}


	DATA_MotionHandling.no_pulses_flag = ON;
}


/* function that resets block detection flag in pulses interrupts
 * this checks that we have sensor pulses */
void Reset_block_detection_flag(void)
{
	DATA_MotionHandling.block_detection_flag = OFF;
}


/* this function returns block_detected flag to motor commands so that it performs reverse motion when needed */
bool GetBlockDetected_flag(void)
{
	return DATA_MotionHandling.block_detected;
}


/* this flag is reset when reverse motion has finished after a block detection */
void ResetBlockDetected_flag(void)
{
	 DATA_MotionHandling.block_detected = OFF;
}


/* function that returns the normed or denormed state of the motor
 * we use it in soft start and soft stop functions for guarding these functionalities for normed state only */
bool isMotorNormed(void)
{
	return DATA_MotionHandling.flag_motor_normed;
}


/* function that handles block detection occurrence in normed and denormed motor state
 *  we get feedback if we have Hall sensor error so that we treat the cases separately: block detection when there are pulses or when we have Hall error detected
 *  this function sets a flag for block detected that is used for setting further motor commands */
void BlockDetection(void)
{
	DATA_MotionHandling.current_in_mA   = ReadCurrent();
	DATA_MotionHandling.check_motor_cmd = GetMotorCommand();

#define USE_BLOCK_NORMING_IN_CLS_ONLY
#ifdef USE_BLOCK_NORMING_IN_CLS_ONLY
	if(MOTOR_Command_MAN_CLS == DATA_MotionHandling.check_motor_cmd)
#elif USE_BLOCK_NORMING_IN_BOTH_DIR
		if((MOTOR_Command_MAN_CLS == check_motor_cmd) || (MOTOR_Command_MAN_OPN == check_motor_cmd))
#endif
		{
			DATA_MotionHandling.current_timestamp = MotorCtrl_Main_GetMicros();

			/* check if flag is reset in pulse interrupts to get the initial timestamp */
			if(!DATA_MotionHandling.block_detection_flag)
			{
				DATA_MotionHandling.initial_timestamp = DATA_MotionHandling.current_timestamp; /* start of no pulse interval */
			}

			DATA_MotionHandling.block_detection_flag = ON;

			DATA_MotionHandling.timestamp_difference = DATA_MotionHandling.current_timestamp - DATA_MotionHandling.initial_timestamp;

			DATA_MotionHandling.maxCurrent = ((DATA_MotionHandling.current_in_mA > DATA_MotionHandling.maxCurrent) ? DATA_MotionHandling.current_in_mA : DATA_MotionHandling.maxCurrent);

			if(DATA_MotionHandling.Hall_sensor_error) /* there were no pulses before block detection */
			{

				if(DATA_MotionHandling.current_in_mA > MAX_CURRENT)
				{
					DATA_MotionHandling.current_time =  HAL_GetTick();
					DATA_MotionHandling.difference_time = DATA_MotionHandling.current_time - DATA_MotionHandling.prev_time;

					if(DATA_MotionHandling.difference_time > TIME_PASSED)
					{
						DATA_MotionHandling.block_detected = ON;
					}
				}
				else
				{
					DATA_MotionHandling.block_detection_flag = OFF;
					DATA_MotionHandling.prev_time = HAL_GetTick();
				}
			}
			else /* block detection based on sensor pulses */
			{
				if((DATA_MotionHandling.timestamp_difference >= MOTOR_STALL_TIME) &&
						(DATA_MotionHandling.current_in_mA >= MOTOR_CURRENT_BLOCK))
				{
					DATA_MotionHandling.block_detected = ON;

					if(!DATA_MotionHandling.flag_motor_normed)
					{
						SetClosePosition(); /* first block detection (CLOSE BLOCK) - motor is being normed */
						DATA_MotionHandling.flag_motor_normed = ON;
					}
				}
			}
		}
}


/* function that implements soft start by gradually increasing the pwm in a specified range based on number of pulses
 * if the motor is denormed, we set the pwm to maximum */
void StartMotion(bool DIR)
{
	DATA_MotionHandling.target_stop_position_flag = OFF;
	DATA_MotionHandling.current_position = GetCurrentPosition();
	DATA_MotionHandling.isNormed = isMotorNormed();

	if(MOTOR_DIRECTION_CW == DIR) /* CLOSE_BLOCK -> OPEN_BLOCK */
	{
		if(DATA_MotionHandling.isNormed)
		{
			if((DATA_MotionHandling.current_position >= OPEN_POSITION - POSITION_CATCH_RANGE) &&
					(DATA_MotionHandling.current_position <= OPEN_POSITION + POSITION_CATCH_RANGE))
			{
				DATA_MotionHandling.target_position_open_start = 0;
				DATA_MotionHandling.PWM_dutycycle = 0;
				TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
			}
			else
			{
				if(DATA_MotionHandling.target_start_position_flag == OFF)
				{
					DATA_MotionHandling.target_position_open_start = DATA_MotionHandling.current_position + MOTOR_SOFT_START_POS_LENGTH;
					DATA_MotionHandling.target_start_position_flag = ON;
				}
			}
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.target_position_open_start >= OPEN_POSITION))
		{
			DATA_MotionHandling.target_position_open_start = OPEN_POSITION;
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.current_position < DATA_MotionHandling.target_position_open_start) && (DATA_MotionHandling.PWM_dutycycle <= MAX_PWM))
		{
			if((DATA_MotionHandling.target_position_open_start - DATA_MotionHandling.current_position) > ((MOTOR_SOFT_START_POS_LENGTH * 4) / 5)) /* 80% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 10) / 10;  /* 30% pwm max */
			}
			else if((DATA_MotionHandling.target_position_open_start - DATA_MotionHandling.current_position) > ((MOTOR_SOFT_START_POS_LENGTH * 3) / 5)) /* 60% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 2) / 5;  /* 40% pwm max */
			}
			else if((DATA_MotionHandling.target_position_open_start - DATA_MotionHandling.current_position) > ((MOTOR_SOFT_START_POS_LENGTH * 2) / 5)) /* 40% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 3) / 5;  /* 60% pwm max */
			}
			else if((DATA_MotionHandling.target_position_open_start - DATA_MotionHandling.current_position) > (MOTOR_SOFT_START_POS_LENGTH / 5)) /* 20% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 4) / 5;  /* 80% pwm max */
			}
			else
			{
				DATA_MotionHandling.PWM_dutycycle = MAX_PWM;
			}

			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
		else
		{
			DATA_MotionHandling.PWM_dutycycle = MAX_PWM;
			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
	}
	else if(MOTOR_DIRECTION_CCW == DIR) /* OPEN_BLOCK -> CLOSE_BLOCK */
	{
		if(DATA_MotionHandling.isNormed)
		{
			if((DATA_MotionHandling.current_position >= CLOSE_POSITION - POSITION_CATCH_RANGE) &&
					(DATA_MotionHandling.current_position <= CLOSE_POSITION + POSITION_CATCH_RANGE))
			{
				DATA_MotionHandling.target_position_close_start = 0;
				DATA_MotionHandling.PWM_dutycycle = 0;
				TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
			}
			else
			{
				if(DATA_MotionHandling.target_start_position_flag == OFF)
				{
					DATA_MotionHandling.target_position_close_start = DATA_MotionHandling.current_position + MOTOR_SOFT_START_POS_LENGTH;
					DATA_MotionHandling.target_start_position_flag = ON;
				}
			}

			DATA_MotionHandling.target_start_position_flag = ON;
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.target_position_close_start <= CLOSE_POSITION))
		{
			DATA_MotionHandling.target_position_close_start = CLOSE_POSITION;
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.current_position > DATA_MotionHandling.target_position_close_start) && (DATA_MotionHandling.PWM_dutycycle <= MAX_PWM))
		{
			if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_start) > ((MOTOR_SOFT_START_POS_LENGTH * 4) / 5)) /* 80% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 10) / 10;  /* 30% pwm max */
			}
			else if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_start) > ((MOTOR_SOFT_START_POS_LENGTH * 3) / 5)) /* 60% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 2) / 5;  /* 40% pwm max */
			}
			else if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_start) > ((MOTOR_SOFT_START_POS_LENGTH * 2) / 5)) /* 40% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 3) / 5;  /* 60% pwm max */
			}
			else if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_start) > (MOTOR_SOFT_START_POS_LENGTH / 5)) /* 20% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 4) / 5;  /* 80% pwm max */
			}
			else
			{
				DATA_MotionHandling.PWM_dutycycle = MAX_PWM;
			}

			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
		else
		{
			DATA_MotionHandling.PWM_dutycycle = MAX_PWM;
			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
	}
	else
	{
		/* MISRA RULES */
	}
}


/* function that implements soft stop by gradually decreasing the pwm in a specified range based on number of pulses
 * if the motor is denormed, we set the pwm to zero */
void StopMotion()
{
	DATA_MotionHandling.motor_direction = GetMotorDirection();
	DATA_MotionHandling.target_start_position_flag = OFF;
	DATA_MotionHandling.current_position = GetCurrentPosition();

	if(MOTOR_DIRECTION_CW == DATA_MotionHandling.motor_direction) /* CLOSE_BLOCK -> OPEN_BLOCK */
	{
		if(DATA_MotionHandling.isNormed)
		{
			if((DATA_MotionHandling.current_position >= OPEN_POSITION - POSITION_CATCH_RANGE) &&
					(DATA_MotionHandling.current_position <= OPEN_POSITION + POSITION_CATCH_RANGE))
			{
				DATA_MotionHandling.target_position_open_stop = 0;
				DATA_MotionHandling.PWM_dutycycle = 0;
				TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
			}
			else
			{
				if(DATA_MotionHandling.target_stop_position_flag == OFF)
				{
					DATA_MotionHandling.target_position_open_stop = DATA_MotionHandling.current_position + MOTOR_SOFT_STOP_POS_LENGTH;
					DATA_MotionHandling.target_start_position_flag = ON;
				}
			}

			DATA_MotionHandling.target_stop_position_flag = ON;
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.target_position_open_stop >= OPEN_POSITION))
		{
			DATA_MotionHandling.target_position_open_stop = OPEN_POSITION;
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.current_position < DATA_MotionHandling.target_position_open_stop) && (DATA_MotionHandling.PWM_dutycycle > MIN_PWM))
		{
			if((DATA_MotionHandling.target_position_open_stop - DATA_MotionHandling.current_position) > ((MOTOR_SOFT_STOP_POS_LENGTH * 4) / 5)) /* 80% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 4) / 5;  /* 80% pwm max */
			}
			else if((DATA_MotionHandling.target_position_open_stop - DATA_MotionHandling.current_position) > ((MOTOR_SOFT_STOP_POS_LENGTH * 3) / 5)) /* 60% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 3) / 5;  /* 60% pwm max */
			}
			else if((DATA_MotionHandling.target_position_open_stop - DATA_MotionHandling.current_position) > ((MOTOR_SOFT_STOP_POS_LENGTH * 2) / 5)) /* 40% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 2) / 5;  /* 40% pwm max */
			}
			else if((DATA_MotionHandling.target_position_open_stop - DATA_MotionHandling.current_position) > (MOTOR_SOFT_STOP_POS_LENGTH / 5)) /* 20% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 10) / 10;  /* 30% pwm max */
			}
			else
			{
				DATA_MotionHandling.PWM_dutycycle = MIN_PWM;
			}

			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
		else
		{
			DATA_MotionHandling.PWM_dutycycle = 0;
			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
	}
	else if(MOTOR_DIRECTION_CCW == DATA_MotionHandling.motor_direction) /* OPEN_BLOCK -> CLOSE_BLOCK */
	{
		if(DATA_MotionHandling.isNormed)
		{
			if((DATA_MotionHandling.current_position >= CLOSE_POSITION - POSITION_CATCH_RANGE) &&
					(DATA_MotionHandling.current_position <= CLOSE_POSITION + POSITION_CATCH_RANGE))
			{
				DATA_MotionHandling.target_position_close_stop = 0;
				DATA_MotionHandling.PWM_dutycycle = 0;
				TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
			}
			else
			{
				if(DATA_MotionHandling.target_stop_position_flag == OFF)
				{
		 			DATA_MotionHandling.target_position_close_stop = ((DATA_MotionHandling.current_position > MOTOR_SOFT_STOP_POS_LENGTH) ?
							(DATA_MotionHandling.current_position - MOTOR_SOFT_STOP_POS_LENGTH) :
							(MOTOR_SOFT_STOP_POS_LENGTH));
					DATA_MotionHandling.target_start_position_flag = ON;
				}
			}
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.target_position_close_stop <= CLOSE_POSITION))
		{
			DATA_MotionHandling.target_position_close_stop = CLOSE_POSITION;
		}

		if((DATA_MotionHandling.isNormed) && (DATA_MotionHandling.current_position > DATA_MotionHandling.target_position_close_stop) && (DATA_MotionHandling.PWM_dutycycle > MIN_PWM))
		{
			if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_stop) > ((MOTOR_SOFT_STOP_POS_LENGTH * 4) / 5)) /* 80% */
					{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 4) / 5;  /* 80% pwm max */
					}
			else if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_stop) > ((MOTOR_SOFT_STOP_POS_LENGTH * 3) / 5)) /* 60% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 3) / 5;  /* 60% pwm max */
			}
			else if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_stop) > ((MOTOR_SOFT_STOP_POS_LENGTH * 2) / 5)) /* 40% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 2) / 5;  /* 40% pwm max */
			}
			else if((DATA_MotionHandling.current_position - DATA_MotionHandling.target_position_close_stop) > (MOTOR_SOFT_STOP_POS_LENGTH / 5)) /* 20% */
			{
				DATA_MotionHandling.PWM_dutycycle = (MAX_PWM * 10) / 10;  /* 30% pwm max */
			}
			else
			{
				DATA_MotionHandling.PWM_dutycycle = MIN_PWM;
			}

			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
		else
		{
			DATA_MotionHandling.PWM_dutycycle = 0;
			TIM1->CCR1 = DATA_MotionHandling.PWM_dutycycle;
		}
	}
	else
	{

	}
}
