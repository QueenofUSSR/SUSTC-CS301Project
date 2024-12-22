################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/SYSTEM/Server.c \
../Core/SYSTEM/UltrasonicWave.c \
../Core/SYSTEM/motor.c 

OBJS += \
./Core/SYSTEM/Server.o \
./Core/SYSTEM/UltrasonicWave.o \
./Core/SYSTEM/motor.o 

C_DEPS += \
./Core/SYSTEM/Server.d \
./Core/SYSTEM/UltrasonicWave.d \
./Core/SYSTEM/motor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/SYSTEM/%.o Core/SYSTEM/%.su Core/SYSTEM/%.cyclo: ../Core/SYSTEM/%.c Core/SYSTEM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Core/SYSTEM -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-SYSTEM

clean-Core-2f-SYSTEM:
	-$(RM) ./Core/SYSTEM/Server.cyclo ./Core/SYSTEM/Server.d ./Core/SYSTEM/Server.o ./Core/SYSTEM/Server.su ./Core/SYSTEM/UltrasonicWave.cyclo ./Core/SYSTEM/UltrasonicWave.d ./Core/SYSTEM/UltrasonicWave.o ./Core/SYSTEM/UltrasonicWave.su ./Core/SYSTEM/motor.cyclo ./Core/SYSTEM/motor.d ./Core/SYSTEM/motor.o ./Core/SYSTEM/motor.su

.PHONY: clean-Core-2f-SYSTEM

