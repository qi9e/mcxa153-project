################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/tetris/src/music.c \
../source/tetris/src/tetris.c \
../source/tetris/src/tetronimo.c 

C_DEPS += \
./source/tetris/src/music.d \
./source/tetris/src/tetris.d \
./source/tetris/src/tetronimo.d 

OBJS += \
./source/tetris/src/music.o \
./source/tetris/src/tetris.o \
./source/tetris/src/tetronimo.o 


# Each subdirectory must supply rules for building sources it contributes
source/tetris/src/%.o: ../source/tetris/src/%.c source/tetris/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MCXA153VLH -DCPU_MCXA153VLH_cm33_nodsp -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"D:\ProgramData\MCUX\MCXA153_Project3\component\lists" -I"D:\ProgramData\MCUX\MCXA153_Project3\drivers" -I"D:\ProgramData\MCUX\MCXA153_Project3\CMSIS" -I"D:\ProgramData\MCUX\MCXA153_Project3\CMSIS\m-profile" -I"D:\ProgramData\MCUX\MCXA153_Project3\device" -I"D:\ProgramData\MCUX\MCXA153_Project3\device\periph" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities" -I"D:\ProgramData\MCUX\MCXA153_Project3\component\serial_manager" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities\debug_console" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities\str" -I"D:\ProgramData\MCUX\MCXA153_Project3\utilities\debug_console\config" -I"D:\ProgramData\MCUX\MCXA153_Project3\component\uart" -I"D:\ProgramData\MCUX\MCXA153_Project3\board" -I"D:\ProgramData\MCUX\MCXA153_Project3\source" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33+nodsp -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-tetris-2f-src

clean-source-2f-tetris-2f-src:
	-$(RM) ./source/tetris/src/music.d ./source/tetris/src/music.o ./source/tetris/src/tetris.d ./source/tetris/src/tetris.o ./source/tetris/src/tetronimo.d ./source/tetris/src/tetronimo.o

.PHONY: clean-source-2f-tetris-2f-src

