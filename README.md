# Mini Shell (Command Line Interpreter)
## Tools and Libraries
  - IDE: Any standard IDE.
  - Libraries: POSIX API (Linux/Unix), Windows API for Windows.

## Steps to Develop

  1. Understand System Calls:
      - Familiarize yourself with system calls like fork(), execvp(), and wait() for Unix-like systems.
      - Use Windows equivalents like CreateProcess() and WaitForSingleObject() on Windows.

  2. Core Features:
      - Execute Commands:
            - Parse the command string using std::stringstream.
            - Use fork() to create a child process.
            - Use execvp() to execute the command in the child process.
      - Handle Built-in Commands:
            - Implement basic commands like cd, exit, etc., without using execvp() (write custom handlers).
      - Wait for Process Completion:
            - Use wait() to handle foreground processes.

  3. Implement Piping and Redirection:
      - Use dup2() to redirect standard input/output for commands with pipes (|) or redirection (>, <).

  4. Enhancements:
      - Background Processes: Allow commands to run in the background using &.
      - History: Maintain a history of commands using a file or a deque.

  5. Testing:
      - Run common shell commands and compare their behavior with the real shell.
      - Test edge cases like invalid commands or incorrect arguments.
