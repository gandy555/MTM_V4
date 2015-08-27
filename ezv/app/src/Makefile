#
# Makefile
#

TARGET	= mts_main

SOURCES	= \
	  controller_serial.c \
	  controller_gpio.c \
	  controller_watchdog.c \
	  controller_microwave.c \
	  comm_hdt.c \
	  comm_shn.c \
	  comm_kcm.c \
	  comm_ezv.c \
	  view_light.c \
	  view_elevator.c \
	  view_gas.c \
	  view_parking.c \
	  view_security.c \
	  view_setup.c \
	  view_weather.c \
	  message.c \
	  object.c \
	  nano-X_custom.c \
	  setup_data.c \
	  duo_utils.c \
	  duo_workqueue.c \
	  parking_info.c \
	  main.c \

PREFIX	= arm-linux
AR	= $(PREFIX)-ar
CC	= $(PREFIX)-gcc
CP	= $(PREFIX)-g++
LD	= $(PREFIX)-ld

WORK_DIR = /home/freejack/MTS_Work/mtm_v3
NANOX_INC_PATH	= $(WORK_DIR)/nanoX/include
NANOX_LIB_PATH	= $(WORK_DIR)/nanoX/libs

DEFINE	 = -D_ARM_
INCLUDE  += -I../include -I$(NANOX_INC_PATH)
LIBS	 = -L$(NANOX_LIB_PATH) -lnano-X -lfreetype -ljpeg -lpng12 -lz -lm -lpthread -lrt 

OBJ_DIR  = ../obj
BIN_DIR  = ../bin
OBJS	= $(SOURCES:.c=.o)

.SUFFIXES : .c .o

.c.o :
	$(CC) -O2 -c $(DEFINE) $(INCLUDE) $(CFLAGS) -o $(OBJ_DIR)/$@ $<

all : $(TARGET)

$(TARGET): $(OBJS)
	$(CP) $(addprefix $(OBJ_DIR)/, $(OBJS)) $(LIBS) -o $(BIN_DIR)/$(TARGET) 

clean :
	rm -rf $(addprefix $(OBJ_DIR)/, $(OBJS)) $(BIN_DIR)/$(TARGET)
