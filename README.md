# twtwt — a twtxt CLI client

**twtwt** — a client for [twtxt](https://twtxt.readthedocs.io/en/latest/index.html): decentralized, minimalist microblogging service for hackers.

## Installation

### Dependencies

`curl/curl.h` and `ini.h` headers are required for building twtwt from source.

- Arch: `pacman -S curl libinih`
- Debian/Ubuntu: `apt install libinih1 libinih-dev libcurl4 libcurl4-openssl-dev`
- Fedora: `dnf install inih inih-devel libcurl libcurl-devel`
- FreeBSD: `pkg install curl inih`
- macOS: `brew install curl inih`

### Installation

```bash
make
make install  # will install to /usr/local; may require superuser privileges
```

You can pass `PREFIX` or `DESTDIR` to `make`:

```bash
make PREFIX=/usr
make PREFIX=/usr install  # may require superuser privileges
```

### Uninstallation

```bash
sudo make uninstall
```

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
