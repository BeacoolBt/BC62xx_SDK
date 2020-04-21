SDK_ROOT = ..\..\..
PATCH_PATH = $(SDK_ROOT)\patch
UTIL_PATH = $(SDK_ROOT)\tools\buildTool
MESHPATH = $(SDK_ROOT)\mesh

GCC_ARM_PATH :=  $(SDK_ROOT)\tools\gcc-arm-none-eabi-5_4-2016q3-20160926-win32
THUMB_PATH := $(GCC_ARM_PATH)\arm-none-eabi\lib\thumb
CROSS_COMPILE := $(GCC_ARM_PATH)\bin\arm-none-eabi-
INC = .\$(GCC_ARM_PATH)\arm-none-eabi\include

SDK_PATH = $(SDK_ROOT)\SDK

WARN_LEV = -Wall

INCLUDES =	-I${INC} \
			-I$(SDK_ROOT)\cmsis \
			-I$(SDK_ROOT)\device\inc \
			-I$(SDK_ROOT)\drivers\hal\inc \
			-I$(SDK_ROOT)\drivers\led\inc \
			-I$(SDK_ROOT)\drivers\ProtocolResolution\inc \
			-I$(SDK_ROOT)\patch \
			-I$(GCC_ARM_PATH)\arm-none-eabi\include \
			-I..\inc \
			
INCLUDES += -I$(MESHPATH)\api \
			-I$(MESHPATH)\app \
			-I$(MESHPATH)\app\ali_light \
			-I$(MESHPATH)\app\ali_socket \
			-I$(MESHPATH)\app\ali_HeatingTable \
			-I$(MESHPATH)\common\api \
			-I$(MESHPATH)\config \
			-I$(MESHPATH)\drivers \
			-I$(MESHPATH)\include \
			-I$(MESHPATH)\m_al \
			-I$(MESHPATH)\models\inc \
			-I$(MESHPATH)\port \
			-I$(MESHPATH)\src \
			-I$(MESHPATH)\src\m_bearer \
			-I$(MESHPATH)\src\m_fnd \
			-I$(MESHPATH)\src\m_lay \
			-I$(MESHPATH)\src\m_prov \
			-I$(MESHPATH)\src\m_tb \

SRC_FILES = ..\src\main.c \
			..\src\gatt_ota.c \
			$(SDK_ROOT)\drivers\hal\bc62xx_ble.c \
			$(SDK_ROOT)\drivers\hal\bc62xx_flash.c \
			$(SDK_ROOT)\drivers\hal\bc62xx_uart.c \
			$(SDK_ROOT)\drivers\hal\bc_flash.c \
            $(SDK_ROOT)\drivers\hal\bc62xx_pwm.c	\
            $(SDK_ROOT)\drivers\hal\bc62xx_timer.c \
            $(SDK_ROOT)\drivers\hal\bc62xx_gpio.c \
            $(SDK_ROOT)\drivers\hal\bc62xx_wdt.c \
            $(SDK_ROOT)\drivers\led\led.c \
            $(SDK_ROOT)\drivers\ProtocolResolution\ProtocolRln.c \
            $(SDK_ROOT)\device\systick.c \
            $(SDK_ROOT)\device\system.c \
			$(SDK_ROOT)\device\bc_sys.c \
			$(MESHPATH)\app\app_mesh.c \
			$(MESHPATH)\app\ali_light\ali_light.c \
			$(MESHPATH)\app\ali_socket\ali_socket.c \
			$(MESHPATH)\app\ali_HeatingTable\ali_HeatingTable.c \
			$(MESHPATH)\app\ali_config.c \
			
			
