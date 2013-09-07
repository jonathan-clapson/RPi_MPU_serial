
all:
	g++ -o sertool main.cpp serialib.cpp input.cpp rpi_mpu_io.cpp support.cpp -lpthread

clean:
	rm sertool *.o
