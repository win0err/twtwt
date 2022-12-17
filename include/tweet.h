#ifndef _TWTWT_TWEET_H
#define _TWTWT_TWEET_H

#include <stddef.h>
#include <time.h>

#include "list.h"

enum timeline_ordering {
	Ascending = 1,
	Descending = -1
};

typedef struct user {
	char *nick;
	char *url;
} user_t;

typedef struct tweet {
	user_t *user;
	char *text;
	time_t time;
} tweet_t;

int twtxt_display_tweets(list_t * tweets, int page, int limit);
void tweet_free(void *node_data);

#endif
