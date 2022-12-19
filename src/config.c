#include <assert.h>
#include <ini.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "list.h"
#include "tweet.h"
#include "utils.h"

extern char *strdup(const char *);

static int handler(void *cfg, const char *section, const char *key, const char *value)
{
	config_t *cgfptr = (config_t *) cfg;

	if (value[0] == '\'' || value[0] == '"') {
		fprintf(stderr,
				"error: config: %s: escaped values is not supported in config file, please, unescape: %s\n",
				key, value);

		exit(EXIT_FAILURE);
	}

	if (!strcmp(section, "twtxt")) {
		if (!strcmp(key, "nick")) {
			cgfptr->nick = strdup(value);
		} else if (!strcmp(key, "twtfile")) {
			cgfptr->twtfile = strdup(value);
		} else if (!strcmp(key, "twturl")) {
			cgfptr->twturl = strdup(value);
		} else if (!strcmp(key, "pre_tweet_hook")) {
			cgfptr->pre_tweet_hook = strdup(value);
		} else if (!strcmp(key, "post_tweet_hook")) {
			cgfptr->post_tweet_hook = strdup(value);
		} else {
			return 0;
		}
	} else if (!strcmp(section, "following")) {
		user_t *u = (user_t *) malloc(sizeof(user_t));

		u->nick = strdup(key);
		u->url = strdup(value);

		list_push(cgfptr->following, list_node_new(u));
	}

	return 1;
}

config_t *config_new()
{
	config_t *cfg = calloc(1, sizeof(config_t));

	cfg->nick = NULL;
	cfg->twtfile = NULL;
	cfg->twturl = NULL;
	cfg->following = list_new();

	return cfg;
}

int config_load(config_t * cfg)
{
	char *filename = get_config_location();
	int result = ini_parse(filename, handler, cfg);

	free(filename);

	switch (result) {
	case LOAD_CONFIG_OK:
		return EXIT_SUCCESS;

	case LOAD_CONFIG_READ_ERROR:
		fprintf(stderr, "error: config file not found or not readable\n");
		return LOAD_CONFIG_READ_ERROR;

	case LOAD_CONFIG_MEMORY_ERROR:
		fprintf(stderr, "error: memory allocation error when parsing config file\n");
		return EXIT_FAILURE;

	default:
		fprintf(stderr, "error: parse error in config, line %d\n", result);
		return EXIT_FAILURE;
	}
}

int config_save(config_t * cfg)
{
	char *filename = get_config_location();

	FILE *fp = ensure_fopen(filename, "w");

	if (fp == NULL) {
		fprintf(stderr, "error: cannot open config file: `%s`\n", filename);
		free(filename);

		return EXIT_FAILURE;
	}

	fprintf(fp, "[twtxt]\n");
	fprintf(fp, "nick = %s\n", cfg->nick);
	fprintf(fp, "twtfile = %s\n", cfg->twtfile);
	fprintf(fp, "twturl = %s\n", cfg->twturl);
	if (cfg->pre_tweet_hook != NULL)
		fprintf(fp, "pre_tweet_hook = %s\n", cfg->pre_tweet_hook);
	if (cfg->post_tweet_hook != NULL)
		fprintf(fp, "post_tweet_hook = %s\n", cfg->post_tweet_hook);
	fprintf(fp, "\n");
	fprintf(fp, "[following]\n");

	list_node_t *node = NULL;
	user_t *user = NULL;

	list_for_each(cfg->following, node) {
		user = (user_t *) node->data;
		fprintf(fp, "%s = %s\n", user->nick, user->url);
	}

	fflush(fp);
	fclose(fp);

	free(filename);

	return EXIT_SUCCESS;
}

void user_free(void *node_data)
{
	if (node_data == NULL)
		return;

	user_t *u = (user_t *) node_data;

	if (u->nick != NULL) {
		free((void *) u->nick);
		u->nick = NULL;
	}

	if (u->url != NULL) {
		free((void *) u->url);
		u->url = NULL;
	}
}

void config_free(config_t * cfg)
{
	if (cfg == NULL)
		return;

	if (cfg->nick)
		free((void *) cfg->nick);
	if (cfg->twtfile)
		free((void *) cfg->twtfile);
	if (cfg->twturl)
		free((void *) cfg->twturl);
	if (cfg->pre_tweet_hook)
		free((void *) cfg->pre_tweet_hook);
	if (cfg->post_tweet_hook)
		free((void *) cfg->post_tweet_hook);
	if (cfg->following)
		list_free(cfg->following, user_free);

	free(cfg);
}
