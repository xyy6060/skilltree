appName=librtsp.a
#CROSS_COMPILE=arm-hisiv300-linux-
CFLAGS:=-Werror -fPIE

ifeq ($(CHIP),HI3516D)
CROSS_COMPILE=arm-hisiv300-linux-
endif

ifeq ($(CHIP),HI3516C)
CROSS_COMPILE=arm-hisiv500-linux-
endif
ifeq ($(CHIP),HI3516CV300)
CROSS_COMPILE=arm-hisiv500-linux-
endif
ifeq ($(CHIP),HI3516C_32M)
CROSS_COMPILE=arm-hisiv500-linux-
endif

ifeq ($(CHIP),HI3519)
CROSS_COMPILE=arm-hisiv500-linux-
endif

ifeq ($(CHIP),HI3516CV500)
CROSS_COMPILE=arm-himix200-linux-
endif
ifeq ($(CHIP),HI3516DV300)
CROSS_COMPILE=arm-himix200-linux-
endif
ifeq ($(CHIP),HI3519AV100)
CROSS_COMPILE=arm-himix200-linux-
endif

ifeq ($(CHIP),HI3516EV300)
CROSS_COMPILE=arm-himix310-linux-
endif
ifeq ($(CHIP),HI3516EV200)
CROSS_COMPILE=arm-himix100-linux-
endif
ifeq ($(CHIP),HI3518EV300)
CROSS_COMPILE=arm-himix100-linux-
endif

ifeq ($(CHIP),SC327DE_128M)
CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-
endif

ifeq ($(CHIP),CH256)
CROSS_COMPILE=arm-linux-gnueabihf-
endif
ifeq ($(CHIP),CH226)
CROSS_COMPILE=arm-linux-gnueabihf-
endif

ifeq ($(CHIP),SC329Q_128M)
CROSS_COMPILE=arm-linux-gnueabihf-
endif
ifeq ($(CHIP),SC339G_128M)
CROSS_COMPILE=arm-linux-gnueabihf-9.1.0-
endif
ifeq ($(CHIP),SC338G_128M)
CROSS_COMPILE=arm-linux-gnueabihf-9.1.0-
endif
ifeq ($(CHIP),SC338Q_128M)
CROSS_COMPILE=arm-linux-gnueabihf-9.1.0-
endif

ifeq ($(CHIP),SC335X_128M)
CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf-uclibc1.0.31-
endif

ifeq ($(CHIP),NT98528)
CROSS_COMPILE=arm-ca9-linux-gnueabihf-
endif


#CFLAGS
ifeq ($(CROSS_COMPILE),HI3516CV300)
CFLAGS += -mcpu=arm926ej-s  -mno-unaligned-access -fno-aggressive-loop-optimizations -DARCH_ARM_V5
else ifeq ($(CROSS_COMPILE),HI3516C_32M)
CFLAGS += -mcpu=arm926ej-s  -mno-unaligned-access -fno-aggressive-loop-optimizations -DARCH_ARM_V5
else ifeq ($(CROSS_COMPILE),HI3519AV100)
CFLAGS += -mcpu=cortex-a53 -mfloat-abi=softfp -mfpu=neon-vfpv4 -mno-unaligned-access -fno-aggressive-loop-optimizations -DARCH_ARM_V7
else
CFLAGS += -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -mno-unaligned-access -fno-aggressive-loop-optimizations -DARCH_ARM_V7
endif


#mgcc=g++
mgcc=$(CROSS_COMPILE)g++
o_fg=-Wall -ggdb -c
l_fg=-Wall -ggdb
obj_path=./object/

AR:=ar

src=$(wildcard ./base64.c ./rtsp_push.c) #not all *.c
obj=$(patsubst ./%.c, $(obj_path)%.o, $(src))#all *.o
#common_path=/home/liwenlong/work/nvr5.0-a01-v8.1.0/device/corelib
#i_boost=$(common_path)/boost
#i_rapidxml=$(common_path)/rapidxml
#i_openssl=$(common_path)/openssl
#i_honlylib=$(common_path)/headeronly_lib
#i_curl=$(common_path)/curl

#l_openssl_lib=$(common_path)/openssl/libs/lib_x86/libssl.a
#l_openssl_crypto=$(common_path)/openssl/libs/lib_x86/libcrypto.a
#l_curl=$(common_path)/curl/libs/lib_x86/libcurl.a

#iflag=-I./ -I./include -I$(i_honlylib) -I$(i_curl) -I$(i_boost) -I$(i_rapidxml) -I$(i_openssl)
iflag = -I./include
#l_rtsp=./rtsp_demo.a 
#iflag+=-DX86_TEST -DTYOOS_MD5
#iflag+=-DX86_TEST

.PHONY: clean

$(appName): $(obj)
	$(mgcc) $(CFLAGS) $(obj)  $(l_fg) -o -W $(appName) -ldl -lpthread

$(obj_path)base64.o: base64.c
	$(mgcc) $(iflag) $(o_fg) -fPIC -w -o $(obj_path)base64.o base64.c
$(obj_path)rtsp_push.o: rtsp_push.c
	$(mgcc) $(iflag) $(o_fg) -fPIC -w -o $(obj_path)rtsp_push.o rtsp_push.c  

$(appName): $(obj)
	$(AR) -rsv $(appName)  $(obj) 

clean:
	rm -rf $(obj_path)*.o
	rm -rf $(appName)
