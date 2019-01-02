#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/**
 * Reads the line from an input stream.
 *
 * @param fp: Input stream.
 * @return line: Line that was read.
 */
char* read_line(FILE* fp) {
	char *buffer;
	int	buffer_size = 0;
	int	cur_pos = 0;
	int	input_char;

	while((input_char = getc(fp)) != EOF) {
    // Resize if no space.
		if (buffer_size < cur_pos + 1) {
			if (buffer_size == 0) {
				buffer = emalloc(BUFSIZ);
      } else {
				buffer = erealloc(buffer, buffer_size + BUFSIZ);
      }
			buffer_size += BUFSIZ;
		}

    // End of line.
		if (input_char == '\n') {
			break;
    }

		buffer[cur_pos++] = input_char;
	}
  // Empty input.
	if (cur_pos == 0 && input_char == EOF)	{
		return NULL;
  }
	buffer[cur_pos] = '\0';
	return buffer;
}


/**
 * Splits the line into strings, white-space separated.
 *
 * @param line: NULL terminated string.
 * @return array of strings.
 */
char** splitline(char* line) {
	char **strings;
	int	spots = 0;
	int	buffer_size = 0;

	int	num_strings = 0;
	char* cur_string;
	int	length;

  char* ptr = line;

	if (line == NULL) {
		return NULL;
  }

	strings = emalloc(BUFSIZ);
	buffer_size = BUFSIZ;
	spots = BUFSIZ / sizeof(char*);

	while (*ptr != '\0') {
		while (is_delim(*ptr)) {
			ptr++;
    }

		if (*ptr == '\0') {
			break;
    }

    // Resize if needed.
		if (num_strings + 1 >= spots) {
			strings = erealloc(strings, buffer_size + BUFSIZ);
			buffer_size += BUFSIZ;
			spots += (BUFSIZ/sizeof(char *));
		}

    // Get the word.
		cur_string = ptr;
		length = 1;
		while (*++ptr != '\0' && !(is_delim(*ptr))) {
			length++;
    }
		strings[num_strings++] = newstr(cur_string, length);
	}
	strings[num_strings] = NULL;
	return strings;
}

/**
 * Copies the string to a new char*.
 *
 * @param str: source string.
 * @param length: the length of the string.
 * @return new string.
 */
char* newstr(char* str, int length) {
  // Allocate memory for the new string.
	char* new_string = emalloc(length + 1);

  // Null-terminate.
	new_string[length] = '\0';

  int i;
  for (i = 0; i < length; i++) {
    new_string[i] = str[i];
  }
	return new_string;
}

/**
 * Free the memory for an array of strings.
 *
 * @param list: array of strings.
 */
void freelist(char **list) {
	char** ptr = list;
	while (*ptr) {
		free(*ptr++);
  }
	free(list);
}

/**
 * Malloc with error handling.
 *
 * @param n: requested memory.
 * @return: pointer to the newly allocated memory.
 */
void* emalloc(size_t n) {
	void* new_mem;
	if ((new_mem = malloc(n)) == NULL) {
		error("Out of memory!");
  }
	return new_mem;
}
 
/**
 * Realloc with error handling.
 *
 * @param p: pointer to reallocate.
 * @param n: new size.
 * @return: new pointer to the new allocated memory.
 */
void* erealloc(void *p, size_t n) {
	void *rv;
	if ((rv = realloc(p,n)) == NULL) {
		error("Realloc() failed.");
  }
	return rv;
}

/**
 * Prints out an error message to stder and exits the program with code 1.
 *
 * @param msg: message to print before exiting.
 */
void error(char* msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}
