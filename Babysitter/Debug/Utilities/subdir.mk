################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Utilities/stm32l_discovery_lcd.c 

OBJS += \
./Utilities/stm32l_discovery_lcd.o 

C_DEPS += \
./Utilities/stm32l_discovery_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Utilities/%.o: ../Utilities/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RCTx -DSTM32L152C_DISCO -DDEBUG -DSTM32L1XX_MDP -DUSE_STDPERIPH_DRIVER -I"/Users/anonymousotaku/Documents/workspace/FuelGuage/Utilities" -I"/Users/anonymousotaku/Documents/workspace/FuelGuage/StdPeriph_Driver/inc" -I"/Users/anonymousotaku/Documents/workspace/FuelGuage/inc" -I"/Users/anonymousotaku/Documents/workspace/FuelGuage/CMSIS/device" -I"/Users/anonymousotaku/Documents/workspace/FuelGuage/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


