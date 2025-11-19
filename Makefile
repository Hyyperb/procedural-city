
CC = g++
CFLAGS = -Wall -O2
LDFLAGS = -lraylib -lm -ldl -lpthread -lGL -lX11
SRC = main.cpp
OUT = build/app

all:
	$(CC) $(SRC) -o $(OUT) $(CFLAGS) $(LDFLAGS)