MSRC_FILES = 	$(MESHPATH)\common\src\co_list.c \
				$(MESHPATH)\common\src\co_utils.c \
				$(MESHPATH)\common\src\co_log.c \
				$(MESHPATH)\m_al\m_al_activity.c \
				$(MESHPATH)\m_al\m_al_adv.c \
				$(MESHPATH)\m_al\m_al_con.c \
				$(MESHPATH)\m_al\m_al_djob.c \
				$(MESHPATH)\m_al\m_al_scan.c \
				$(MESHPATH)\m_al\m_al_sec.c \
				$(MESHPATH)\m_al\m_al_task.c \
				$(MESHPATH)\m_al\m_al_timer.c \
				$(MESHPATH)\models\src\mm_com.c \
				$(MESHPATH)\models\src\ali\vendors\mm_alis.c \
				$(MESHPATH)\models\src\ali\mm_ali.c \
				$(MESHPATH)\models\src\ali\ali_timer.c \
				$(MESHPATH)\models\src\ali\unixTime.c \
				$(MESHPATH)\models\src\gen\gens\bc_mm_gens_oo.c \
				$(MESHPATH)\models\src\light\lights\bc_mm_lights_ln.c \
				$(MESHPATH)\models\src\light\lights\bc_mm_lights_ctl.c \
				$(MESHPATH)\port\meshTimer.c \
				$(MESHPATH)\port\ecc.c \
				$(MESHPATH)\port\port.c \
				$(MESHPATH)\port\portBle.c \
				$(MESHPATH)\port\portSec.c \
				$(MESHPATH)\port\heap_4.c \
				$(MESHPATH)\src\m_api.c \
				$(MESHPATH)\src\m_api_msg.c \
				$(MESHPATH)\src\m_bcn\m_bcn.c \
				$(MESHPATH)\src\m_bearer\m_bearer.c \
				$(MESHPATH)\src\m_bearer\m_bearer_adv.c \
				$(MESHPATH)\src\m_bearer\m_bearer_gatt.c \
				$(MESHPATH)\src\m_fnd\m_fnd.c \
				$(MESHPATH)\src\m_fnd\m_fnd_confs.c \
				$(MESHPATH)\src\m_fnd\m_fnd_hlths.c \
				$(MESHPATH)\src\m_lay\m_lay.c \
				$(MESHPATH)\src\m_lay\m_lay_access.c \
				$(MESHPATH)\src\m_lay\m_lay_friend.c \
				$(MESHPATH)\src\m_lay\m_lay_hb.c \
				$(MESHPATH)\src\m_lay\m_lay_lpn.c \
				$(MESHPATH)\src\m_lay\m_lay_ltrans.c \
				$(MESHPATH)\src\m_lay\m_lay_net.c \
				$(MESHPATH)\src\m_lay\m_lay_proxy.c \
				$(MESHPATH)\src\m_lay\m_lay_utrans.c \
				$(MESHPATH)\src\m_prov\m_prov.c \
				$(MESHPATH)\src\m_prov\m_prov_adv_trans.c \
				$(MESHPATH)\src\m_prov\m_prov_bearer.c \
				$(MESHPATH)\src\m_tb\m_tb.c \
				$(MESHPATH)\src\m_tb\m_tb_buf.c \
				$(MESHPATH)\src\m_tb\m_tb_friend.c \
				$(MESHPATH)\src\m_tb\m_tb_key.c \
				$(MESHPATH)\src\m_tb\m_tb_mio.c \
				$(MESHPATH)\src\m_tb\m_tb_sec.c \
				$(MESHPATH)\src\m_tb\m_tb_state.c \
				$(MESHPATH)\src\m_tb\m_tb_store_wvt.c \
				$(MESHPATH)\src\m_tb\m_tb_timer.c 
           
OBJ = ${patsubst %.c, %.o, ${SRC_FILES}} 
MOBJ = ${patsubst %.c, %.o, ${MSRC_FILES}} 

# Debug build flags
ifeq ($(release),1)
	  MACRO += -DMESH_RELEASE -DRELEASE
else
      MACRO += 
endif


LDSCRIPTS := -T $(SDK_ROOT)\gcc.ld 
LIBFILES := -lc -lgcc
LIBDIR   := -L . \

