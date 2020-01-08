SDK_ROOT = ..\..\..
PATCH_PATH = $(SDK_ROOT)\patch
UTIL_PATH = $(SDK_ROOT)\tools\buildTool

GCC_ARM_PATH :=  $(SDK_ROOT)\tools\gcc-arm-none-eabi-5_4-2016q3-20160926-win32
THUMB_PATH := $(GCC_ARM_PATH)\arm-none-eabi\lib\thumb
CROSS_COMPILE := $(GCC_ARM_PATH)\bin\arm-none-eabi-
INC = .\$(GCC_ARM_PATH)\arm-none-eabi\include

INCLUDES =	-I${INC} \
			-I$(SDK_ROOT)/cmsis/ \
			-I$(SDK_ROOT)/device/inc \
			-I$(SDK_ROOT)/drivers/hal/inc \
			-I$(SDK_ROOT)/drivers/ble/inc \
			-I$(SDK_ROOT)/patch/ \
			-I$(GCC_ARM_PATH)/arm-none-eabi/include \
			-I../inc \

SRC_FILES = ../src/main.c \
			$(SDK_ROOT)/drivers/hal/bc62xx_ble.c \
			$(SDK_ROOT)/drivers/ble/ble_adv.c \
			$(SDK_ROOT)/drivers/hal/bc62xx_uart.c \
            $(SDK_ROOT)/drivers/hal/bc62xx_gpio.c \
            $(SDK_ROOT)/device/system.c \
           
OBJ = ${patsubst %.c, %.o, ${SRC_FILES}} 

# Debug build flags
ifeq ($(release),1)
	  MACRO = -DMESH_RELEASE -DRELEASE
else
      MACRO := 
endif

#srcFile = $(SRC_FILES)
#srcFile += $(MSRC_FILES)

#include $(srcFile:.c=.d)

#%.d: %.c
#	$(CROSS_COMPILE)gcc -MM -c $(INCLUDES) $(CPPFLAGS) $< > $@.$$$$; \
#	sed 's,\($*\)\.o[:]*,\1.o$@:,g' < $@.$$$$ > $@;

LDSCRIPTS := -T $(SDK_ROOT)/gcc.ld 

LIBFILES := -lc -lgcc
LIBDIR   := -L . 
LIBDIR   += -L "$(GCC_ARM_PATH)\lib\gcc\arm-none-eabi\5.4.1\armv6-m"
LIBDIR   += -L "$(GCC_ARM_PATH)\arm-none-eabi\lib\armv6-m"

all : output\a.out
	$(CROSS_COMPILE)objdump -S $^ > output\disasm
	$(CROSS_COMPILE)objcopy $^ -O ihex output\out.hex
	python $(UTIL_PATH)\hex2rom.py 0 40000 < output\out.hex > output\out.rom
	copy $(PATCH_PATH)\ramcode.rom .\output\ramcode.rom
	copy $(PATCH_PATH)\patch.dat + $(PATCH_PATH)\user.dat + $(PATCH_PATH)\rf.dat .\output\sched.rom
	copy $(PATCH_PATH)\memmap.format .\output\memmap.format
	
OBJS = $(SDK_ROOT)/startup_m0.o

output\a.out : $(OBJ) $(OBJS)
	$(CROSS_COMPILE)ld $^ -o $@ $(LIBDIR) $(LIBFILES) $(LDSCRIPTS) -Map=output\memmap

$(OBJS) : $(SDK_ROOT)/startup_m0.s
	$(CROSS_COMPILE)as $^ -o $@ -mthumb -mcpu=cortex-m0 

$(OBJ) : %.o : %.c
	$(CROSS_COMPILE)gcc $^ -c $(INCLUDES) -g $(MACRO) -Os -o $@ -mthumb -mcpu=cortex-m0 -msoft-float

clean :
	@del /Q output\*.*
	@del /Q /S $(SDK_ROOT)\*.o
	@del /Q /S $(SDK_ROOT)\*.d.*

