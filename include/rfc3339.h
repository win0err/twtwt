#ifndef _TWTWT_RFC3339_H
#define _TWTWT_RFC3339_H

#include <time.h>

char *localtime_to_rfc3339_local(const struct tm *timeptr);
char *gmtime_to_rfc3339_utc(const struct tm *timeptr);
time_t rfc3339_to_time_t(const char *formatted);

#endif
