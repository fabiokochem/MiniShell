# MiniShell Project

## Overview
MiniShell is a custom-built shell written in C that mimics the functionality of a Unix shell. It supports basic shell commands, job control, piping, redirection, and custom commands. The goal of this project is to provide a lightweight and functional shell that demonstrates process management, signal handling, and command execution in a Unix environment.

## Features
- **Command Execution**: Execute standard Unix commands and built-in custom commands.
- **I/O Redirection**: Support for input (`<`), output (`>`), append (`>>`), and error redirection (`2>`).
- **Piping**: Enable chaining commands using pipes (`|`).
- **Background Processes**: Run commands in the background using `&`.
- **Job Control**:
  - List active jobs (`jobs`).
  - Resume jobs in the background (`bg`).
  - Resume jobs in the foreground (`fg`).
- **Custom Commands**:
  - `cd`: Change the working directory.
  - `pwd`: Print the current working directory.
  - `jobs`: List active and suspended jobs.
  - `bg` and `fg`: Manage suspended or background jobs.
- **Signal Handling**: Handle `SIGINT` and `SIGTSTP` signals to interrupt or suspend processes.

## File Structure
- **`Processes/myshell.c`**: The main shell implementation, including the command loop and core functionalities like command parsing and execution.
- **`Processes/jobs_command.c`**: Manages job control, including listing, adding, and resuming jobs.
- **`Processes/custom_commands.c`**: Implements custom commands like `cd`, `pwd`, and job-related commands.

## Compilation and Execution
### Prerequisites
- A Unix-based system (Linux, macOS).
- GCC (GNU Compiler Collection).

### Compilation
Run the following command in the project directory:
```bash
gcc -o minishell myshell.c jobs_command.c custom_commands.c -Wall -Wextra -pedantic
```

### Execution
To start the shell, execute the following:
```bash
./minishell
```

## Usage Examples
### Basic Commands
```bash
> ls -l
> echo "Hello, World!"
```

### I/O Redirection
```bash
> echo "This is a test" > output.txt
> cat < input.txt
```

### Piping
```bash
> ls | grep "minishell"
```

### Background Processes
```bash
> sleep 10 &
```

### Job Control
```bash
> jobs
[1]    1234    Running    sleep 10
> fg 1
> bg 1
```

### Custom Commands
```bash
> cd /path/to/directory
> pwd
> jobs
```

