################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Scheduler/Scheduler.c 

OBJS += \
./Middlewares/Scheduler/Scheduler.o 

C_DEPS += \
./Middlewares/Scheduler/Scheduler.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Scheduler/%.o Middlewares/Scheduler/%.su Middlewares/Scheduler/%.cyclo: ../Middlewares/Scheduler/%.c Middlewares/Scheduler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4R5xx -c -I../Core/Inc -I../FATFS/App -I../Drivers/SPI_FLASH/MT29F2G01 -I../Core -I../Middlewares/Third_Party/FatFs/src -I../Middlewares -I../ -IMiddlewares/Scheduler -I../UART -I../Console -I../Queue -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../USB_HOST/App -I../USB_HOST/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-Scheduler

clean-Middlewares-2f-Scheduler:
	-$(RM) ./Middlewares/Scheduler/Scheduler.cyclo ./Middlewares/Scheduler/Scheduler.d ./Middlewares/Scheduler/Scheduler.o ./Middlewares/Scheduler/Scheduler.su

.PHONY: clean-Middlewares-2f-Scheduler

