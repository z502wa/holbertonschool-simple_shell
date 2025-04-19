#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static char *shell_name;
static int line_count;

/**
 * main - entry point for simple_shell
 * @argc: arg count
 * @argv: arg vector
 * Return: exit status
 */
int main(int argc, char **argv)
{
	int interactive = isatty(STDIN_FILENO);
	char *line, **args;
	int status = 0;

	shell_name = argv[0];
	line_count = 0;

	while (1)
	{
		if (interactive)
			if (write(STDOUT_FILENO, "$ ", 2) < 0)
				exit(EXIT_FAILURE);

		line = read_line();
		if (!line)
		{
			if (interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		args = split_line(line);
		free(line);
		if (!args[0])
		{
			free(args);
			continue;
		}

		line_count++;
		status = execute(args);
		free(args);
	}

	return (interactive ? 0 : status);
}

/**
 * read_line - read input line
 * Return: buffer or NULL on EOF
 */
char *read_line(void)
{
	char *buf = NULL;
	size_t sz = 0;
	ssize_t r = getline(&buf, &sz, stdin);

	if (r < 0)
	{
		free(buf);
		return (NULL);
	}
	return (buf);
}

/**
 * split_line - split line into tokens by whitespace
 * @line: input line
 * Return: NULL-terminated array
 */
char **split_line(char *line)
{
	size_t bs = 64, pos = 0;
	char **tok = malloc(bs * sizeof(char *));
	char *t;

	if (!tok)
		exit(EXIT_FAILURE);

	t = strtok(line, " \t\n");
	while (t)
	{
		tok[pos++] = t;
		if (pos >= bs)
		{
			bs *= 2;
			tok = realloc(tok, bs * sizeof(char *));
			if (!tok)
				exit(EXIT_FAILURE);
		}
		t = strtok(NULL, " \t\n");
	}
	tok[pos] = NULL;
	return (tok);
}

/**
 * find_path - locate cmd in PATH without getenv
 * @cmd: program name
 * Return: full path or NULL
 */
char *find_path(char *cmd)
{
	int i = 0;
	char *p = NULL, *dup, *dir, *full;
	size_t len;

	while (environ[i])
	{
		if (!strncmp(environ[i], "PATH=", 5))
		{
			p = environ[i] + 5;
			break;
		}
		i++;
	}
	if (!p)
		return (NULL);

	dup = strdup(p);
	if (!dup)
		exit(EXIT_FAILURE);

	dir = strtok(dup, ":");
	while (dir)
	{
		len = strlen(dir) + strlen(cmd) + 2;
		full = malloc(len);
		if (!full)
			exit(EXIT_FAILURE);
		strcpy(full, dir);
		strcat(full, "/");
		strcat(full, cmd);
		if (access(full, X_OK) == 0)
		{
			free(dup);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}
	free(dup);
	return (NULL);
}

/**
 * execute - run command via fork and execve
 * @args: args array
 * Return: child exit status or 127 if not found
 */
int execute(char **args)
{
	pid_t pid;
	int st;
	char *path;

	if (strchr(args[0], '/'))
	{
		path = args[0];
		if (access(path, X_OK) != 0)
		{
			fprintf(stderr, "%s: %d: %s: not found\n",
				shell_name, line_count, args[0]);
			return (127);
		}
	}
	else
	{
		path = find_path(args[0]);
		if (!path)
		{
			fprintf(stderr, "%s: %d: %s: not found\n",
				shell_name, line_count, args[0]);
			return (127);
		}
	}

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		if (path != args[0])
			free(path);
		return (-1);
	}
	if (pid == 0)
	{
		execve(path, args, environ);
		perror(path);
		exit(EXIT_FAILURE);
	}
	waitpid(pid, &st, 0);
	if (path != args[0])
		free(path);
	return (WIFEXITED(st) ? WEXITSTATUS(st) : st);
}
