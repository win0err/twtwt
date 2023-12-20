# twtwt — a twtxt CLI client

**twtwt** — a client for [twtxt](https://twtxt.readthedocs.io/en/latest/index.html): decentralized, minimalist microblogging service for hackers.

## Installation

### Dependencies

#### Utilities

`pkg-config`/`pkgconf` and GNU `make` are required for building using `Makefile`.

#### Libraries

`curl/curl.h` and `ini.h` headers are required for building twtwt from source.

##### Packages

- Debian/Ubuntu: `libinih1 libinih-dev libcurl4 libcurl4-openssl-dev`
- Fedora: `inih inih-devel libcurl libcurl-devel`
- macOS/BSD: `curl inih`

### Installation

```bash
make          # GNU make should be used
make install  # will install to /usr/local; may require superuser privileges
```

You can pass `PREFIX` or `DESTDIR` to `make`:

```bash
make PREFIX=/usr
make PREFIX=/usr install  # may require superuser privileges
```

### Uninstallation

```bash
doas make uninstall  # note: doas is modern sudo
```

### Testing

`check.h` are required for unit testing.

##### Packages

- Debian/Ubuntu/FreeBSD/macOS: `check`
- Fedora: `check check-devel`

```bash
make test
```


## Quickstart

At the first startup, the quickstart wizard will be started, that will help you to configure twtwt.

The quickstart wizard prompts for the following configuration values:

1. Your desired nickname
2. Location of your twtxt.txt file on the computer
3. The URL your twtxt.txt file will be accessible from remotely


## Usage

### Manage sources

Add a new source to your followings:
```bash
$ twtwt follow win0err https://kolesnikov.se/twtxt.txt
You're now following win0err (https://kolesnikov.se/twtxt.txt)
```

Show the list of sources you are following:
```bash
$ twtwt following
win0err @ https://kolesnikov.se/twtxt.txt
```

Remove an existing source from your followings:
```bash
$ twtwt unfollow win0err
You've unfollowed win0err (https://kolesnikov.se/twtxt.txt)
```

### Post a status update

Append a new tweet to your twtxt.txt file:

```bash
$ twtwt tweet "Hello, world! :-)"
```

If `pull_command` is set in the configuration file, it will be executed before tweeting. \
If `push_command` is set, it will be executed after tweeting.

#### Mentioning users

If you're following the user you want to mention, you can use short mentions that will be automatically expanded:
```bash
$ twtwt following
bob @ https://example.com/bob.txt

$ twtwt tweet "@bob: Have you tried twtwt?"

$ tail -n1 ~/twtxt.txt
2023-01-21T14:38:41+03:00	@<bob https://example.com/bob.txt>: Have you tried twtwt?
```

Otherwise, you have to write a full mention in the tweet:
```bash
$ twtwt tweet "@<alice https://example.com/alice.txt>: How often does it rain in St. Petersburg?"

$ tail -n1 ~/twtxt.txt
2023-01-21T14:39:15+03:00	@<alice https://example.com/alice.txt>: How often does it rain in St. Petersburg?
```

#### Manage twtxt.txt file

You can edit your twtxt.txt file running:
```bash
$ twtwt file edit
# opens $EDITOR (nano if the env. variable is unset)
```

You may want to sync txtxt.txt file after editing:

```bash
$ twtwt file pull  # executes pull_command, see Configuration section
scp twtwt@kolesnikov.se:/path/on/remote/server/twtxt.txt ~/twtxt.txt
twtxt.txt                               100% 6350    63.2KB/s   00:00

$ twtwt file push  # executes push_command, see Configuration section
scp ~/twtxt.txt twtwt@kolesnikov.se:/path/on/remote/server/twtxt.txt
twtxt.txt                               100% 6350   120.8KB/s   00:00
```

### Timeline and feeds

`twtwt timeline` and `twtwt view` commands are used to view your personal timeline or feed of a specific source.
Both commands support the `-l` (limit) and `-p` (page) flags.

For example, `twtwt timeline` will display the last 20 posts in your timeline.
To see the next (earlier) posts, use the `-p` flag: `twtwt timeline -p2`.

The `twtwt timeline -l5` command will show the last 5 posts in your feed.
To see the next (earlier) 5 posts: `twtwt timeline -l5 -p2`.

To see all available posts, set the limit to `0`: `twtwt view https://kolesnikov.se/twtxt.txt -l0`.

#### Retrieve your personal timeline

```bash
$ twtwt timeline
bob wrote on 21 Jan 2023, 14:42:
There will be an answer... Let it be... Let it be

alice wrote on 21 Jan 2023, 14:43:
@<bob https://example.com/bob.txt>: Let it B, let it B, let it C, let it D...
```

#### Show feed of a given source
```bash
$ twtwt view https://kolesnikov.se/twtxt.txt
https://kolesnikov.se/twtxt.txt wrote on 18 Mar 2022, 03:20:
Hello world!
...
```

If you are following the user whose feed you want to view, you can use nick instead of URL:
```bash
$ twtwt view win0err
win0err wrote on 18 Mar 2022, 03:20:
Hello world!
...
```

### Edit configuration file

```bash
$ twtwt config
# opens $EDITOR (nano if the variable is unset)
```

### Version and usage

To show program version, use: `twtwt -v`. \
To show available commands, use: `twtwt -h`.


## Configuration

Config can be edited using `twtwt config` command.

Locations of the configuration file:
- macOS: `~/Library/Application Support/twtwt/twtwt.conf`
- Linux/BSD: `~/.config/twtwt/twtwt.conf`

Example of a configuration file:
```ini
[twtxt]
nick = win0err
twtfile = ~/twtxt.txt
twturl = https://kolesnikov.se/twtxt.txt
pull_command = scp twtwt@kolesnikov.se:/path/on/remote/server/twtxt.txt ~/twtxt.txt
push_command = scp ~/twtxt.txt twtwt@kolesnikov.se:/path/on/remote/server/twtxt.txt

[following]
win0err = https://kolesnikov.se/twtxt.txt
```

### Options

`pull_command` and `push_command` are very useful if you want to push your twtxt file to a remote (web) server.

#### `[twtxt]` section

- `nick`: your nick, will be displayed in your timeline
- `twtfile`: path to your local twtxt file
- `twturl`: URL to your public twtxt file
- `pull_command`: command to be executed before tweeting and when running command `twtwt file pull`
- `push_command`: command to be executed after tweeting and when running command `twtwt file push`

#### `[following]` section

This section holds all your followings as nick and URL pairs. You can edit this section manually or use the `follow`/`unfollow` commands.

---
_Developed by [Sergei Kolesnikov](https://github.com/win0err)_
