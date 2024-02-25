#ifndef _HEADER_COMMANDCTRL_
#define _HEADER_COMMANDCTRL_

#include "MotorCtrl.h"

void ButtonPressedToCommandDecoder(void);
MOTOR_Commands_Type GetMotorCommand(void);
void CommandCtrl_Init(void);

#endif
