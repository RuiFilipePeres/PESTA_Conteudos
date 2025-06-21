################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/DMA_config.c \
../src/GPIO_init.c \
../src/TIM_config.c \
../src/USART_config.c \
../src/adc_config.c \
../src/main.c \
../src/stm32f4xx_it.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/DMA_config.o \
./src/GPIO_init.o \
./src/TIM_config.o \
./src/USART_config.o \
./src/adc_config.o \
./src/main.o \
./src/stm32f4xx_it.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/DMA_config.d \
./src/GPIO_init.d \
./src/TIM_config.d \
./src/USART_config.d \
./src/adc_config.d \
./src/main.d \
./src/stm32f4xx_it.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F410RBTx -DNUCLEO_F410RB -DDEBUG -DSTM32F410Rx -I"C:/PROJETO_FINAL/SystemWorkBench/PESTA_FINAL/inc" -I"C:/PROJETO_FINAL/SystemWorkBench/PESTA_FINAL/CMSIS/device" -I"C:/PROJETO_FINAL/SystemWorkBench/PESTA_FINAL/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


