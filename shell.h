#ifndef SHELL_H
#define SHELL_H

/* external environment variable */
extern char **environ;

/* read a line from stdin */
char *read_line(void);

/* split line into array of args (trims newline) */
char **split_line(char *line);

/* search for cmd in PATH, return full path or NULL */
char *find_path(char *cmd);

/* fork and exec the given command */
int execute(char **args);

#endif /* SHELL_H */
