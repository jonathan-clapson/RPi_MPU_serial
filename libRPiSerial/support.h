#ifndef __SUPPORT_H__
#define __SUPPORT_H__

double hexCharsToDouble(char *buf);

extern bool write_allowed;
bool input_available();
int print(const char* fmt, ...);

#endif /* __SUPPORT_H__ */
