CC = g++

all: rpiserial

support.o: support.cpp support.h
	$(CC) -fpic -c support.cpp

serialib.o: serialib.cpp serialib.h
	$(CC) -fpic -c serialib.cpp

rpi_mpu_io.o: rpi_mpu_io.cpp rpi_mpu_io.h
	$(CC) -fpic -c rpi_mpu_io.cpp

rpiserial: serialib.o rpi_mpu_io.o  support.o error.h
	$(CC) -shared -o libRPiSerial.so rpi_mpu_io.o serialib.o support.o
		
clean:
	rm rpi_mpu_io.o serialib.o support.o libRPiSerial.so
