################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/System/motor.c 

OBJS += \
./Core/System/motor.o 

C_DEPS += \
./Core/System/motor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/System/%.o Core/System/%.su Core/System/%.cyclo: ../Core/System/%.c Core/System/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-System

clean-Core-2f-System:
	-$(RM) ./Core/System/motor.cyclo ./Core/System/motor.d ./Core/System/motor.o ./Core/System/motor.su

.PHONY: clean-Core-2f-System

