# simple_shell

A simple UNIX command-line interpreter project for Holberton School.

## Description

`simple_shell` is a minimal POSIX-compliant shell that:
- Displays a prompt (`$ `) in interactive mode and waits for user input.
- Reads and executes simple commands with or without arguments.
- Searches for executables in `PATH` or accepts absolute/relative paths.
- Implements two built-ins:
  - `env`: prints the current environment.
  - `exit`: exits the shell, returning the last command’s status.
- Handles end-of-file (`Ctrl+D`) by exiting gracefully.
- Conforms to Betty style (tabs for indentation, ≤ 80 characters per line, English comments).
- Has no memory leaks (verified with Valgrind).

## Compilation

```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 shell.c -o hsh

Usage
Interactive mode
./hsh
$ /bin/ls
file1  shell.c  shell.h
$ env
USER=julien
PATH=/usr/local/bin:...
$ exit

Non-interactive mode

echo "/bin/ls -l /tmp" | ./hsh
total 20
drwxrwxrwt   9 root root 4096 Apr 23 10:00 tmp

Built-ins
env
Prints each environment variable on its own line:
$ env
USER=julien
HOME=/home/julien
PATH=/usr/bin:...

exit
Exits the shell, returning the exit status of the last executed command:
$ exit

Project Structure
holbertonschool-simple_shell/
├── AUTHORS              # list of contributors
├── README.md            # project documentation (this file)
├── man_1_simple_shell   # man page for hsh, section 1
├── shell.h              # function prototypes and include guard
└── shell.c              # shell implementation (loop, parsing, exec, built-ins)

