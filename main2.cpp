#include <iostream>
#include <boost/asio.hpp>
#include <string>

class serial_io
{	
private:
	boost::asio::io_service io;
	boost::asio::serial_port serial;	
	std::size_t read_data_bytes = 0;
	
	const static int read_buf_size = 512;
	char read_data[read_buf_size];
	const void * read_data_temp;
	
	void handler(const boost::system::error_code& error, long unsigned int bytes_transferred)
	{
		read_data[read_buf_size-1] = '\0';
		printf("%s", read_data);
		read_data_bytes = bytes_transferred;
	}	
	
public:
	serial_io(const std::string& device, unsigned int baud)
	: io(), serial(io,device)
	{
		if (!serial.is_open())
		{
			std::cerr << "Failed to open serial port\n";
			return;			
		}
		serial.set_option(boost::asio::serial_port_base::baud_rate(baud));
	}
	
	void write(std::string& msg)
	{
		boost::asio::write(serial, boost::asio::buffer(msg.c_str(),msg.size()));
	}
	
	void close()
	{
		serial.close();
	}
	
	std::size_t data_available()
	{
		return read_data_bytes;
	}
	
	void read_async( void )
	{
		read_data_temp = read_data;
		serial.async_read_some(boost::asio::buffer(read_data_temp, read_buf_size), &handler);
	}	
};

int main(int argc, char *argv[])
{
	serial_io serial(std::string("/dev/ttyS0"), 115200);
	while (1)
	{	
		serial.read_async();		
	}
	
	return 0;
}	
