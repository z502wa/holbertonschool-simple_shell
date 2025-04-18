# simple_shell

A simple UNIX command interpreter project for Holberton School.

## Description

`simple_shell` is a minimal POSIX‑compliant shell that can:  
- Read and execute commands in interactive and non‑interactive modes.  
- Handle basic tokenization, environment variables, and execution flow.  
- Use only permitted system calls and functions as per project requirements.  

## Compilation

```bash
# Compile all C files into the executable hsh
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh

Usage

# Interactive mode
./hsh
($) /bin/ls
file1 file2
($) exit

# Non‑interactive mode (script or pipe)
echo "/bin/ls" | ./hsh
file1 file2

