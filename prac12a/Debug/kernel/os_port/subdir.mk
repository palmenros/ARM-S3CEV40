################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/os_port/os_cpu_c.c 

ASM_SRCS += \
../kernel/os_port/os_cpu_a.asm \
../kernel/os_port/os_cpu_isr_wrapper.asm \
../kernel/os_port/s3c44b0x.asm \
../kernel/os_port/s3cev40.asm 

OBJS += \
./kernel/os_port/os_cpu_a.o \
./kernel/os_port/os_cpu_c.o \
./kernel/os_port/os_cpu_isr_wrapper.o \
./kernel/os_port/s3c44b0x.o \
./kernel/os_port/s3cev40.o 

C_DEPS += \
./kernel/os_port/os_cpu_c.d 

ASM_DEPS += \
./kernel/os_port/os_cpu_a.d \
./kernel/os_port/os_cpu_isr_wrapper.d \
./kernel/os_port/s3c44b0x.d \
./kernel/os_port/s3cev40.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/os_port/%.o: ../kernel/os_port/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel/os_port/%.o: ../kernel/os_port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


