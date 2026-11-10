#ifndef LOGGER_H
#define LOGGER_H
#include <stdint.h>
int  log_init(const char *path);
void log_set_time(uint32_t ms);
void log_event(const char *fmt, ...);
void log_close(void);
#endif
