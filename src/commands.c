#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bold.h"
#include "commands.h"
#include "config.h"
#include "list.h"
#include "network.h"
#include "rfc3339.h"
#include "tweet.h"
#include "utils.h"

extern char *strdup(const char *);

static long tweets_comparator_asc(void *node1_data, void *node2_data)
{
	return ((tweet_t *) node2_data)->time - ((tweet_t *) node1_data)->time;
}

int command_config()
{
	char *config_location = get_config_location();

	char *editor = getenv("EDITOR");

	if (editor == NULL) {
		editor = DEFAULT_EDITOR;
	}

	char *args[] = { editor, config_location, NULL };

	if (execvp(editor, args) == -1) {
		fprintf(stderr, "error: cannot run editor %s %s\n", editor, config_location);

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int command_following(list_t * following)
{
	list_node_t *node = NULL;
	user_t *user = NULL;

	// TODO: show an information about user, e.g.
	// HTTP code, size & last update date

	list_for_each(following, node) {
		user = (user_t *) node->data;
		printf(BOLD("%s") " @ %s\n", user->nick, user->url);
	}

	return EXIT_SUCCESS;
}

int command_follow(const char *nick, const char *url, config_t * config)
{
	user_t *new_user = (user_t *) malloc(sizeof(user_t));

	new_user->nick = strdup(nick);
	new_user->url = strdup(url);

	list_node_t *node = NULL;
	user_t *user = NULL;

	list_for_each(config->following, node) {
		user = (user_t *) node->data;

		if (!strcmp(user->nick, new_user->nick)) {
			printf("You're already following " BOLD("%s") " (%s)\n", new_user->nick, new_user->url);

			user_free((void *) new_user);
			free((void *) new_user);

			// TODO: check if urls are different (updated)

			return EXIT_FAILURE;
		}
	}

	list_push(config->following, list_node_new(new_user));

	if (config_save(config) != EXIT_SUCCESS)
		return EXIT_FAILURE;

	printf("You're now following " BOLD("%s") " (%s)\n", new_user->nick, new_user->url);

	return EXIT_SUCCESS;
}

int command_unfollow(const char *nick, config_t * config)
{
	list_node_t *node, *unfollowed = NULL;
	user_t *user = NULL;

	list_for_each(config->following, node) {
		user = (user_t *) node->data;

		if (!strcmp(user->nick, nick)) {
			unfollowed = list_node_detach(config->following, node);
		}
	}

	if (unfollowed == NULL) {
		printf("You're not following " BOLD("%s") "\n", nick);

		return EXIT_FAILURE;
	}

	user = (user_t *) unfollowed->data;
	printf("You've unfollowed " BOLD("%s") " (%s)\n", user->nick, user->url);

	list_node_free(unfollowed, user_free);

	return config_save(config);
}

int command_timeline(config_t * config)
{
	int fetched_count;

	// TODO: retcodes & memory leaks
	list_t **tweets = fetch_timeline_tweets(config, &fetched_count);
	list_t *timeline = list_merge_k_sorted(fetched_count, tweets, tweets_comparator_asc);

	if (timeline != NULL) {
		twtxt_display_tweets(timeline);
		list_free(timeline, tweet_free);
	}

	if (tweets != NULL) {
		free(tweets);
	}

	return EXIT_SUCCESS;
}

int command_view(const char *url_or_nick, config_t * config)
{
	user_t *found_user = NULL;
	int is_user_found = 0;

	list_node_t *node;
	user_t *user;

	list_for_each(config->following, node) {
		user = (user_t *) node->data;

		if (!strcmp(user->nick, url_or_nick)) {
			found_user = user;
			is_user_found = 1;
			break;
		}
	}

	if (!is_user_found) {
		found_user = (user_t *) malloc(sizeof(user_t));
		found_user->nick = strdup(url_or_nick);
		found_user->url = strdup(url_or_nick);
	}
	// TODO: retcodes
	list_t *tweets = fetch_user_tweets(found_user, config);

	if (tweets != NULL) {
		twtxt_display_tweets(tweets);
		list_free(tweets, tweet_free);
	}

	if (!is_user_found) {
		user_free((void *) found_user);
		free((void *) found_user);
	}

	return EXIT_SUCCESS;
}

int command_tweet(const char *tweet, config_t * config)
{
	// TODO: need to expand mentions, e.g.:
	// @win0err -> @<win0err https://kolesnikov.se/twtxt.txt>

	FILE *fp = ensure_fopen(config->twtfile, "a");

	if (fp == NULL) {
		fprintf(stderr, "error: cannot open twtxt file\n");

		return EXIT_FAILURE;
	}

	int hook_retcode = 0;

	if (config->pre_tweet_hook != NULL) {
		hook_retcode = system(config->pre_tweet_hook);

		if (hook_retcode != 0) {
			fprintf(stderr, "error: failed to run pre_tweet_hook, return code: %d\n", hook_retcode);
			fclose(fp);

			return EXIT_FAILURE;
		}
	}

	time_t now = time(NULL);

	char *formatted_time = localtime_to_rfc3339_local(localtime(&now));

	fprintf(fp, "%s\t%s\n", formatted_time, tweet);

	free(formatted_time);

	fclose(fp);

	if (config->post_tweet_hook != NULL) {
		hook_retcode = system(config->post_tweet_hook);

		if (hook_retcode != 0) {
			fprintf(stderr, "error: failed to run post_tweet_hook, return code: %d\n", hook_retcode);

			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
