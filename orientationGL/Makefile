CC = g++ -Wall -pedantic
INC_DIRS = ../libRPiSerial
LIB_DIRS = ../libRPiSerial

all: main main_test
	
main: main.cpp
	$(CC) -I$(INC_DIRS) -L$(LIB_DIRS) main.cpp graphics.cpp -o main -lGL -lGLU `sdl-config --cflags --libs` -lRPiSerial

main_test: main_test.cpp
	$(CC) -I$(INC_DIRS) -L$(LIB_DIRS) main_test.cpp graphics.cpp -o main_test -lGL -lGLU `sdl-config --cflags --libs` -lRPiSerial

clean:
	@echo Cleaning up...
	@rm main main_test
	@echo Done.
