#ifndef _TWTWT_NETWORK_H
#define _TWTWT_NETWORK_H

#include "config.h"
#include "tweet.h"

struct twtxt_contents {
	char *data;
	size_t size;
	user_t *user;
};

list_t *fetch_user_tweets(user_t * user, config_t * cfg);
list_t **fetch_timeline_tweets(config_t * config, int *parsed_count);

#endif
