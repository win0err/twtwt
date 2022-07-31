#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "commands.h"
#include "config.h"
#include "twtwt.h"
#include "quickstart.h"
#include "utils.h"

#define PRINT_PROGRAM_USAGE() PRINT_HEADER(); PRINT_COMMANDS();

#define PRINT_COMMAND_USAGE(cmd, args, errmsg)                  \
	fprintf(stderr, "usage: " PROGNAME " %s %s\n", cmd, args);  \
	fprintf(stderr, "error: %s\n", errmsg);

int main(int argc, const char **argv)
{
	if (getenv("HOME") == NULL) {
		fprintf(stderr, "error: $HOME environment variable must be set\n");

		return EXIT_FAILURE;
	}

	char *config_location = get_config_location();
	int is_config_exists = is_file_exists(config_location);

	free(config_location);

	if (argc == 1) {
		if (!is_config_exists)
			return run_quickstart();

		PRINT_PROGRAM_USAGE();

		return EXIT_SUCCESS;
	} else if (argc == 2) {
		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
			PRINT_PROGRAM_USAGE();

			return EXIT_SUCCESS;
		}

		if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
			printf("%s, version %s\n", PROGNAME, VERSION);

			return EXIT_SUCCESS;
		}

	}

	if (!is_config_exists)
		return run_quickstart();

	int rc = EXIT_SUCCESS;

	config_t *config = config_new();

	if (config_load(config) != LOAD_CONFIG_OK) {
		config_free(config);

		return EXIT_FAILURE;
	}

	if (!strcmp(argv[1], COMMAND_CONFIG)) {
		rc = command_config();
	} else if (!strcmp(argv[1], COMMAND_TWEET)) {
		if (argc < 3) {
			PRINT_COMMAND_USAGE(COMMAND_TWEET, "<text>", "text cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_tweet(argv[2], config);
		}
	} else if (!strcmp(argv[1], COMMAND_FOLLOWING)) {
		rc = command_following(config->following);
	} else if (!strcmp(argv[1], COMMAND_FOLLOW)) {
		if (argc < 4) {
			PRINT_COMMAND_USAGE(COMMAND_FOLLOW, "<nick> <url>", "nick and url cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_follow(argv[2], argv[3], config);
		}
	} else if (!strcmp(argv[1], COMMAND_UNFOLLOW)) {
		if (argc < 3) {
			PRINT_COMMAND_USAGE(COMMAND_UNFOLLOW, "<nick>", "nick cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_unfollow(argv[2], config);
		}
	} else if (!strcmp(argv[1], COMMAND_TIMELINE)) {
		rc = command_timeline(config);
	} else if (!strcmp(argv[1], COMMAND_VIEW)) {
		if (argc < 3) {
			PRINT_COMMAND_USAGE(COMMAND_VIEW, "<nick/url>", "nickname or feed url cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_view(argv[2], config);
		}
	} else {
		fprintf(stderr, "error: %s: no such command or option\n", argv[1]);

		rc = EXIT_FAILURE;
	}

	config_free(config);

	return rc;
}
