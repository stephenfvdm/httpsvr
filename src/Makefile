PROJECT = libhttpsvr.a
SOURCES = httpsvr.c httpsvr_file.c
DEPENDS = httpsvr.h httpsvr_file.h
INC_DIR = ../include
PRJ_DIR = ../lib
OBJ_DIR = ../build
CFLAGS  = -Wall -I$(INC_DIR)
CC      = gcc

_OBJECT = $(patsubst %,$(OBJ_DIR)/%,$(SOURCES:.c=.o))
_DEPEND = $(patsubst %,$(INC_DIR)/%,$(DEPENDS))
_OUTPUT = $(PRJ_DIR)/$(PROJECT)

all: $(_OUTPUT)

$(OBJ_DIR)/%.o: %.c $(_DEPEND)
	$(CC) -c -o $@ $< $(CFLAGS)

$(_OUTPUT): $(_OBJECT)
	libtool -o $(_OUTPUT) -static $(_OBJECT)

clean:
	-rm -f $(_OUTPUT) $(_OBJECT)