ifeq ($(user),1)
LIBFILES += -lmesh
LIBDIR += -L $(SDK_ROOT)\mesh\lib
endif

LIBDIR   += -L "$(GCC_ARM_PATH)\lib\gcc\arm-none-eabi\5.4.1\armv6-m"
LIBDIR   += -L "$(GCC_ARM_PATH)\arm-none-eabi\lib\armv6-m"

#$(warning   OBJIEAT is $(OBJ)) 

all : output\a.out
	$(CROSS_COMPILE)objdump -S $^ > output\disasm
	$(CROSS_COMPILE)objcopy $^ -O ihex output\out.hex
	python $(UTIL_PATH)\hex2rom.py 0 40000 < output\out.hex > output\out.rom
	copy $(PATCH_PATH)\ramcode.rom .\output\ramcode.rom
	copy $(PATCH_PATH)\patch.dat + $(PATCH_PATH)\user.dat + $(PATCH_PATH)\rf.dat .\output\sched.rom
	copy $(PATCH_PATH)\memmap.format .\output\memmap.format
	
OBJS = $(SDK_ROOT)\startup_m0.o

ifeq ($(user),1)
output\a.out : $(OBJ) $(OBJS) $(SDK_ROOT)\mesh\lib\libmesh.a
	@$(CROSS_COMPILE)ld $^ -o $@ $(LIBDIR) $(LIBFILES) $(LDSCRIPTS) -Map=output\memmap
else
$(SDK_ROOT)\mesh\lib\libmesh.a:$(MOBJ)
	$(CROSS_COMPILE)ar crv $@ $^
	
output\a.out : $(OBJ) $(OBJS) $(MOBJ) $(SDK_ROOT)\mesh\lib\libmesh.a
	@$(CROSS_COMPILE)ld $^ -o $@ $(LIBDIR) $(LIBFILES) $(LDSCRIPTS) -Map=output\memmap
endif


$(OBJS) : $(SDK_ROOT)\startup_m0.s
	@$(CROSS_COMPILE)as $^ -o $@ -mthumb -mcpu=cortex-m0 

$(OBJ) : %.o : %.c
	@$(CROSS_COMPILE)gcc $^ -c $(INCLUDES) -g $(MACRO)  $(WARN_LEV) -Os -o $@ -mthumb -mcpu=cortex-m0 -msoft-float

$(MOBJ) : %.o : %.c
	@$(CROSS_COMPILE)gcc $^ -c $(INCLUDES) -g $(MACRO)  $(WARN_LEV) -Os -o $@ -mthumb -mcpu=cortex-m0 -msoft-float

##
%.d: %.c
	@$(CROSS_COMPILE)gcc -MM $< -c $(INCLUDES) -g $(MACRO)  $(WARN_LEV)  > $@.$$$$
	@sed "s,$(notdir $*)\.o[ :]*,$(notdir $*)\.o $(notdir $*)\.d : ,g" < $@.$$$$ > $@
	@del /Q /S $@.$$$$ > nul
	@python3 $(SDK_ROOT)\tools\Script\dodepend.py < $@ >> $(SDK_ROOT)\depend.list.tmp

sinclude ${patsubst %.c, %.d, ${SRC_FILES}} 
sinclude ${patsubst %.c, %.d, ${MSRC_FILES}} 
##

filelist: $(SRC_FILES:.c=.d) $(MSRC_FILES:.c=.d)
	@python3 $(SDK_ROOT)\tools\Script\doSort.py < $(SDK_ROOT)\depend.list.tmp > $(SDK_ROOT)\depend.list 

clean :
	@del /Q output\*.*
	@del /Q /S $(SDK_ROOT)\*.o
	@del /Q /S $(SDK_ROOT)\*.d.*
	@del /Q /S $(SDK_ROOT)\depend.list.tmp
	@del /Q /S $(SDK_ROOT)\depend.list 
	

