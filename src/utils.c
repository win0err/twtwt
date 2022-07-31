#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"

#define	PATH_MAX 1024			// from xnu's sys/syslimits.h
#define MODE  0755

extern char *strdup(const char *);

char *trim(char *str)
{
	size_t beg = strspn(str, " \t\n");
	size_t len = strlen(str);

	// if all string contains whitespaces only
	if (beg == len) {
		*str = '\0';
		return str;
	}

	memmove(str, str + beg, len - beg + 1);
	for (int i = (int)(len - beg - 1); i >= 0; i--) {
		if (!isspace(str[i]))
			break;

		str[i] = '\0';
	}

	return str;
}

static int mkdir_p(const char *path)
{
	char *path_to_split = strdup(path);
	struct stat st;
	int rc;

	char current_path[PATH_MAX] = "";
	char *dir = strtok(path_to_split, "/");

	if (path_to_split[0] == '/')
		strcat(current_path, "/");

	while (dir != NULL) {
		if (strlen(current_path) + 1 + strlen(dir) + 1 > PATH_MAX) {
			fprintf(stderr, "error: specified path is too long\n");
			free(path_to_split);

			return EXIT_FAILURE;
		}

		strcat(current_path, dir);
		strcat(current_path, "/");

		if (stat(current_path, &st) != 0) {
			rc = mkdir(current_path, MODE);

			if (rc != 0) {
				fprintf(stderr, "error: cannot create directory `%s`: %s\n", current_path, strerror(errno));
				free(path_to_split);

				return EXIT_FAILURE;
			}
		}

		dir = strtok(NULL, "/");
	}

	free(path_to_split);

	return EXIT_SUCCESS;
}

int is_file_exists(const char *path)
{
	return access(path, F_OK) == 0;
}

char *expand_homedir(const char *filename)
{
	if (filename[0] != '~')
		return strdup(filename);

	char *dir_src = strdup(filename);
	char *dir = dirname(dir_src);

	char *file_src = strdup(filename);
	char *file = basename(file_src);

	dir += 1;					// skip ~ char

	char *home = getenv("HOME");
	char *expanded_dir = calloc(strlen(home) + strlen(dir) + 1, sizeof(char));
	char *ptr = expanded_dir;

	strcpy(ptr, home);

	ptr += strlen(home);
	while ((ptr - 1)[0] == '/')
		ptr--;

	strcpy(ptr, dir);
	dir = expanded_dir;

	size_t len = strlen(dir) + strlen(file) + 2;
	char *expanded_path = calloc(len, sizeof(char));

	snprintf(expanded_path, len, "%s/%s", dir, file);

	free(file_src);
	free(dir_src);

	return expanded_path;
}

FILE *ensure_fopen(const char *filename, const char *mode)
{
	char *path = expand_homedir(filename);
	char *dir_src = strdup(path);
	char *dir = dirname(dir_src);

	int rc = mkdir_p(dir);

	free(dir_src);

	if (rc != 0)
		return NULL;

	FILE *fp = fopen(path, mode);

	if (fp == NULL)
		fprintf(stderr, "error: cannot open file `%s`: %s\n", path, strerror(errno));

	free(path);

	return fp;
}
