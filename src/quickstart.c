#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "quickstart.h"
#include "twtwt.h"
#include "utils.h"

#define BUFSIZE 2048

extern char *strdup(const char *);

static void get_nick(config_t * config)
{
	char *os_user = getenv("USER");
	char *default_val = strdup((os_user == NULL) ? "anonymous" : os_user);
	char *nick = calloc(BUFSIZE, sizeof(char));

	printf("your nickname [%s]: ", default_val);
	fgets(nick, BUFSIZE, stdin);

	nick = trim(nick);

	if (strlen(nick) == 0) {
		config->nick = default_val;
		free(nick);
	} else {
		config->nick = nick;
		free(default_val);
	}
}

static void get_twtfile(config_t * config)
{
	char *default_val = strdup(getenv("HOME"));
	size_t len = strlen(default_val);

	// remove trailing slash if exists
	if (default_val[len - 1] == '/')
		default_val[len - 1] = '\0';

	default_val = (char *) realloc(default_val, len + strlen("/twtxt.txt") + 1);
	strcat(default_val, "/twtxt.txt");

	char *twtfile = calloc(BUFSIZE, sizeof(char));

	printf("local location of your twtxt.txt file [%s]: ", default_val);
	fgets(twtfile, BUFSIZE, stdin);
	twtfile = trim(twtfile);

	if (strlen(twtfile) == 0) {
		config->twtfile = default_val;
		free(twtfile);
	} else {
		config->twtfile = twtfile;
		free(default_val);
	}
}

static void get_twturl(config_t * config)
{
	char *domain = "https://example.com/";
	char *filename = basename(config->twtfile);
	size_t len = strlen(filename) + strlen(domain) + 1;

	char *default_val = calloc(len, sizeof(char));

	snprintf(default_val, len, "%s%s", domain, filename);

	char *twturl = calloc(BUFSIZE, sizeof(char));

	printf("url of your twtxt.txt on the web [%s]: ", default_val);
	fgets(twturl, BUFSIZE, stdin);
	twturl = trim(twturl);

	if (strlen(twturl) == 0) {
		config->twturl = default_val;
		free(twturl);
	} else {
		config->twturl = twturl;
		free(default_val);
	}
}

static int create_twtxt_file(config_t * config)
{
	FILE *fp = ensure_fopen(config->twtfile, "a");

	if (fp == NULL) {
		fprintf(stderr, "error: cannot create twtxt file\n");

		return EXIT_FAILURE;
	}

	fprintf(fp, TWTXT_FILE_CONTENTS_TEMPLATE, config->nick, config->twturl, config->nick, config->twturl);

	fclose(fp);

	return EXIT_SUCCESS;
}

int run_quickstart()
{
	PRINT_HEADER();

	config_t *config = config_new();

	get_nick(config);
	get_twtfile(config);
	get_twturl(config);

	if (config_save(config) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	puts("");

	char *config_location = get_config_location();

	printf("config file created at %s\n", config_location);
	free(config_location);

	if (create_twtxt_file(config) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	printf("twtxt file created at %s\n\n", config->twtfile);

	config_free(config);

	printf("enjoy using %s!\n", PROGNAME);

	return EXIT_SUCCESS;
}
