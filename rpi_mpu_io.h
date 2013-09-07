#ifndef __RPI_MPU_IO_H__
#define __RPI_MPU_IO_H__

#include "serialib.h"

struct reading_memory_type {
	double a_x, a_y, a_z;
	double temp;
	double w_x, w_y, w_z;
	
	double x,y,z;
	double v_x, v_y, v_z;
	
	double o_x, o_y, o_z;	
};

class rpi_mpu_io {
	serialib serial;
	bool connectionValid;
	char buffer[2000];
	bool calibration_dump;
	std::string calibration_dump_file_name;

public:	
	rpi_mpu_io ( char *device, int baud );
	
	~rpi_mpu_io ( );
	
	bool device_valid ( void );
	
	//write to serial port
	int writeString (std::string str);
    
    int getReading(struct reading_memory_type *reading);
	
	void enable_calibration_dump( void );
	void enable_calibration_dump(std::string filename);
};

#endif /* __RPI_MPU_IO_H__ */
