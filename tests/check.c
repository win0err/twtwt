#include <stdlib.h>
#include <check.h>

Suite *blank_suite(void)
{
	Suite *s = suite_create("");

	return s;
}

Suite *rfc3339_suite(void);

int main(void)
{
	SRunner *sr = srunner_create(blank_suite());

	srunner_add_suite(sr, rfc3339_suite());

	srunner_run_all(sr, CK_NORMAL);
	int no_failed = srunner_ntests_failed(sr);

	srunner_free(sr);

	return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
