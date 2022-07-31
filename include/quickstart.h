#ifndef _TWTWT_QUICKSTART_H
#define _TWTWT_QUICKSTART_H

#include "config.h"
#include "list.h"

#define TWTXT_FILE_CONTENTS_TEMPLATE                       \
    "#  __             __          __    \n"               \
    "# |  |_.--.--.--.|  |_.--.--.|  |_  \n"               \
    "# |   _|  |  |  ||   _|_   _||   _| \n"               \
    "# |____|________||____|__.__||____| \n"               \
    "# \n"                                                 \
    "# twtxt is an open, distributed microblogging \n"     \
    "# platform that uses human-readable text files, \n"   \
    "# common transport protocols, and free software. \n"  \
    "# \n"                                                 \
    "# Learn more: https://github.com/buckket/twtxt \n"    \
    "# \n"                                                 \
    "# twtwt - a client for twtxt: \n"                     \
    "#   https://github.com/win0err/twtwt \n"              \
    "# \n"                                                 \
    "# --------------------------- \n"                     \
    "# \n"                                                 \
    "# nick = %s \n"                                       \
    "# url = %s \n"                                        \
    "# \n"                                                 \
    "# $ twtwt follow %s %s \n"                            \
    "# \n"                                                 \
    "\n"

int run_quickstart();

#endif
