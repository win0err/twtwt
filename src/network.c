#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "config.h"
#include "list.h"
#include "network.h"
#include "rfc3339.h"
#include "tweet.h"
#include "twtwt.h"

#define PLAIN_TEXT_PREFIX "text/plain"
#define USERAGENT_FORMAT "%s/%s (+%s; @%s)"
#define DELIM "\n"
#define MAX_PARALLEL 10
#define TIMEOUT_MS 5000

extern char *strdup(const char *);

static char *useragent = NULL;

static void set_useragent(config_t *config)
{
	size_t len =
		strlen(USERAGENT_FORMAT) + strlen(PROGNAME VERSION) + strlen(config->twturl) + strlen(config->nick) + 1;
	useragent = calloc(len, sizeof(char));

	snprintf(useragent, len, USERAGENT_FORMAT, PROGNAME, VERSION, config->twturl, config->nick);
}

static list_t *parse_twtxt_content(struct twtxt_contents *twtxt)
{
	char *line = strtok(twtxt->data, DELIM);
	list_t *tweets = list_new();

	while (line != NULL) {
		char *datetime = (char *) calloc(strlen(line) + 1, sizeof(char));
		char *text = (char *) calloc(strlen(line) + 1, sizeof(char));

		if (isdigit(line[0]) > 0 && sscanf(line, "%s\t%[^\n]$", datetime, text) == 2) {
			tweet_t *tweet = (tweet_t *) malloc(sizeof(tweet_t));

			text = realloc(text, strlen(text) + 1);

			tweet->user = twtxt->user;
			tweet->text = strdup(text);
			tweet->time = rfc3339_to_time_t(datetime);

			list_push(tweets, list_node_new(tweet));
		}

		free(text);
		free(datetime);

		line = strtok(NULL, DELIM);
	}

	return tweets;
}

static size_t write_callback(char *contents, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;

	struct twtxt_contents *twtxt = (struct twtxt_contents *) userdata;

	char *ptr = realloc(twtxt->data, twtxt->size + realsize + 1);

	assert(ptr != NULL);

	twtxt->data = ptr;
	memcpy(&(twtxt->data[twtxt->size]), contents, realsize);
	twtxt->size += realsize;
	twtxt->data[twtxt->size] = 0;

	return realsize;
}

static CURL *get_handle_from_content(struct twtxt_contents *content, int private_data)
{
	CURL *handle = curl_easy_init();

	curl_easy_setopt(handle, CURLOPT_URL, content->user->url);
	curl_easy_setopt(handle, CURLOPT_PRIVATE, private_data);
	curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, TIMEOUT_MS);
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *) content);
	curl_easy_setopt(handle, CURLOPT_USERAGENT, useragent);
	curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1);

	return handle;
}

static list_t *process_finished_request(CURL *handle, CURLcode result, struct twtxt_contents *twtxt)
{
	long http_code;

	switch (result) {
	case CURLE_OK:
		return parse_twtxt_content(twtxt);

	case CURLE_HTTP_RETURNED_ERROR:
		curl_easy_getinfo(handle, CURLINFO_HTTP_CODE, &http_code);
		fprintf(stderr, "error: %s: status code: %ld\n", twtxt->user->url, http_code);

		break;
	case CURLE_UNSUPPORTED_PROTOCOL:
		fprintf(stderr, "error: %s: protocol is not supported.\nuse http(s), ftp(s), gopher, file, etc.\n",
				twtxt->user->url);

		break;
	default:
		fprintf(stderr, "error: %s: %s\n", twtxt->user->url, curl_easy_strerror(result));
	}

	return NULL;
}

static struct twtxt_contents *users_list_to_contents(list_t *users)
{
	struct twtxt_contents *contents = calloc(users->count, sizeof(struct twtxt_contents));

	list_node_t *node;
	int i = 0;

	list_for_each(users, node) {
		contents[i].data = malloc(1);
		contents[i].size = 0;
		contents[i].user = (user_t *) node->data;

		i++;
	}

	return contents;
}

list_t *fetch_user_tweets(user_t *user, config_t *cfg)
{
	struct twtxt_contents content = {
		.data = malloc(1),		// will be grown
		.size = 0,
		.user = user
	};

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *handle = get_handle_from_content(&content, 0);
	CURLcode result = curl_easy_perform(handle);

	list_t *tweets = process_finished_request(handle, result, &content);

	curl_easy_cleanup(handle);
	curl_global_cleanup();

	free(content.data);

	return tweets;
}

list_t **fetch_timeline_tweets(config_t *config, int *parsed_count)
{
	if (useragent == NULL) {
		set_useragent(config);
	}

	CURLM *multi_handle;
	CURLMsg *msg;

	int urls_count = config->following->count;

	int transfer_id = 0, msgs_left = -1, transfers_left = 0;

	*parsed_count = 0;

	struct twtxt_contents *contents = users_list_to_contents(config->following);
	list_t **tweets = calloc(urls_count, sizeof(list_t *));

	curl_global_init(CURL_GLOBAL_ALL);

	multi_handle = curl_multi_init();
	curl_multi_setopt(multi_handle, CURLMOPT_MAXCONNECTS, (long) MAX_PARALLEL);

	for (transfer_id = 0; transfer_id < MAX_PARALLEL && transfer_id < urls_count; transfer_id++) {
		curl_multi_add_handle(multi_handle, get_handle_from_content(&(contents[transfer_id]), transfer_id));
		transfers_left++;
	}

	do {
		int running_handles = 1;

		curl_multi_perform(multi_handle, &running_handles);

		while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
			if (msg->msg == CURLMSG_DONE) {
				int finished_id;

				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &finished_id);

				list_t *tweets_from_url =
					process_finished_request(msg->easy_handle, msg->data.result, &contents[finished_id]);
				if (tweets_from_url != NULL) {
					tweets[*parsed_count] = tweets_from_url;
					(*parsed_count)++;
				}

				curl_multi_remove_handle(multi_handle, msg->easy_handle);
				curl_easy_cleanup(msg->easy_handle);

				transfers_left--;
			} else {
				fprintf(stderr, "error: CURLMsg (%d)\n", msg->msg);
			}

			if (transfer_id < urls_count) {
				curl_multi_add_handle(multi_handle, get_handle_from_content(&(contents[transfer_id]), transfer_id));
				transfer_id++;
				transfers_left++;
			}
		}

		if (transfers_left > 0)
			curl_multi_wait(multi_handle, NULL, 0, 1000, NULL);

	} while (transfers_left > 0);

	curl_multi_cleanup(multi_handle);
	curl_global_cleanup();

	tweets = realloc(tweets, (*parsed_count) * sizeof(list_t *));

	return tweets;
}
