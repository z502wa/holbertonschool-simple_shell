#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * main - entry point for simple_shell
 * Return: 0 on success, or exit on failure
 */
int main(void)
{
	int interactive = isatty(STDIN_FILENO);
	char *line, **args;
	int status;

	while (1)
	{
		/* display prompt only in interactive mode */
		if (interactive)
			if (write(STDOUT_FILENO, "$ ", 2) == -1)
				exit(EXIT_FAILURE);

		line = read_line();
		if (line == NULL)
		{
			/* handle EOF (Ctrl+D) */
			if (interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		args = split_line(line);
		if (args[0] == NULL)
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
 * read_line - read a line of input using getline
 * Return: pointer to buffer (must be freed), or NULL on EOF
 */
char *read_line(void)
{
	char *buffer = NULL;
	size_t bufsize = 0;
	ssize_t len;

	len = getline(&buffer, &bufsize, stdin);
	if (len == -1)
	{
		free(buffer);
		return (NULL);
	}

	return (buffer);
}

/**
 * split_line - trim newline and split into tokens
 * @line: input string
 * Return: NULL-terminated array of pointers to tokens
 */
char **split_line(char *line)
{
	size_t bufsize = 64, pos = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;
	size_t len;

	if (tokens == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	/* trim trailing newline */
	len = strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';

	token = strtok(line, " \t");
	while (token)
	{
		tokens[pos++] = token;
		if (pos >= bufsize)
		{
			bufsize *= 2;
			tokens = realloc(tokens, bufsize * sizeof(char *));
			if (tokens == NULL)
			{
				perror("realloc");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, " \t");
	}
	tokens[pos] = NULL;
	return (tokens);
}

/**
 * find_path - search for cmd in PATH environment
 * @cmd: command name (no slashes)
 * Return: full path (malloc'd) or NULL if not found
 */
char *find_path(char *cmd)
{
	char *path_env, *path_dup, *dir, *full;
	size_t len;

	path_env = getenv("PATH");
	if (path_env == NULL)
		return (NULL);

	path_dup = strdup(path_env);
	if (path_dup == NULL)
		exit(EXIT_FAILURE);

	dir = strtok(path_dup, ":");
	while (dir)
	{
		len = strlen(dir) + 1 + strlen(cmd) + 1;
		full = malloc(len);
		if (full == NULL)
			exit(EXIT_FAILURE);
		strcpy(full, dir);
		strcat(full, "/");
		strcat(full, cmd);
		if (access(full, X_OK) == 0)
		{
			free(path_dup);
			return (full);
		}
		free(full);
		dir = strtok(NULL, ":");
	}

	free(path_dup);
	return (NULL);
}

/**
 * execute - fork and execve the given command
 * @args: NULL-terminated array of command and arguments
 * Return: exit status of child, or -1 on error/no fork
 */
int execute(char **args)
{
	char *cmd_path;
	pid_t pid;
	int status;

	/* decide full path: direct or via PATH */
	if (strchr(args[0], '/'))
	{
		cmd_path = args[0];
		if (access(cmd_path, X_OK) != 0)
		{
			perror(cmd_path);
			return (-1);
		}
	}
	else
	{
		cmd_path = find_path(args[0]);
		if (cmd_path == NULL)
		{
			fprintf(stderr, "%s: not found\n", args[0]);
			return (-1);
		}
	}

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		if (cmd_path != args[0])
			free(cmd_path);
		return (-1);
	}

	if (pid == 0)
	{
		/* child process */
		execve(cmd_path, args, environ);
		perror(cmd_path);
		exit(EXIT_FAILURE);
	}

	/* parent waits for child */
	waitpid(pid, &status, 0);
	if (cmd_path != args[0])
		free(cmd_path);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	return (status);
}
