################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/MCXA153_Project3.c \
../source/boot_anim.c \
../source/botton.c \
../source/buzzer.c \
../source/display.c \
../source/game.c \
../source/gfx.c \
../source/keypad.c \
../source/semihost_hardfault.c \
../source/systemUI.c 

C_DEPS += \
./source/MCXA153_Project3.d \
./source/boot_anim.d \
./source/botton.d \
./source/buzzer.d \
./source/display.d \
./source/game.d \
./source/gfx.d \
./source/keypad.d \
./source/semihost_hardfault.d \
./source/systemUI.d 

OBJS += \
./source/MCXA153_Project3.o \
./source/boot_anim.o \
./source/botton.o \
./source/buzzer.o \
./source/display.o \
./source/game.o \
./source/gfx.o \
./source/keypad.o \
./source/semihost_hardfault.o \
./source/systemUI.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MCXA153VLH -DCPU_MCXA153VLH_cm33_nodsp -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"D:\ProgramData\MCUX\MCXA153_Project3\component\lists" -I"D:\ProgramData\MCUX\MCXA153_Project3\drivers" -I"D:\ProgramData\MCUX\MCXA153_Project3\CMSIS" -I"D:\ProgramData\MCUX\MCXA153_Project3\CMSIS\m-profile" -I"D:\ProgramData\MCUX\MCXA153_Project3\device" -I"D:\ProgramData\MCUX\MCXA153_Project3\device\periph" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities" -I"D:\ProgramData\MCUX\MCXA153_Project3\component\serial_manager" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities\debug_console" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities\str" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities\debug_console\config" -I"D:\ProgramData\MCUX\MCXA153_Project3\component\uart" -I"D:\ProgramData\MCUX\MCXA153_Project3\board" -I"D:\ProgramData\MCUX\MCXA153_Project3\source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33+nodsp -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/MCXA153_Project3.d ./source/MCXA153_Project3.o ./source/boot_anim.d ./source/boot_anim.o ./source/botton.d ./source/botton.o ./source/buzzer.d ./source/buzzer.o ./source/display.d ./source/display.o ./source/game.d ./source/game.o ./source/gfx.d ./source/gfx.o ./source/keypad.d ./source/keypad.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o ./source/systemUI.d ./source/systemUI.o

.PHONY: clean-source

