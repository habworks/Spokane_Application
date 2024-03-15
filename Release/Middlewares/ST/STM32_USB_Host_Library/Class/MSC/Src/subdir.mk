################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c \
../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.c \
../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.c 

OBJS += \
./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.o \
./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.o \
./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.o 

C_DEPS += \
./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.d \
./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.d \
./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/%.o Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/%.su Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/%.cyclo: ../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/%.c Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_CONSOLE -DSTM32L4R5xx -c -I../Core/Inc -I../Drivers/SPI_FLASH/MT29F2G01 -I../Middlewares/Radar_Log_Parser -I../Middlewares/IIS2MDC -I../Middlewares/Partition_Manager -I../Middlewares/I2C -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Inc -I../Middlewares/FAT_FS -I../USB_HOST/Target -I../Middlewares/ST/STM32_USB_Host_Library/HAL/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../USB_DEVICE -I../USB_DEVICE/Target -I../USB_DEVICE/App -I../USB_HOST -I../Drivers -I../Core -I../Scheduler -I../Middlewares -I../Middlewares/Scheduler -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../Middlewares/Third_Party/FatFs/src -I../Drivers/QBG96 -I../Temp -Og -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ST-2f-STM32_USB_Host_Library-2f-Class-2f-MSC-2f-Src

clean-Middlewares-2f-ST-2f-STM32_USB_Host_Library-2f-Class-2f-MSC-2f-Src:
	-$(RM) ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.cyclo ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.d ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.o ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.su ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.cyclo ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.d ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.o ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_bot.su ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.cyclo ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.d ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.o ./Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc_scsi.su

.PHONY: clean-Middlewares-2f-ST-2f-STM32_USB_Host_Library-2f-Class-2f-MSC-2f-Src

