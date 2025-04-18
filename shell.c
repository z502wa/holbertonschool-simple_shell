#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * main - entry point for simple_shell
 *
 * Return: 0 on success, or exit on failure
 */
int main(void)
{
	char *line, **args;
	int status;
	int interactive = isatty(STDIN_FILENO);

	while (1)
	{
		/* display prompt only in interactive mode */
		if (interactive)
			if (write(STDOUT_FILENO, "$ ", 2) == -1)
				exit(EXIT_FAILURE);

		line = read_line();
		if (line == NULL)
		{
			/* handle EOF (Ctrl+D) in interactive mode */
			if (interactive)
				write(STDOUT_FILENO, "\n", 1);
			break;
		}

		line = trim_newline(line);
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
 *
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
 * trim_newline - replace trailing newline with NUL byte
 * @str: input string
 *
 * Return: pointer to modified string
 */
char *trim_newline(char *str)
{
	size_t i = strlen(str);

	if (i > 0 && str[i - 1] == '\n')
		str[i - 1] = '\0';

	return (str);
}

/**
 * split_line - split a line into tokens (arguments)
 * @line: input string
 *
 * Return: NULL-terminated array of pointers to tokens
 */
char **split_line(char *line)
{
	size_t bufsize = 64, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (tokens == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, " \t");
	while (token != NULL)
	{
		tokens[position++] = token;

		if (position >= bufsize)
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
	tokens[position] = NULL;
	return (tokens);
}

/**
 * execute - fork and execve the given command
 * @args: NULL-terminated array of command and arguments
 *
 * Return: exit status of child, or -1 on fork error
 */
int execute(char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}

	if (pid == 0)
	{
		/* child process */
		execve(args[0], args, environ);
		/* execve failed */
		perror(args[0]);
		exit(EXIT_FAILURE);
	}

	/* parent waits for child */
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	return (status);
}
