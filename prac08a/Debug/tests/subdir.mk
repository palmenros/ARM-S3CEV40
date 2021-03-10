################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tests/test_EEPROM.c \
../tests/test_WAV.c \
../tests/test_sound_effects.c 

OBJS += \
./tests/test_EEPROM.o \
./tests/test_WAV.o \
./tests/test_sound_effects.o 

C_DEPS += \
./tests/test_EEPROM.d \
./tests/test_WAV.d \
./tests/test_sound_effects.d 


# Each subdirectory must supply rules for building sources it contributes
tests/%.o: ../tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


