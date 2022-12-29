#include <assert.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bold.h"
#include "list.h"
#include "tweet.h"

#define NICK_RE "[A-Za-z0-9_-]+"
#define MENTION_RE "@(" NICK_RE ")"
#define EXTENDED_MENTION_RE "@<(" NICK_RE ") ([^>]+)>"

#define TW_BUFSIZE 512
#define MIN(a, b) ((a < b) ? a : b)

char *mentions_shrink(const char *src, list_t * following)
{
	char *text = calloc(strlen(src) + 1, sizeof(char));

	strcpy(text, src);

	int rc;

	regex_t preg;

	rc = regcomp(&preg, EXTENDED_MENTION_RE, REG_EXTENDED | REG_ICASE);
	assert(rc == 0);

	regmatch_t *matches = calloc(preg.re_nsub + 1, sizeof(regmatch_t));

	int off = 0;

	while (0 == (rc = regexec(&preg, text + off, preg.re_nsub + 1, matches, REG_NOTBOL))) {
		unsigned long tweet_len = strlen(text);

		int mention_start_off = matches[0].rm_so + off;
		int mention_end_off = matches[0].rm_eo + off;

		int opening_brace_off = mention_start_off + 1;

		int nick_start_off = matches[1].rm_so + off;
		int nick_end_off = matches[1].rm_eo + off;
		int nick_len = nick_end_off - nick_start_off;

		char *nick = calloc(nick_len + 1, sizeof(char));

		strncpy(nick, text + nick_start_off, nick_len);

		list_node_t *node = NULL;
		user_t *user = NULL;
		user_t *found_user = NULL;

		list_for_each(following, node) {
			user = (user_t *) node->data;

			if (!strcmp(user->nick, nick)) {
				found_user = user;
			}
		}

		free(nick);

		// do not collapse if not following the user
		if (found_user != NULL) {
			// "@<win0err https://kolesnikov.se/twtxt.txt>..." -> "@win0err https://kolesnikov.se/twtxt.txt>..."
			memmove(text + opening_brace_off, text + nick_start_off, nick_len);

			// "@win0err https://kolesnikov.se/twtxt.txt>..." -> "@win0err..."
			memmove(text + opening_brace_off + nick_len, text + mention_end_off, tweet_len - mention_end_off + 1);

			off = opening_brace_off + nick_len;
		} else {
			off = mention_end_off;
		}
	}

	text = realloc(text, strlen(text) + 1);

	regfree(&preg);

	return text;
}

char *mentions_expand(const char *src, list_t * following)
{
	char *text = calloc(strlen(src) + 1, sizeof(char));

	strcpy(text, src);

	int rc;

	regex_t preg;

	rc = regcomp(&preg, MENTION_RE, REG_EXTENDED | REG_ICASE);
	assert(rc == 0);

	regmatch_t *matches = calloc(preg.re_nsub + 1, sizeof(regmatch_t));

	int off = 0;

	while (0 == (rc = regexec(&preg, text + off, preg.re_nsub + 1, matches, REG_NOTBOL))) {
		int mention_end_off = matches[0].rm_eo + off;

		int nick_start_off = matches[1].rm_so + off;
		int nick_end_off = matches[1].rm_eo + off;
		int nick_len = nick_end_off - nick_start_off;

		char *nick = calloc(nick_len + 1, sizeof(char));

		strncpy(nick, text + nick_start_off, nick_len);

		list_node_t *node = NULL;
		user_t *user = NULL;
		user_t *found_user = NULL;

		list_for_each(following, node) {
			user = (user_t *) node->data;

			if (!strcmp(user->nick, nick)) {
				found_user = user;
			}
		}

		free(nick);

		// do not expand if not following the user
		if (found_user != NULL) {
			unsigned long tweet_len = strlen(text);
			unsigned long diff_len = strlen(found_user->url) + strlen("< >");

			text = realloc(text, tweet_len + diff_len + 1);

			// add space for extended mention: "<... URL>"
			memmove(text + nick_end_off + diff_len, text + nick_end_off, tweet_len - nick_end_off);

			// "@win0err..." -> "@ win0err..."
			memmove(text + nick_start_off + 1, text + nick_start_off, nick_len);

			// "@ win0err..." -> "@<win0err ..."
			text[nick_start_off] = '<';
			text[nick_end_off + 1] = ' ';

			// "@<win0err ..." -> "@<win0err https://kolesnikov.se/twtxt.txt>..."
			memcpy(text + nick_end_off + 2, found_user->url, strlen(found_user->url));
			text[nick_end_off + diff_len - 1] = '>';

			off = nick_end_off + diff_len - 1;
		} else {
			off = mention_end_off;
		}
	}

	regfree(&preg);

	return text;
}

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

int twtxt_display_tweets(list_t * tweets, list_t * following, int page, int limit)
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

		char *text = mentions_shrink(tweet->text, following);

		printf(BOLD("%s") " wrote on %s:\n%s\n\n", tweet->user->nick, pretty_date, text);

		free(text);
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
