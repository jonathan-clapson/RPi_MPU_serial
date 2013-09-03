#include <exception>
#include <sstream>
#include <iomanip>

#include <stdio.h>
#include <cmath>

#include "serialib.h"

#include "input.h"


#if defined (_WIN32) || defined( _WIN64)
#define DEVICE_PORT "COM1"                               // COM1 for windows
#endif

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyUSB0"                         // ttyS0 for linux
#endif

#define ERR_OK 0
#define ERR_DBG -10
#define ERR_GOTREADING -11

struct reading_memory_type {
	double a_x, a_y, a_z;
	double temp;
	double w_x, w_y, w_z;
	
	double x,y,z;
	double v_x, v_y, v_z;
	
	double o_x, o_y, o_z;	
};

char hexToChar (char buf)
{
	char ret = 0;
	if (buf >= '0' && buf <= '9')
	{
		ret = buf - '0';
	}
	else if (buf >= 'A' && buf <= 'F')
	{
		ret = buf - 'A' + 10;
	}
	
	return ret;	
}

char hexCharsToChar(char *buf)
{
	char ret = 0; 
	
	char MSBS;
	char LSBS;
	MSBS = hexToChar(buf[0]);
	LSBS = hexToChar(buf[1]);
	
	MSBS = MSBS << 4;
	
	ret = MSBS|LSBS;
}

double hexCharsToDouble(char *buf)
{
	double res = 0;
	char *retBuf = (char *) &res;
	
	for (int i=0; i<sizeof(double); i++)
	{
		*(retBuf + i)  = hexCharsToChar(buf + i*2);
	}
	
	return res;	
}



//should make this class run in another probably multiple threads
class rpi_mpu_io
{
	serialib serial;
	bool connectionValid;
	char buffer[2000];

public:	
	rpi_mpu_io ( char *device, int baud )
	: connectionValid(true)
	{
		int ret = serial.Open( device, baud );
		
		if ( ret != 1 ) {
			std::cout << "Could not open serial device\n";
			connectionValid = false;
		}			
	}
	
	~rpi_mpu_io ( )
	{
		serial.Close();
	}
	
	bool device_valid ( void ) {
		return connectionValid;
	}
	
	//write to serial port
	int writeString (std::string str)
	{
		return serial.WriteString(str.c_str());
    }
    
    int getReading(struct reading_memory_type *reading)
    {
		const int timeout = 500;
		int ret = serial.ReadString(buffer, '\n', sizeof(buffer), timeout);
		
		//get rid of error cases
		if (ret <= 0)
			return ret;
		
		int num_faces = buffer[1];
		
		//catch debug messages (debug messages do not begin or end with start of message)
		//FIXME: this should probably check for byte alignment too ie is divisable by 8
		if (buffer[0] != 0x02 || buffer[ret-3] != 0x03) //start of message, end of message
		{
			std::cout << "RPi_DBG: " << buffer;
			return ERR_DBG;
		}
		
		char *readingStart = buffer+2;
		
		
		//number of iterations is number of faces * number of doubles in a sensor reading set
		int num_iters = num_faces*sizeof(reading_memory_type)/sizeof(double);
		for (int i=0; i<num_iters; i++)
		{
			/* overwrites the first 16 bytes of the hex value with the ascii hex, last byte is 0 (from allocation)
			 * this allows c based string functions to print the ascii hex for debugging
			 */
			char message_iter_buffer[17] = {0};		
			memcpy(message_iter_buffer, readingStart+16*i, 16);
			printf("buf%d: %s\n", i,message_iter_buffer);
			
			//printf("message_iter_buffer: %s\n", message_iter_buffer);
			
			*((double*)reading + i) = hexCharsToDouble(message_iter_buffer);
		}
		
		return ERR_GOTREADING;
	}
};

int main()
{
	pthread_t read_thread;
	struct read_message_type r_msg;
	spawn_input_thread(&read_thread, &r_msg);
	
    // Open serial port device
    rpi_mpu_io* rpi_mpu_dev = new rpi_mpu_io((char *)DEVICE_PORT, 115200);
    if (!rpi_mpu_dev->device_valid())
	{
		delete rpi_mpu_dev;
		return -1;
	}
    printf ("Serial port opened successfully!\n");
    
	while(1){		
		//FIXME: hard code for 6 readings 
		struct reading_memory_type readings[6];
		
		int ret = rpi_mpu_dev->getReading(readings);
		
		if (r_msg.message_valid)
		{
			rpi_mpu_dev->writeString(r_msg.message);
			r_msg.message_valid = 0;			
		}
		
		if (ret == ERR_GOTREADING)
		{
			for (int i=0; i<6; i++)
			{
				//std::cout << "RPi_Dev" << i << ": x " << std::setprecision(2) << std::setw(8) << readings[i].x << " y " << readings[i].y << " z " << readings[i].z << 
				std::cout << "RPi_Dev" << i << ": x " << readings[i].x << " y " << readings[i].y << " z " << readings[i].z << \
				" vx " << readings[i].v_x << " vy " << readings[i].v_y << " vz " << readings[i].v_z << \
				" ax " << readings[i].a_x << " ay " << readings[i].a_y << " az " << readings[i].a_z << \
				" temp " << readings[i].temp << \
				" ox " << readings[i].o_x << " oy " << readings[i].o_y << " oz " << readings[i].o_z << \
				" wx " << readings[i].w_x << " wy " << readings[i].w_y << " wz " << readings[i].w_z << std::endl;
			}			
		}	
		/*else if (ret != ERR_DBG)
			std::cout << "TimeOut reached. No data received!\n";*/
	}
	
	end_input_thread(&read_thread, &r_msg);
    
    delete rpi_mpu_dev;

    return 0;
}
