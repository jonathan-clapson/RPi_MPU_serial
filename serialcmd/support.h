#ifndef __SUPPORT_H__
#define __SUPPORT_H__

extern bool write_allowed;
bool input_available();
int print(const char* fmt, ...);

void disable_line_buffering();
void reset_terminal();

#endif /* __SUPPORT_H__ */
