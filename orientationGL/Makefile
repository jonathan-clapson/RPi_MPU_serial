CC = g++ -Wall -pedantic
INC_DIRS = ../libRPiSerial
LIB_DIRS = ../libRPiSerial

SCRATCH_PFX=/mnt/scratch/localInstalls

all: main main_test
	
main: main.cpp
	$(CC) -I$(INC_DIRS) -I$(SCRATCH_PFX)/usr/include/SDL -L$(LIB_DIRS) -L$(SCRATCH_PFX)/usr/lib/x86_64-linux-gnu main.cpp graphics.cpp -o main -lGL -lGLU `sdl-config --cflags --libs` -lRPiSerial

main_test: main_test.cpp
	$(CC) -I$(INC_DIRS) -I$(SCRATCH_PFX)/usr/include/SDL -L$(LIB_DIRS) -L$(SCRATCH_PFX)/usr/lib/x86_64-linux-gnu main_test.cpp graphics.cpp -o main_test -lGL -lGLU `sdl-config --cflags --libs` -lRPiSerial

clean:
	@echo Cleaning up...
	@rm main main_test
	@echo Done.
