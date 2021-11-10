################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/gpio.c \
../Core/Src/iwdg.c \
../Core/Src/main.c \
../Core/Src/max7219.c \
../Core/Src/rtc.c \
../Core/Src/spi.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/gpio.o \
./Core/Src/iwdg.o \
./Core/Src/main.o \
./Core/Src/max7219.o \
./Core/Src/rtc.o \
./Core/Src/spi.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/gpio.d \
./Core/Src/iwdg.d \
./Core/Src/main.d \
./Core/Src/max7219.d \
./Core/Src/rtc.d \
./Core/Src/spi.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Core/Inc -I/Users/kittaphorn/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.4/Drivers/STM32F1xx_HAL_Driver/Inc -I/Users/kittaphorn/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.4/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I/Users/kittaphorn/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.4/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I/Users/kittaphorn/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.4/Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

