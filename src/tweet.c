#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "bold.h"
#include "list.h"
#include "tweet.h"

#define TW_BUFSIZE 512
#define MIN(a, b) ((a < b) ? a : b)

static void display_pagination_info(int current_page, int total_pages, int start, int end, int total)
{
	start += 1;

	printf("page: %d of %d", current_page, total_pages);

	printf(", ");

	printf("tweets: ");
	if (start != end)
		printf("%d-", start);

	printf("%d of %d", end, total);
}

int twtxt_display_tweets(list_t * tweets, int page, int limit)
{
	if (tweets == NULL)
		return EXIT_FAILURE;

	if (tweets->count == 0)
		return EXIT_SUCCESS;

	int total = (int) tweets->count;

	list_node_t *current = tweets->head;
	tweet_t *tweet;

	if (limit == 0)
		limit = total;

	// TODO: add: enum timeline_ordering ordering
	int total_pages = (total + limit - 1) / limit;

	if (page > total_pages)
		return EXIT_FAILURE;

	int from_id = total - limit * page;
	int to_id = MIN(from_id + limit, total);

	if (from_id < 0)
		from_id = 0;

	int i = 0;

	for (; i < from_id; i++, current = current->next)
		/* rewind the list */ ;

	for (; i < to_id; i++, current = current->next) {
		tweet = (tweet_t *) current->data;

		char pretty_date[TW_BUFSIZE];

		strftime(pretty_date, TW_BUFSIZE, "%d %b %Y, %H:%M", localtime(&tweet->time));

		printf(BOLD("%s") " wrote on %s:\n%s\n\n", tweet->user->nick, pretty_date, tweet->text);
	}

	// TODO: design how it will look like
	if (0)
		display_pagination_info(page, total_pages, from_id, to_id, total);

	return EXIT_SUCCESS;
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
