#ifndef _HEADER_MOTIONHANDLING_
#define _HEADER_MOTIONHANDLING_


#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "HallSensorsCtrl.h"

#define CLOSE_BLOCK 0
#define OPEN_BLOCK  5000
#define POSITION_CATCH_RANGE 25

#define CLOSE_POS_OFFSET 50
#define OPEN_POS_OFFSET  50

#define MOTOR_SOFT_START_POS_LENGTH  50
#define MOTOR_SOFT_STOP_POS_LENGTH   50

#define MAX_PWM 90u
#define MIN_PWM 10u

#define CLOSE_POSITION (CLOSE_BLOCK + CLOSE_POS_OFFSET)
#define OPEN_POSITION  (OPEN_BLOCK - OPEN_POS_OFFSET)


void MotionHandling_Init(void);

void Reset_no_pulses_flag(void);
void HallSensorError(void);

void Reset_block_detection_flag(void);
bool GetBlockDetected_flag(void);
void ResetBlockDetected_flag(void);
bool isMotorNormed(void);
void BlockDetection(void);
bool return_InitialPulses(void);
void StartMotion(bool DIR);
void StopMotion(void);



#endif
