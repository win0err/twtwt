#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bold.h"
#include "list.h"
#include "tweet.h"
#include "rfc3339.h"

#define LIMIT 20

void twtxt_display_tweets(list_t * tweets)
{
	if (tweets == NULL)
		return;

	list_node_t *current;
	tweet_t *tweet;

	// TODO: add: int limit, int page, enum timeline_ordering ordering
	int start_from = tweets->count - LIMIT;
	int i = 0;

	list_for_each(tweets, current) {
		tweet = (tweet_t *) current->data;

		if (i >= start_from) {
			// TODO: Pretty-print date

			char *datetime = localtime_to_rfc3339_local(localtime(&tweet->time));

			printf(BOLD("%s") " (%s):\n%s\n\n", tweet->user->nick, datetime, tweet->text);

			free(datetime);
		}
		i++;
	}
}

void tweet_free(void *node_data)
{
	if (node_data == NULL)
		return;

	tweet_t *t = (tweet_t *) node_data;

	if (t->text != NULL) {
		free((void *) t->text);
		t->text = NULL;
	}
	// t->user will be freed in other places:
	// e.g. config->following or command

	t->time = 0;
}
