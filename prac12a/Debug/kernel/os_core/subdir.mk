################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kernel/os_core/os_core.c \
../kernel/os_core/os_dbg_r.c \
../kernel/os_core/os_flag.c \
../kernel/os_core/os_mbox.c \
../kernel/os_core/os_mem.c \
../kernel/os_core/os_mutex.c \
../kernel/os_core/os_q.c \
../kernel/os_core/os_sem.c \
../kernel/os_core/os_task.c \
../kernel/os_core/os_time.c \
../kernel/os_core/ucos_ii.c 

OBJS += \
./kernel/os_core/os_core.o \
./kernel/os_core/os_dbg_r.o \
./kernel/os_core/os_flag.o \
./kernel/os_core/os_mbox.o \
./kernel/os_core/os_mem.o \
./kernel/os_core/os_mutex.o \
./kernel/os_core/os_q.o \
./kernel/os_core/os_sem.o \
./kernel/os_core/os_task.o \
./kernel/os_core/os_time.o \
./kernel/os_core/ucos_ii.o 

C_DEPS += \
./kernel/os_core/os_core.d \
./kernel/os_core/os_dbg_r.d \
./kernel/os_core/os_flag.d \
./kernel/os_core/os_mbox.d \
./kernel/os_core/os_mem.d \
./kernel/os_core/os_mutex.d \
./kernel/os_core/os_q.d \
./kernel/os_core/os_sem.d \
./kernel/os_core/os_task.d \
./kernel/os_core/os_time.d \
./kernel/os_core/ucos_ii.d 


# Each subdirectory must supply rules for building sources it contributes
kernel/os_core/%.o: ../kernel/os_core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


