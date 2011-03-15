PROJECT = testsvr
SOURCES = testsvr.c
DEPENDS = 
INC_DIR = ../include
LIB_DIR = ../lib
PRJ_DIR = .
OBJ_DIR = ../build
CFLAGS  = -Wall -I$(INC_DIR)
CC      = gcc

_OBJECT = $(patsubst %,$(OBJ_DIR)/%,$(SOURCES:.c=.o))
_OUTPUT = $(PRJ_DIR)/$(PROJECT)
vpath %.h $(INC_DIR)
vpath %.a $(LIB_DIR)
vpath %.so $(LIB_DIR)

.PHONY: all
all: $(_OUTPUT)

$(OBJ_DIR)/%.o: %.c $(DEPENDS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(_OUTPUT): $(_OBJECT) -lhttpsvr
	$(CC) -o $(_OUTPUT) $(_OBJECT) -lhttpsvr -L$(LIB_DIR)

.PHONY: clean
clean:
	-rm -f $(_OUTPUT) $(_OBJECT)
