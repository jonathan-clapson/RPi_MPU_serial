#include <stdio.h>

#include "rpi_mpu_io.h"
#include "support.h"
#include "error.h"

rpi_mpu_io::rpi_mpu_io ( char *device, int baud )
: connectionValid(true), calibration_dump(false), calibration_dump_file_name(std::string("calibration_dump.dat"))
{
	int ret = serial.Open( device, baud );
	
	if ( ret != 1 ) {
		std::cout << "Could not open serial device\n";
		connectionValid = false;
	}			
}

rpi_mpu_io::~rpi_mpu_io ( )
{
	serial.Close();
}

bool rpi_mpu_io::device_valid ( void ) {
	return connectionValid;
}

//write to serial port
int rpi_mpu_io::writeString (std::string str)
{
	return serial.WriteString(str.c_str());
}

int rpi_mpu_io::getReading(struct reading_memory_type *reading)
{
	const int timeout = 500;
	int ret = serial.ReadString(buffer, '\n', sizeof(buffer), timeout);
	
	//get rid of error cases
	if (ret <= 0)
		return ret;
	
	int num_faces = buffer[1];
	
	//FIXME: this should probably check for byte alignment too ie is divisable by 8
	
	/* 
	 * catch readings
	 * start of message = start of reading
	 * start of header = start of calibration values
	 * end of message = end of data
	 */
	if ( (buffer[0] == 0x01 || buffer[0] == 0x02) && buffer[ret-3] == 0x03)
	{
		char *readingStart = buffer+2;
		
		if (calibration_dump)
		{
			FILE *fp = fopen(calibration_dump_file_name.c_str(), "w");
			fprintf(fp, "%s\n", buffer);
			fclose(fp);
			std::cout << "calibration dump complete\n";
			calibration_dump = false;			
		}
	
		//number of iterations is number of faces * number of doubles in a sensor reading set
		int num_iters = num_faces*sizeof(struct reading_memory_type)/sizeof(double);
		for (int i=0; i<num_iters; i++) 
		{
			/* overwrites the first 16 bytes of the hex value with the ascii hex, last byte is 0 (from allocation)
			 * this allows c based string functions to print the ascii hex for debugging
			 */
			char message_iter_buffer[17] = {0};		
			memcpy(message_iter_buffer, readingStart+16*i, 16);
			print("buf%d: %s\n", i,message_iter_buffer);
			
			*((double*)reading + i) = hexCharsToDouble(message_iter_buffer);
		}		
		return ERR_GOTREADING;
	}	
	
	print("RPi_DBG: %s", buffer);
	return ERR_DBG;
}

void rpi_mpu_io::enable_calibration_dump( void )
{
	calibration_dump = true;	
}


void rpi_mpu_io::enable_calibration_dump(std::string filename)
{
	calibration_dump_file_name = filename;
	enable_calibration_dump();
	std::cout << "calibration enabled\n";
}
