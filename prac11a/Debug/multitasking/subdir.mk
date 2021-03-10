################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../multitasking/cyclic.c \
../multitasking/foreback.c \
../multitasking/kernelcoop.c \
../multitasking/non_expropiative.c \
../multitasking/queue.c 

OBJS += \
./multitasking/cyclic.o \
./multitasking/foreback.o \
./multitasking/kernelcoop.o \
./multitasking/non_expropiative.o \
./multitasking/queue.o 

C_DEPS += \
./multitasking/cyclic.d \
./multitasking/foreback.d \
./multitasking/kernelcoop.d \
./multitasking/non_expropiative.d \
./multitasking/queue.d 


# Each subdirectory must supply rules for building sources it contributes
multitasking/%.o: ../multitasking/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


