#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "rfc3339.h"

#define DATETIME_FORMAT "%Y-%m-%dT%H:%M:%S%z"
size_t DATETIME_FORMAT_LEN = strlen("2006-01-02T15:04:05Z07:00") + 1;
size_t DATETIME_COLON_POS = strlen("2006-01-02T15:04:05Z07"); // len is a ":"'s position

#define DATETIME_PARSE_FORMAT "%Y-%m-%dT%H:%M:%S"

#define to_digit(c) ((c) - '0')

extern char *strdup(const char *);
extern char *strptime(const char *, const char *, struct tm *);

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
	char *buff = calloc(DATETIME_FORMAT_LEN, sizeof(char));

	if (strftime(buff, DATETIME_FORMAT_LEN, DATETIME_FORMAT, timeptr)
		&& buff[DATETIME_COLON_POS] != ':') {
		// add colon in offset: +1234\0 -> +12:34\0
		for (size_t i = DATETIME_FORMAT_LEN-1; i > DATETIME_COLON_POS; i--)
			buff[i] = buff[i - 1];

		buff[DATETIME_COLON_POS] = ':';
	}

	return buff;
}

char *gmtime_to_rfc3339_utc(const struct tm *timeptr)
{
	char *buff = calloc(DATETIME_FORMAT_LEN, sizeof(char));

	strftime(buff, DATETIME_FORMAT_LEN, DATETIME_FORMAT, timeptr);

	return buff;
}

time_t rfc3339_to_time_t(const char *formatted)
{
	struct tm *datetime = calloc(1, sizeof(struct tm));

	// see: https://datatracker.ietf.org/doc/html/rfc3339#section-5.6
	char *unparsed = strptime(formatted, DATETIME_PARSE_FORMAT, datetime);

	// we need to parse timezone offset manually
	// because %z doesn't support colon-separated format (12:43)
	int off_sign = 1;
	int gmtoff = 0;

	if (unparsed != NULL) {
		// skip second's fractions (time-secfrac)
		if (unparsed[0] == '.') {
			do {
				unparsed++;
			} while (unparsed[0] >= '0' && unparsed[0] <= '9');
		}

		// now `unparsed` is formatted like "+12:45"

		switch (unparsed[0]) {
		case '+':
			off_sign = 1;
			break;
		case '-':
			off_sign = -1;
			break;
		case 'Z':				// zero offset
			off_sign = 0;
			break;
		default:
			return -1;
		}

		if (off_sign != 0) {
			int off_hour = to_digit(unparsed[1])*10 + to_digit(unparsed[2]);
			int off_min = to_digit(unparsed[4])*10 + to_digit(unparsed[5]);

			gmtoff = off_sign * 60 * (off_hour * 60 + off_min);
		}
	}

	time_t result = mktime(datetime);

	free(datetime);

	return result != -1 ? result - gmtoff + get_local_tz_offset() : -1;
}
