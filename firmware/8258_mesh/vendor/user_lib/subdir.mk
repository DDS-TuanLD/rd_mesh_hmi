################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vendor/user_lib/app_lcd_handler.c \
../vendor/user_lib/app_lcd_interface.c \
../vendor/user_lib/app_lcd_model.c \
../vendor/user_lib/app_lcd_serial.c \
../vendor/user_lib/user_fifo.c \
../vendor/user_lib/user_flash.c \
../vendor/user_lib/user_model.c \
../vendor/user_lib/user_serial.c 

OBJS += \
./vendor/user_lib/app_lcd_handler.o \
./vendor/user_lib/app_lcd_interface.o \
./vendor/user_lib/app_lcd_model.o \
./vendor/user_lib/app_lcd_serial.o \
./vendor/user_lib/user_fifo.o \
./vendor/user_lib/user_flash.o \
./vendor/user_lib/user_model.o \
./vendor/user_lib/user_serial.o 


# Each subdirectory must supply rules for building sources it contributes
vendor/user_lib/%.o: ../vendor/user_lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TC32 Compiler'
	tc32-elf-gcc -ffunction-sections -fdata-sections -I"C:\Users\admin\OneDrive\M�y t�nh\rd_mesh_hmi\firmware" -I"C:\Users\admin\OneDrive\M�y t�nh\rd_mesh_hmi\firmware\vendor\common\mi_api\libs" -I"C:\Users\admin\OneDrive\M�y t�nh\rd_mesh_hmi\firmware\vendor\common\mi_api\mijia_ble_api" -D__PROJECT_MESH__=1 -D__PROJECT_NODE_LCD__=1 -D__telink__ -DCHIP_TYPE=CHIP_TYPE_8258 -Wall -O2 -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


