all : flash

TARGET:=systick_protothreads

CFLAGS+=-DSTDOUT_UART -Wno-unused-but-set-variable

include ../../ch32v003fun/ch32v003fun.mk

flash : cv_flash
clean : cv_clean

