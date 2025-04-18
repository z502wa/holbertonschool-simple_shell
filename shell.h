#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

/* External environment variable */
extern char **environ;

/* Read a line from stdin */
char *read_line(void);

/* Remove trailing newline from string */
char *trim_newline(char *str);

/* Split line into array of arguments */
char **split_line(char *line);

/* Fork and execute the parsed command */
int execute(char **args);

#endif /* SHELL_H */
