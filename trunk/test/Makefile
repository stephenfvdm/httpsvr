PROJECT = testsvr
SOURCES = testsvr.c
DEPENDS = 
INC_DIR = ../include
PRJ_DIR = .
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
	$(CC) -o $(_OUTPUT) $(_OBJECT) -lhttpsvr -L../lib

clean:
	-rm -f $(_OUTPUT) $(_OBJECT)
