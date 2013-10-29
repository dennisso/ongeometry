# As of 2013 Sept 25, there are two versions of gcc on the machine. 
# Use the one on /usr/bin/gcc because gcc is broken.
# As of 2013 Sept 27, the broken gcc has been removed
# CC has been changed back to gcc

DEP_SRC = geospatial.c rasterize.c
SRC = main.c $(DEP_SRC)
SRC_TEST = test.c $(DEP_SRC)
OBJ = $(SRC:.c=.o)
OBJ_TEST = $(SRC_TEST:.c=.o)
OUT = onland
OUT_TEST = test

INCLUDES =-Iinclude

CFLAGS=-Wall -std=c99 -g
CC=gcc
LIBS=-lgeos_c -Llib -lshp

.SUFFIXES: .c

all: clean dep $(OUT) $(OUT_TEST)

test_all: dep_test $(OUT_TEST)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(LIBS)

$(OUT_TEST): $(OBJ_TEST)
	$(CC) $(OBJ_TEST) -o $(OUT_TEST) $(LIBS)

depend: dep

dep:
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC)

dep_test:
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC_TEST)

clean:
	rm -f $(OBJ) $(OUT) $(OBJ_TEST) $(OUT_TEST)


