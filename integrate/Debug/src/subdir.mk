################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/nmea.c \
../src/syscalls.c \
../src/system_stm32l1xx.c 

OBJS += \
./src/main.o \
./src/nmea.o \
./src/syscalls.o \
./src/system_stm32l1xx.o 

C_DEPS += \
./src/main.d \
./src/nmea.d \
./src/syscalls.d \
./src/system_stm32l1xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RCTx -DSTM32L152C_DISCO -DDEBUG -DSTM32L1XX_MDP -DUSE_STDPERIPH_DRIVER -I"/Users/chibuikemohanele/Google Drive/eceCode/ECE477/integrate/Utilities" -I"/Users/chibuikemohanele/Google Drive/eceCode/ECE477/integrate/StdPeriph_Driver/inc" -I"/Users/chibuikemohanele/Google Drive/eceCode/ECE477/integrate/inc" -I"/Users/chibuikemohanele/Google Drive/eceCode/ECE477/integrate/CMSIS/device" -I"/Users/chibuikemohanele/Google Drive/eceCode/ECE477/integrate/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


