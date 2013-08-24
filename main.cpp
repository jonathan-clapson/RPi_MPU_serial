#include <stdio.h>
#include "serialib.h"
#include <exception>

#include <cmath>

#if defined (_WIN32) || defined( _WIN64)
#define DEVICE_PORT "COM1"                               // COM1 for windows
#endif

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyUSB0"                         // ttyS0 for linux
#endif

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
    
    int getReading(std::string &reading)
    {
		const int timeout = 5000;
		int ret = serial.ReadString(buffer, '\n', sizeof(buffer), timeout);
		reading = std::string(buffer);
		return ret;
	}
	int getReading(struct readingsType& readings)
	{
		const int timeout = 5000;
		int ret = serial.ReadString(buffer, '\n', sizeof(buffer), timeout);		
		readings = *((struct readingsType*) buffer);
		
		return ret;		
	}
	int getReading(char *buf)
	{
		const int timeout = 5000;
		int ret = serial.Read(buffer, sizeof(buffer), timeout);
		strcpy(buf, buffer);
		return ret;
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
		std::string reading;
		//struct readingsType reading;
		int ret = rpi_mpu_dev->getReading(reading);
		
		if (ret>0)
		{
			/*std::cout << "read: " << reading.length();
			//std::cout << " String read from serial port : aX " << reading.accelX << " aY " << reading.accelY << " aZ " << reading.accelZ << "\n";
			std::cout << " String read from serial port : " << reading << "\n";
			
			std::cout << "dbg: ";
			char buf[500];
			char chunk[500];
			buf[0] = '\0';
		
			sprintf(buf, "%2X\n", reading.c_str());
			
			for (int i=0; i<reading.length(); i++)
			{
				if (!(i%8))
				{
					printf("%s\n", buf);
					buf[0] = '\0';
				}
				sprintf(chunk, "%2X", reading[i]);
				strcat(buf, chunk);
			}
			if (buf[0] != '\0')
				printf("%s\n", buf);
			std::cout << ": dbg2\n";*/
			char str[500];
			char buffer[500];
			int numBytes = rpi_mpu_dev->getReading(buffer);
			buffer[numBytes] = '\0';
			printf("numBytes: %d msg: %s\n", numBytes, buffer);
			
			sprintf(str, "%2X\n", buffer);
			printf("%s\n", str);
		}	
		else
			std::cout << "TimeOut reached. No data received!\n";
	}
    
    delete rpi_mpu_dev;

    return 0;
}
