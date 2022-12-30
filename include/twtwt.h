#ifndef _TWTWT_TWTWT_H
#define _TWTWT_TWTWT_H

#include <stdio.h>

#define PROGNAME "twtwt"

#ifndef VERSION
#define VERSION "0.0.1"
#endif

#define PRINT_HEADER() puts(                                 \
        " __             __             __    \n"            \
        "|  |_.--.--.--.|  |_.--.--.--.|  |_  \n"            \
        "|   _|  |  |  ||   _|  |  |  ||   _| \n"            \
        "|____|________||____|________||____| \n"            \
        "\n"                                                 \
        PROGNAME " - a client for twtxt: decentralized, \n"  \
        "minimalist microblogging service for hackers."      \
    );

#define PRINT_COMMANDS() puts(                                             \
        "Usage: " PROGNAME " [options] command \n"                         \
        "\n"                                                               \
        "Commands: \n"                                                     \
        "  config    - edit " PROGNAME " config file \n"                   \
        "  file      - edit, pull or push twtxt.txt file \n"               \
        "  follow    - add a new source to your followings \n"             \
        "  following - show the list of sources you are following \n"      \
        "  timeline  - retrieve your personal timeline \n"                 \
        "  tweet     - append a new tweet to your twtxt.txt file \n"       \
        "  unfollow  - remove an existing source from your followings \n"  \
        "  view      - show feed of given source"                          \
    );

#endif
