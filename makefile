# As of 2013 Sept 25, there are two versions of gcc on the machine. 
# Use the one on /usr/bin/gcc because gcc is broken.
# As of 2013 Sept 27, the broken gcc has been removed
# CC has been changed back to gcc

TARGET=OnGeometry

SRC_DIR=src
OBJ_DIR=obj
OUT_DIR=bin

SRC_LIB = $(wildcard $(SRC_DIR)/lib/*.c) 
OBJ_LIB = $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_LIB:.c=.o)))
SRC = $(wildcard $(SRC_DIR)/*.c) 
OBJ = $(addprefix $(OBJ_DIR)/,$(notdir $(SRC:.c=.o)))
OUT = $(OUT_DIR)/$(TARGET)

INCLUDES =-Iinclude

CFLAGS=-Wall -std=c99 -g
CC=gcc
LIBS=-lgeos_c -Llib -lshp

all: clean $(OUT) test_all

$(OBJ_DIR)/%.o: $(SRC_DIR)/lib/%.c mkdir_all
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c mkdir_all
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(OUT): $(OBJ_LIB) $(OBJ_DIR)/main.o
	$(CC) -o $@ $^ $(LIBS)
   
test_all: $(OBJ_LIB) $(OBJ_DIR)/test.o
	$(CC) -o bin/test $^ $(LIBS)

mkdir_all:
	bash -c "mkdir -p {$(OBJ_DIR),$(OUT_DIR)}"

clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/*
