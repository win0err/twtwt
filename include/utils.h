#ifndef _TWTWT_UTILS_H
#define _TWTWT_UTILS_H

char *trim(char *str);
int is_file_exists(const char *path);
FILE *ensure_fopen(const char *filename, const char *mode);
char *expand_homedir(const char *filename);

#endif
