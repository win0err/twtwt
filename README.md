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

---
_Developed by [Sergei Kolesnikov](https://github.com/win0err)_
