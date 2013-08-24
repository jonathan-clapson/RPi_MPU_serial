/*#include <deque> 
#include <iostream> 
#include <boost/bind.hpp> 
#include <boost/asio.hpp> 
#include <boost/asio/serial_port.hpp> 
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp> 
#include <boost/date_time/posix_time/posix_time_types.hpp> 

using namespace std; 

int flag = 0;


class serial_io
{ 
public: 
	serial_io(const string& device, unsigned int baud)
	: io(), serial(io, device) 
	{ 
		if (not serial.is_open()) 
		{ 
			cerr << "Failed to open serial port\n"; 
			return; 
		} 
		boost::asio::serial_port_base::baud_rate baud_option(baud); 
		serial.set_option(baud_option); // set the baud rate after the port has been opened 
	} 
        
	void write(std::string& msg)
	{
		boost::asio::write(serial, boost::asio::buffer(msg.c_str(),msg.size()));
	}
	
	void async_read(void) 
	{ // Start an asynchronous read and call read_complete when it completes or fails 
		serial.async_read_some(boost::asio::buffer(read_data, read_buf_size), 
		boost::bind(&serial_io::read_complete, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)); 
		std::cout << "sent read\n";
	} 

	void close ( void )
	{
		serial.close(); 		
	} 

private: 

	//static const int max_read_length = 512; // maximum amount of data to read in one operation 
	std::size_t read_data_bytes = 0;
	static const int read_buf_size = 512;	

	void read_complete(const boost::system::error_code& error, size_t bytes_transferred) 
	{ // the asynchronous read operation has now completed or failed and returned an error 
		if (!error) 
		{ // read completed, so process the data 
			cout.write(read_data, bytes_transferred); // echo to standard output 
		} 
		else 
		{
			serial.close(); 
			std::cout << "read err!\n";
		}
		flag = 1;
	} 
	
private: 
	boost::asio::io_service io;
	boost::asio::serial_port serial;	
	char read_data[read_buf_size];

    deque<char> write_msgs_; // buffered write data 
}; 

int main(int argc, char* argv[]) 
{ 
	serial_io serial(std::string("/dev/ttyS0"), 115200);
	std::cout << "entered\n";
	string a = std::string("a");
	serial.async_read();
	
	
	while (flag = 0);
	//serial.close();

	return 0; 
} 
*/

/* minicom.cpp 
        A simple demonstration minicom client with Boost asio 
        
        Parameters: 
                baud rate 
                serial port (eg /dev/ttyS0 or COM1) 
                
        To end the application, send Ctrl-C on standard input 
*/ 

#include <deque> 
#include <iostream> 
#include <boost/bind.hpp> 
#include <boost/asio.hpp> 
#include <boost/asio/serial_port.hpp> 
#include <boost/thread.hpp> 
#include <boost/lexical_cast.hpp> 
#include <boost/date_time/posix_time/posix_time_types.hpp> 

using namespace std; 

int flag = 1;

class minicom_client 
{ 
public: 
        minicom_client(boost::asio::io_service& io_service, unsigned int baud, const string& device) 
                  ://io_service_(io_service), 
                  serialPort(io_service, device) 
        { 
                if (not serialPort.is_open()) 
                { 
                        cerr << "Failed to open serial port\n"; 
                        return; 
                } 
                serialPort.set_option(boost::asio::serial_port_base::baud_rate(baud)); // set the baud rate after the port has been opened                 
        } 
        
        minicom_client(unsigned int baud, const string& device) 
                  :serialPort(io_service, device) 
        { 
                if (not serialPort.is_open()) 
                { 
                        cerr << "Failed to open serial port\n"; 
                        return; 
                } 
                serialPort.set_option(boost::asio::serial_port_base::baud_rate(baud)); // set the baud rate after the port has been opened                 
        } 
        
        void write(const char msg) // pass the write data to the do_write function via the io service in the other thread 
        { 
                io_service.post(boost::bind(&minicom_client::do_write, this, msg)); 
        } 
        
