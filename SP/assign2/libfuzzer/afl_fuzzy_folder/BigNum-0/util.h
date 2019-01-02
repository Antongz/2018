#ifndef __UTIL_H__
#define __UTIL_H__ 1

// Macro to check if a char is a delimiter.
#define	is_delim(x) ((x)==' '||(x)=='\t')

// Prints an error message and exits.
void error(char* msg1);

char* read_line(FILE* fp);
char** splitline(char* line);

char* newstr(char *str, int length);

void* emalloc(size_t n);
void* erealloc(void *p, size_t n);

#endif // __UTIL_H__
