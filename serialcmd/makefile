CC = g++ -Wall -pedantic
INC_DIRS = ../libRPiSerial
LIB_DIRS = ../libRPiSerial

all: main.cpp support.cpp support.h error.h
	$(CC) -I$(INC_DIRS) -L$(LIB_DIRS) main.cpp support.cpp -o sertool -lRPiSerial

clean:
	rm sertool *.o
