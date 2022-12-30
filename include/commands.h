#ifndef _TWTWT_COMMANDS_H
#define _TWTWT_COMMANDS_H

#include "config.h"
#include "list.h"

#define COMMAND_CONFIG "config"
#define COMMAND_TWEET "tweet"
#define COMMAND_TIMELINE "timeline"
#define COMMAND_FOLLOW "follow"
#define COMMAND_UNFOLLOW "unfollow"
#define COMMAND_FOLLOWING "following"
#define COMMAND_VIEW "view"
#define COMMAND_FILE "file"

#ifndef DEFAULT_EDITOR
#define DEFAULT_EDITOR "nano"
#endif

int command_config();
int command_following(list_t * following);
int command_follow(const char *nick, const char *url, config_t * config);
int command_unfollow(const char *nick, config_t * config);
int command_timeline(config_t * config, int page, int limit);
int command_view(const char *url_or_nick, config_t * config, int page, int limit);
int command_tweet(const char *tweet, config_t * config);
int command_file(const char *subcommand, config_t * config);

#endif
