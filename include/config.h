#ifndef _TWTWT_CONFIG_H
#define _TWTWT_CONFIG_H

#include "list.h"
#include "twtwt.h"

#define CONFIG_LOCATION "/" PROGNAME "/twtwt.conf"

#define LOAD_CONFIG_OK 0
#define LOAD_CONFIG_READ_ERROR -1
#define LOAD_CONFIG_MEMORY_ERROR -2

typedef struct {
	char *nick;
	char *twtfile;
	char *twturl;
	char *pre_tweet_hook;
	char *post_tweet_hook;
	list_t *following;
} config_t;

config_t *config_new();
void config_free(config_t * cfg);

int config_load(config_t * cfg);
int config_save(config_t * cfg);

void user_free(void *node_data);

#endif
