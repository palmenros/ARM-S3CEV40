################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../D8Led.c \
../button.c \
../gpio.c \
../intcontroller.c \
../keyboard.c \
../leds.c \
../main.c \
../rtc.c \
../timer.c \
../uart.c \
../utils.c 

ASM_SRCS += \
../bootloader.asm \
../s3c44b0x.asm \
../s3cev40.asm 

OBJS += \
./D8Led.o \
./bootloader.o \
./button.o \
./gpio.o \
./intcontroller.o \
./keyboard.o \
./leds.o \
./main.o \
./rtc.o \
./s3c44b0x.o \
./s3cev40.o \
./timer.o \
./uart.o \
./utils.o 

C_DEPS += \
./D8Led.d \
./button.d \
./gpio.d \
./intcontroller.d \
./keyboard.d \
./leds.d \
./main.d \
./rtc.d \
./timer.d \
./uart.d \
./utils.d 

ASM_DEPS += \
./bootloader.d \
./s3c44b0x.d \
./s3cev40.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


