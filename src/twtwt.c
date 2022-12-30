#include <getopt.h>
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

#define PRINT_PROGRAM_VERSION() printf("%s, version %s\n", PROGNAME, VERSION);
#define PRINT_PROGRAM_USAGE() PRINT_HEADER(); puts(""); PRINT_COMMANDS();

#define PRINT_COMMAND_USAGE(cmd, args, errmsg)                  \
	fprintf(stderr, "usage: " PROGNAME " %s %s\n", cmd, args);  \
	fprintf(stderr, "error: %s\n", errmsg);

static struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"limit", required_argument, NULL, 'l'},
	{"page", required_argument, NULL, 'p'},
	{NULL, 0, NULL, 0}
};

int main(int argc, char *const *argv)
{
	if (getenv(EHOME) == NULL) {
		fprintf(stderr, "error: $HOME environment variable must be set\n");

		return EXIT_FAILURE;
	}

	char *config_location = get_config_location();
	int is_config_exists = is_file_exists(config_location);

	free(config_location);

	int limit = 20, page = 1;

	int c;

	// int option_index = 0;
	while ((c = getopt_long(argc, argv, "hvl:p:", long_options, NULL)) != -1) {
		switch (c) {
		case 'h':
			PRINT_PROGRAM_USAGE();
			return EXIT_SUCCESS;
		case 'v':
			PRINT_PROGRAM_VERSION();
			return EXIT_SUCCESS;
		case 'l':
			if (atoi(optarg) >= 0)	// 0 -- print all
				limit = atoi(optarg);
			break;
		case 'p':
			if (atoi(optarg) > 0)
				page = atoi(optarg);
			break;
		case '?':
			break;
		default:
			fprintf(stderr, "error: getopt returned character code 0%o\n", c);
		}
	}

	if (optind >= argc) {
		if (is_config_exists) {
			PRINT_PROGRAM_USAGE();
			return EXIT_SUCCESS;
		}

		return run_quickstart();
	}

	const char *command = argv[optind++];
	int params_count = argc - optind;

	int rc = EXIT_SUCCESS;

	config_t *config = config_new();

	if (config_load(config) != LOAD_CONFIG_OK) {
		config_free(config);

		return EXIT_FAILURE;
	}

	if (!strcmp(command, COMMAND_CONFIG)) {
		rc = command_config();
	} else if (!strcmp(command, COMMAND_TWEET)) {
		if (params_count != 1) {
			PRINT_COMMAND_USAGE(COMMAND_TWEET, "<text>", "text cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_tweet(argv[optind], config);
		}
	} else if (!strcmp(command, COMMAND_FOLLOWING)) {
		rc = command_following(config->following);
	} else if (!strcmp(command, COMMAND_FOLLOW)) {
		if (params_count != 2) {
			PRINT_COMMAND_USAGE(COMMAND_FOLLOW, "<nick> <url>", "nick and url cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_follow(argv[optind], argv[optind + 1], config);
		}
	} else if (!strcmp(command, COMMAND_UNFOLLOW)) {
		if (params_count != 1) {
			PRINT_COMMAND_USAGE(COMMAND_UNFOLLOW, "<nick>", "nick cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_unfollow(argv[optind], config);
		}
	} else if (!strcmp(command, COMMAND_TIMELINE)) {
		rc = command_timeline(config, page, limit);
	} else if (!strcmp(command, COMMAND_VIEW)) {
		if (params_count != 1) {
			PRINT_COMMAND_USAGE(COMMAND_VIEW, "<nick/url>", "nickname or feed url cannot be empty");

			rc = EXIT_FAILURE;
		} else {
			rc = command_view(argv[optind], config, page, limit);
		}
	} else if (!strcmp(command, COMMAND_FILE)) {
		if (params_count != 1) {
			PRINT_COMMAND_USAGE(COMMAND_FILE, "<edit/pull/push>", "subcommand is required");

			rc = EXIT_FAILURE;
		} else if (strcmp(argv[optind], "edit") != 0 && strcmp(argv[optind], "pull") != 0
				   && strcmp(argv[optind], "push") != 0) {
			PRINT_COMMAND_USAGE(COMMAND_FILE, "<edit/pull/push>", "wrong subcommand");

			rc = EXIT_FAILURE;
		} else {
			rc = command_file(argv[optind], config);
		}
	} else {
		fprintf(stderr, "error: %s: no such command or option\n", command);

		rc = EXIT_FAILURE;
	}

	config_free(config);

	return rc;
}
