#include <cstdarg>
#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

bool input_available() 
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return (FD_ISSET(0, &fds));
}

bool write_allowed = true;
int print(const char* fmt, ...){
	int ret_val=0;

	va_list ap;
	va_start(ap, fmt);

	if (write_allowed)
		ret_val = vprintf(fmt, ap);

	va_end(ap);
	
	return ret_val;
}

struct termios old_tio;
void disable_line_buffering()
{
	struct termios new_tio;

	/* get the terminal settings for stdin */
	tcgetattr(STDIN_FILENO,&old_tio);

	/* we want to keep the old setting to restore them a the end */
	new_tio=old_tio;

	/* disable canonical mode (buffered i/o) */
	new_tio.c_lflag &=(~ICANON);

	/* set the new settings immediately */
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);
}

void reset_terminal()
{	
	/* restore the former settings */
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
}


