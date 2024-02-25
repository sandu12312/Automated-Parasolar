################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/CommandCtrl.c \
../Core/Src/GPIO.c \
../Core/Src/HallSensorsCtrl.c \
../Core/Src/InterruptHandling.c \
../Core/Src/MotionHandling.c \
../Core/Src/MotorCtrl.c \
../Core/Src/MotorCtrl_Main.c \
../Core/Src/SwitchHandling.c \
../Core/Src/main.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/CommandCtrl.o \
./Core/Src/GPIO.o \
./Core/Src/HallSensorsCtrl.o \
./Core/Src/InterruptHandling.o \
./Core/Src/MotionHandling.o \
./Core/Src/MotorCtrl.o \
./Core/Src/MotorCtrl_Main.o \
./Core/Src/SwitchHandling.o \
./Core/Src/main.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/CommandCtrl.d \
./Core/Src/GPIO.d \
./Core/Src/HallSensorsCtrl.d \
./Core/Src/InterruptHandling.d \
./Core/Src/MotionHandling.d \
./Core/Src/MotorCtrl.d \
./Core/Src/MotorCtrl_Main.d \
./Core/Src/SwitchHandling.d \
./Core/Src/main.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/CommandCtrl.cyclo ./Core/Src/CommandCtrl.d ./Core/Src/CommandCtrl.o ./Core/Src/CommandCtrl.su ./Core/Src/GPIO.cyclo ./Core/Src/GPIO.d ./Core/Src/GPIO.o ./Core/Src/GPIO.su ./Core/Src/HallSensorsCtrl.cyclo ./Core/Src/HallSensorsCtrl.d ./Core/Src/HallSensorsCtrl.o ./Core/Src/HallSensorsCtrl.su ./Core/Src/InterruptHandling.cyclo ./Core/Src/InterruptHandling.d ./Core/Src/InterruptHandling.o ./Core/Src/InterruptHandling.su ./Core/Src/MotionHandling.cyclo ./Core/Src/MotionHandling.d ./Core/Src/MotionHandling.o ./Core/Src/MotionHandling.su ./Core/Src/MotorCtrl.cyclo ./Core/Src/MotorCtrl.d ./Core/Src/MotorCtrl.o ./Core/Src/MotorCtrl.su ./Core/Src/MotorCtrl_Main.cyclo ./Core/Src/MotorCtrl_Main.d ./Core/Src/MotorCtrl_Main.o ./Core/Src/MotorCtrl_Main.su ./Core/Src/SwitchHandling.cyclo ./Core/Src/SwitchHandling.d ./Core/Src/SwitchHandling.o ./Core/Src/SwitchHandling.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

