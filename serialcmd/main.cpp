#include <exception>
#include <sstream>
#include <iomanip>

#include <stdio.h>
#include <cmath>

//#include "serialib.h"

#include "rpi_mpu_io.h"
//#include "input.h"
#include "error.h"
#include "support.h"

#if defined (_WIN32) || defined( _WIN64)
#define DEVICE_PORT "COM1"			// COM1 for windows
#endif

#ifdef __linux__
#define DEVICE_PORT "/dev/ttyUSB0"	// ttyS0 for linux
#endif

/* yeah globals a bit dodge, but it seemed cleaner than pushing it round everywhere */
rpi_mpu_io* rpi_mpu_dev;
bool record_readings = 0;
bool main_loop_exit = false;

const char *command_table[] = {
	"quit",
	"dumpcalibration",
	"recordreadings",
	NULL
};
	
void process_internal_command (char *message)
{	
	//get command
	char *command = strtok(message, " ");
	
	int command_num = 0;
	while (command_table[command_num])
	{
		if (strcmp(command, command_table[command_num]) == 0)
			break;			
		command_num++;		
	}
	
	char *arg = strtok(NULL, " ");

	switch (command_num)
	{
		case 0: /* quit */
			main_loop_exit = true;
			break;
		case 1: /* dump calibration */
			if (arg)
				rpi_mpu_dev->enable_calibration_dump(std::string(arg));
			else
				rpi_mpu_dev->enable_calibration_dump();
			break;
		case 2: /* record readings */
			record_readings = !record_readings;			
		default:
			break;		
	}
}

int main()
{
	FILE *fp = NULL;
	
    rpi_mpu_dev = new rpi_mpu_io((char *)DEVICE_PORT, 2000000);
    // Open serial port device
    if (!rpi_mpu_dev->device_valid()) 
    {
		delete rpi_mpu_dev;
		return -1;
	}
    printf ("Serial port opened successfully!\n");
    
	disable_line_buffering();
    write_allowed = true;
    
	while(!main_loop_exit){		
		//FIXME: hard coded for 6 readings 
		struct reading_memory_type readings[6];
		
		if (input_available())
		{
			write_allowed = false;
		
			char input_buf[201]; //1 more for appending '\n'
			fgets(input_buf, 200, stdin);
			if (input_buf[0] == ';')
			{
				input_buf[strlen(input_buf) -1] = '\0';
				process_internal_command(input_buf + 1);				
			}
			else
			{
				strcat(input_buf, "\n");
				rpi_mpu_dev->writeString(input_buf);				
			}
		}
		write_allowed = true;
		
		int ret = rpi_mpu_dev->getReading(readings);
		
		//enable/disable csv file export
		if (!fp && record_readings == true)
		{
			//FIXME: hard coded file name
			printf("opening file\n");
			fp = fopen("reading_record.csv", "w");
			for (int i=0; i<6; i++)
				fprintf(fp, "RPi_Dev,ts,x,y,z,vx,vy,vz,ax,ay,az,temp,ox,oy,oz,wx,wy,wz,");
			fprintf(fp, "\n");
		}
		if (fp && record_readings == false)
		{
			printf("closing file\n");
			fclose(fp);
		}		
		
		if (fp)
		{
			for (int i=0; i<6; i++)
				//fprintf(fp, "%d,%ld,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,%6.0f,",
				fprintf(fp, "%d,%ld,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,",
					i,
					readings[i].timestamp,
					readings[i].x, readings[i].y, readings[i].z, 
					readings[i].v_x, readings[i].v_y, readings[i].v_z,
					readings[i].a_x, readings[i].a_y, readings[i].a_z,
					readings[i].temp,
					readings[i].o_x, readings[i].o_y, readings[i].o_z,
					readings[i].w_x, readings[i].w_y, readings[i].w_z);
			
			fprintf(fp, "\n");
		}
		
		//show readings
		if (ret == ERR_GOTREADING) 
		{
			for (int i=0; i<6; i++) 
			{
				print("RPi_Dev%d: ts %ld x %7f y %7f z %7f vx %7f vy %7f vz %7f ax %7f ay %7f az %7f temp %7f ox %7f oy %7f oz %7f wx %7f wy %7f wz %7f\n",
				//print("RPi_Dev%d: ts %ld x %6.0f y %6.0f z %6.0f vx %6.0f vy %6.0f vz %6.0f ax %6.0f ay %6.0f az %6.0f temp %6.0f ox %6.0f oy %6.0f oz %6.0f wx %6.0f wy %6.0f wz %6.0f\n",
					i,
					readings[i].timestamp,
					readings[i].x, readings[i].y, readings[i].z, 
					readings[i].v_x, readings[i].v_y, readings[i].v_z,
					readings[i].a_x, readings[i].a_y, readings[i].a_z,
					readings[i].temp,
					readings[i].o_x, readings[i].o_y, readings[i].o_z,
					readings[i].w_x, readings[i].w_y, readings[i].w_z);
			}			
		}	
		/*else if (ret != ERR_DBG)
			std::cout << "TimeOut reached. No data received!\n";*/
	}
	
	if (fp)
		fclose(fp);
   
    delete rpi_mpu_dev;

    return 0;
}
