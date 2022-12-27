#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "rfc3339.h"

#define LOCAL_FMT "%Y-%m-%dT%H:%M:%S%z"
#define LOCAL_FMT_LEN 26

#define UTC_FMT "%Y-%m-%dT%H:%M:%SZ"
#define UTC_FMT_LEN 21

#define DATETIME_PARSE_FORMAT "%Y-%m-%dT%H:%M:%S%z"

#define OFFSET_COLON_POS 22
#define OFFSET_POS 19			// +, - or Z

extern char *strdup(const char *);
extern char *strptime(const char *, const char *, struct tm *);
extern void tzset(void);

static time_t get_local_tz_offset()
{
	time_t rawtime = time(0);

	struct tm *ptm = gmtime(&rawtime);
	time_t gmt = mktime(ptm);

	ptm = localtime(&rawtime);

	return rawtime - gmt + (ptm->tm_isdst ? 3600 : 0);
}

char *localtime_to_rfc3339_local(const struct tm *timeptr)
{
	char *buff = calloc(LOCAL_FMT_LEN + 1, sizeof(char));

	if (strftime(buff, LOCAL_FMT_LEN, LOCAL_FMT, timeptr) && buff[OFFSET_COLON_POS] != ':') {
		// add colon in offset: +1234\0 -> +12:34\0
		for (int i = LOCAL_FMT_LEN; i > OFFSET_COLON_POS; i--)
			buff[i] = buff[i - 1];

		buff[OFFSET_COLON_POS] = ':';
	}

	return buff;
}

char *gmtime_to_rfc3339_utc(const struct tm *timeptr)
{
	char *buff = calloc(UTC_FMT_LEN, sizeof(char));

	strftime(buff, UTC_FMT_LEN, UTC_FMT, timeptr);

	return buff;
}

time_t rfc3339_to_time_t(const char *formatted)
{
	tzset();					// to calculate timezone offset

	struct tm *datetime = calloc(1, sizeof(struct tm));

	char *buff = strdup(formatted);

	buff[OFFSET_POS] = 0;		// 1999-08-28T13:45:59\0

	if (strptime(buff, DATETIME_PARSE_FORMAT, datetime) != NULL) {
		return -1;
	}

	// we need to parse timezone offset manually
	// because %z doesn't work as expected
	int off_sign = 1;
	int off_hour = 0, off_min = 0;

	free(buff);

	size_t idx = OFFSET_POS;

	buff = strdup(formatted);

	// skip second's fractions
	if (buff[idx] == '.' || buff[idx] == ',') {
		do {
			idx++;
		} while (buff[idx] >= '0' && buff[idx] <= '9');
	}

	switch (buff[idx]) {
	case '+':
		off_sign = 1;
		break;
	case '-':
		off_sign = -1;
		break;
	case 'Z':					// zero offset
		off_sign = 0;
		break;
	default:
		return -1;
	}

	int gmtoff = 0;

	if (off_sign != 0) {
		char off_hour_s[3] = { buff[idx + 1], buff[idx + 2], 0 };
		off_hour = atoi(off_hour_s);

		char off_min_s[3] = { buff[idx + 4], buff[idx + 5], 0 };
		off_min = atoi(off_min_s);

		gmtoff = off_sign * 60 * (off_hour * 60 + off_min);
	}

	time_t result = mktime(datetime);

	free(buff);
	free(datetime);

	return result != -1 ? result - gmtoff + get_local_tz_offset() : -1;
}
