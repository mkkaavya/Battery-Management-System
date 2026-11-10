#include <stdio.h>
#include <stdarg.h>
#include "logger.h"

static FILE *s_fp = NULL;
static uint32_t s_ms = 0;

int log_init(const char *path) { s_fp = fopen(path, "w"); return s_fp ? 0 : -1; }
void log_set_time(uint32_t ms) { s_ms = ms; }

void log_event(const char *fmt, ...) {
    char msg[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof msg, fmt, ap);
    va_end(ap);

    uint32_t s = s_ms / 1000;
    char line[320];
    snprintf(line, sizeof line, "[%02u:%02u.%03u] %s\n",
             (unsigned)(s / 60), (unsigned)(s % 60), (unsigned)(s_ms % 1000), msg);
    fputs(line, stdout);
    if (s_fp) fputs(line, s_fp);
}

void log_close(void) { if (s_fp) { fclose(s_fp); s_fp = NULL; } }
