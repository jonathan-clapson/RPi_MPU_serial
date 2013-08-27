#include <exception>
#include <sstream>

#include <stdio.h>

#include "serialib.h"


#include <cmath>

#if defined (_WIN32) || defined( _WIN64)
#define DEVICE_PORT "COM1"                               // COM1 for windows
#endif

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyUSB0"                         // ttyS0 for linux
#endif

#define ERR_OK 0
#define ERR_DBG -10
#define ERR_GOTREADING -11

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


struct readingsType
{
	double_t accelX;
	double_t accelY;
	double_t accelZ;
	double_t temp;
	double_t gyroX;
	double_t gyroY;
	double_t gyroZ;	
};

//should make this class run in another probably multiple threads
class rpi_mpu_io
{
	serialib serial;
	bool connectionValid;
	char buffer[256];

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
    
    int getReading(readingsType &reading)
    {
		const int timeout = 5000;
		int ret = serial.ReadString(buffer, '\n', sizeof(buffer), timeout);
		
		//get rid of error cases
		if (ret <= 0)
			return ret;
		
		//catch debug messages (debug messages do not begin or end with start of message)
		if (buffer[0] != 0x02 && buffer[sizeof(double)*2*7+1] != 0x03) //start of message, end of message
		{
			std::cout << "RPi_DBG: " << buffer;
			return ERR_DBG;
		}
		
		char *readingStart = buffer+1;
		
		for (int i=0; i<ret/sizeof(double)/2; i++)
		{
			char b[17] = {0};		
			memcpy(b, readingStart+16*i, 16);					
			
			*((double*)&reading + i) = hexCharsToDouble(b);
		}
		
		return ERR_GOTREADING;
	}
};

int main()
{
    // Open serial port device
    rpi_mpu_io* rpi_mpu_dev = new rpi_mpu_io((char *)DEVICE_PORT, 115200);
    if (!rpi_mpu_dev->device_valid())
	{
		delete rpi_mpu_dev;
		return -1;
	}
    printf ("Serial port opened successfully!\n");
    
	while(1){
		// Read a string from the serial device
		//std::string reading;
		struct readingsType reading;
		int ret = rpi_mpu_dev->getReading(reading);
		
		if (ret == ERR_GOTREADING)
		{
			std::cout << "String read from serial port : aX " << reading.accelX << " aY " << reading.accelY << " aZ " << reading.accelZ << " temp " << \
				reading.temp << " gX " << reading.gyroX << " gY " << reading.gyroY << " gZ " << reading.gyroZ << "\n";
			
			printf("\n");			
		}	
		else if (ret != ERR_DBG)
			std::cout << "TimeOut reached. No data received!\n";
	}
    
    delete rpi_mpu_dev;

    return 0;
}