        std::string read( void )
        {
			read_start(); 
			std::cout << "sent read\n";
		}
        
        void close() // call the do_close function via the io service in the other thread 
        { 
			serialPort.close();
        } 
        
        bool is_open()
        {
			return serialPort.is_open();
		}
		
private: 
        static const int max_read_length = 512; // maximum amount of data to read in one operation 
        
        void read_start(void) 
        { // Start an asynchronous read and call read_complete when it completes or fails 
                serialPort.async_read_some(boost::asio::buffer(read_msg_, max_read_length), 
                        boost::bind(&minicom_client::read_complete, 
                                this, 
                                boost::asio::placeholders::error, 
                                boost::asio::placeholders::bytes_transferred)); 
        } 
        
        void read_complete(const boost::system::error_code& error, size_t bytes_transferred) 
        { // the asynchronous read operation has now completed or failed and returned an error 
			std::cout << "entered callback\n";
                if (!error) 
                { // read completed, so process the data 
                        cout.write(read_msg_, bytes_transferred); // echo to standard output 
                        std::cout << "no err\n";
                } 
                else 
                {
					std::cout << "err close\n";
					serialPort.close();
					std::cout << "err closed\n";
				}
			std::cout << "callback complete transferred: " << bytes_transferred << "\n";
			flag = 0;
        } 
        
        void do_write(const char msg) 
        { // callback to handle write call from outside this class 
                bool write_in_progress = !write_msgs_.empty(); // is there anything currently being written? 
                write_msgs_.push_back(msg); // store in write buffer 
                if (!write_in_progress) // if nothing is currently being written, then start 
                        write_start(); 
        } 
        
        void write_start(void) 
        { // Start an asynchronous write and call write_complete when it completes or fails 
                boost::asio::async_write(serialPort, 
                        boost::asio::buffer(&write_msgs_.front(), 1), 
                        boost::bind(&minicom_client::write_complete, 
                                this, 
                                boost::asio::placeholders::error)); 
        } 
        
        void write_complete(const boost::system::error_code& error) 
        { // the asynchronous read operation has now completed or failed and returned an error 
                if (!error) 
                { // write completed, so send next write data 
                        write_msgs_.pop_front(); // remove the completed data 
                        if (!write_msgs_.empty()) // if there is anthing left to be written 
                                write_start(); // then start sending the next item in the buffer 
                } 
                else 
					serialPort.close();
        } 
        
private: 
        //boost::asio::io_service& io_service_; // the main IO service that runs this connection 
        boost::asio::io_service io_service; // the main IO service that runs this connection 
        boost::asio::serial_port serialPort; // the serial port this instance is connected to 
        char read_msg_[max_read_length]; // data read from the socket 
        deque<char> write_msgs_; // buffered write data 
}; 

int main(int argc, char* argv[]) 
{ 
// on Unix POSIX based systems, turn off line buffering of input, so cin.get() returns after every keypress 
// On other systems, you'll need to look for an equivalent 
 
	if (argc != 3) 
	{ 
		cerr << "Usage: minicom <baud> <device>\n"; 
		return 1; 
	} 
	boost::asio::io_service io_service; 
	// define an instance of the main class of this program 
	//minicom_client c(io_service, boost::lexical_cast<unsigned int>(argv[1]), argv[2]); 
	minicom_client c(boost::lexical_cast<unsigned int>(argv[1]), argv[2]); 
	// run the IO service as a separate thread, so the main thread can block on standard input 
	boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service)); 
	/*while (1) // check the internal state of the connection to make sure it's still running 
	{ 
		char ch; 
		cin.get(ch); // blocking wait for standard input 
		if (ch == 3) // ctrl-C to end program 
		break; 
		c.write(ch); 
	} */
	c.read();
	
	while (flag)
	{
		std::cout << "waiting for data\n";
	};
	
	if (c.is_open())
	{
		c.close(); // close the minicom client connection 
		std::cout << "ser port still open closing\n";
	}
	t.join(); // wait for the IO service thread to close 


	return 0; 
} 
