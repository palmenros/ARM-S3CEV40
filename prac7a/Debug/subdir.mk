################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../D8Led.c \
../at24c04.c \
../button.c \
../font8x16.c \
../gpio.c \
../iic.c \
../intcontroller.c \
../keyboard.c \
../lcd.c \
../leds.c \
../main.c \
../rtc.c \
../timer.c \
../uart.c \
../utils.c 

S_UPPER_SRCS += \
../init.S 

OBJS += \
./D8Led.o \
./at24c04.o \
./button.o \
./font8x16.o \
./gpio.o \
./iic.o \
./init.o \
./intcontroller.o \
./keyboard.o \
./lcd.o \
./leds.o \
./main.o \
./rtc.o \
./timer.o \
./uart.o \
./utils.o 

C_DEPS += \
./D8Led.d \
./at24c04.d \
./button.d \
./font8x16.d \
./gpio.d \
./iic.d \
./intcontroller.d \
./keyboard.d \
./lcd.d \
./leds.d \
./main.d \
./rtc.d \
./timer.d \
./uart.d \
./utils.d 

S_UPPER_DEPS += \
./init.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


