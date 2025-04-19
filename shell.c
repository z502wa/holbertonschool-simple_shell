#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * main - entry point for simple_shell
 * Return: exit status
 */
int main(void)
{
	int interactive = isatty(STDIN_FILENO);
	char *line, **args;
	int status;

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
		if (!args[0])
		{
			free(args);
			free(line);
			continue;
		}

		status = execute(args);
		free(args);
		free(line);
		(void)status;
	}

	return (0);
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
		if (!access(full, X_OK))
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
 * Return: child exit status or -1
 */
int execute(char **args)
{
	pid_t pid;
	int st;
	char *path = strchr(args[0], '/') ? args[0] : find_path(args[0]);

	if (!path || (strchr(args[0], '/') && access(path, X_OK)))
	{
		fprintf(stderr, "%s: not found\n", args[0]);
		return (-1);
	}

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		if (path != args[0])
			free(path);
		return (-1);
	}
	if (!pid)
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
