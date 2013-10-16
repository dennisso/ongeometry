# As of 2013 Sept 25, there are two versions of gcc on the machine. 
# Use the one on /usr/bin/gcc because gcc is broken.

SRC = main.c 
OBJ = $(SRC:.c=.o)
OUT = onland 

INCLUDES =-Iinclude

CFLAGS=-Wall
CC=gcc
LIBS=-Llib -lshp -static

.SUFFIXES: .c

all: dep $(OUT)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) $(OBJ) -o $(OUT) $(LIBS)

depend: dep

dep:
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC)

clean:
	rm -f $(OBJ) $(OUT)
