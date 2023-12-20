#include <stdlib.h>
#include <time.h>
#include <check.h>

#include "rfc3339.h"

extern void tzset(void);
extern int setenv(const char *name, const char *value, int overwrite);

#define TIMESTAMP_DEC 1702808945
#define TIMESTAMP_JUN 1686997745

/* gmtime_to_rfc3339_utc */

START_TEST(gmtime_to_rfc3339_utc_test)
{
	time_t rawtime = TIMESTAMP_JUN;
	struct tm *ptm = gmtime(&rawtime);

	ck_assert_str_eq(gmtime_to_rfc3339_utc(ptm), "2023-06-17T10:29:05Z");
}

END_TEST;

/* localtime_to_rfc3339_local */
START_TEST(localtime_to_rfc3339_local_test__utc)
{
	setenv("TZ", "UTC", 1);
	tzset();

	time_t rawtime = TIMESTAMP_JUN;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-06-17T10:29:05+00:00");
}

END_TEST;

START_TEST(localtime_to_rfc3339_local_test__moscow_jun)
{
	setenv("TZ", "Europe/Moscow", 1);
	tzset();

	time_t rawtime = TIMESTAMP_JUN;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-06-17T13:29:05+03:00");
}

END_TEST;

START_TEST(localtime_to_rfc3339_local_test__moscow_dec)
{
	setenv("TZ", "Europe/Moscow", 1);
	tzset();

	time_t rawtime = TIMESTAMP_DEC;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-12-17T13:29:05+03:00");
}

END_TEST;

START_TEST(localtime_to_rfc3339_local_test__adelaide_jun)
{
	setenv("TZ", "Australia/Adelaide", 1);
	tzset();

	time_t rawtime = TIMESTAMP_JUN;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-06-17T19:59:05+09:30");
}

END_TEST;

START_TEST(localtime_to_rfc3339_local_test__adelaide_dec)
{
	setenv("TZ", "Australia/Adelaide", 1);
	tzset();

	time_t rawtime = TIMESTAMP_DEC;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-12-17T20:59:05+10:30");
}

END_TEST;

START_TEST(localtime_to_rfc3339_local_test__anchorage_jun)
{
	setenv("TZ", "America/Anchorage", 1);
	tzset();

	time_t rawtime = TIMESTAMP_JUN;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-06-17T02:29:05-08:00");
}

END_TEST;

START_TEST(localtime_to_rfc3339_local_test__anchorage_dec)
{
	setenv("TZ", "America/Anchorage", 1);
	tzset();

	time_t rawtime = TIMESTAMP_DEC;
	struct tm *ptm = localtime(&rawtime);

	ck_assert_str_eq(localtime_to_rfc3339_local(ptm), "2023-12-17T01:29:05-09:00");
}

END_TEST
/* rfc3339_to_time_t */
START_TEST(rfc3339_to_time_t_test__utc_dec)
{
	setenv("TZ", "UTC", 1);
	tzset();

	time_t timestamp = TIMESTAMP_DEC;

	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__moscow_jun)
{
	setenv("TZ", "Europe/Moscow", 1);
	tzset();

	time_t timestamp = TIMESTAMP_JUN;

	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__moscow_dec)
{
	setenv("TZ", "Europe/Moscow", 1);
	tzset();

	time_t timestamp = TIMESTAMP_DEC;

	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__anchorage_jun)
{
	setenv("TZ", "Europe/Anchorage", 1);
	tzset();

	time_t timestamp = TIMESTAMP_JUN;

	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__anchorage_dec)
{
	setenv("TZ", "America/Anchorage", 1);
	tzset();

	time_t timestamp = TIMESTAMP_DEC;

	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__adelaide_jun)
{
	setenv("TZ", "Europe/Adelaide", 1);
	tzset();

	time_t timestamp = TIMESTAMP_JUN;

	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-06-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__adelaide_dec)
{
	setenv("TZ", "Australia/Adelaide", 1);
	tzset();

	time_t timestamp = TIMESTAMP_DEC;

	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T01:29:05-09:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T02:29:05-08:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T10:29:05+00:00"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T13:29:05+03:00"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__format_z)
{
	time_t timestamp = TIMESTAMP_DEC;

	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T10:29:05Z"), timestamp);
}

END_TEST;

START_TEST(rfc3339_to_time_t_test__format_ms)
{
	time_t timestamp = TIMESTAMP_DEC;

	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T10:29:05.1234567890Z"), timestamp);
	ck_assert_int_eq(rfc3339_to_time_t("2023-12-17T13:29:05.1234567890+03:00"), timestamp);
}

END_TEST;

Suite *rfc3339_suite(void)
{
	Suite *s = suite_create("rfc3339");

	TCase *tc0 = tcase_create("gmtime_to_rfc3339_utc");

	tcase_add_test(tc0, gmtime_to_rfc3339_utc_test);
	suite_add_tcase(s, tc0);

	TCase *tc1 = tcase_create("localtime_to_rfc3339_local");

	tcase_add_test(tc1, localtime_to_rfc3339_local_test__utc);
	tcase_add_test(tc1, localtime_to_rfc3339_local_test__moscow_jun);
	tcase_add_test(tc1, localtime_to_rfc3339_local_test__moscow_dec);
	tcase_add_test(tc1, localtime_to_rfc3339_local_test__adelaide_jun);
	tcase_add_test(tc1, localtime_to_rfc3339_local_test__adelaide_dec);
	tcase_add_test(tc1, localtime_to_rfc3339_local_test__anchorage_jun);
	tcase_add_test(tc1, localtime_to_rfc3339_local_test__anchorage_dec);

	suite_add_tcase(s, tc1);

	TCase *tc2 = tcase_create("rfc3339_to_time_t");

	tcase_add_test(tc2, rfc3339_to_time_t_test__utc_dec);
	tcase_add_test(tc2, rfc3339_to_time_t_test__moscow_jun);
	tcase_add_test(tc2, rfc3339_to_time_t_test__moscow_dec);
	tcase_add_test(tc2, rfc3339_to_time_t_test__adelaide_jun);
	tcase_add_test(tc2, rfc3339_to_time_t_test__adelaide_dec);
	tcase_add_test(tc2, rfc3339_to_time_t_test__anchorage_jun);
	tcase_add_test(tc2, rfc3339_to_time_t_test__anchorage_dec);
	tcase_add_test(tc2, rfc3339_to_time_t_test__format_z);
	tcase_add_test(tc2, rfc3339_to_time_t_test__format_ms);
	suite_add_tcase(s, tc2);

	return s;
}
