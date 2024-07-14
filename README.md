# Shell-Implementation-in-C

This simple shell program, written in C, allows users to execute commands, manage foreground and background processes, handle signals, and change directories. Below is an explanation of the code's functionality.

## Overview
The shell program continuously prompts the user for input, tokenizes the input into individual commands and arguments, executes the commands, and handles special cases such as background execution and signal interruptions.

Features
1. Command Execution: Executes commands entered by the user.
2. Foreground and Background Processes: Supports running processes in both foreground and background.
3. Signal Handling: Handles SIGINT signals (e.g., from pressing Ctrl+C) to safely terminate foreground processes.
4. Built-in Commands: Supports cd for changing directories and exit for terminating the shell.
5. Memory Management: Efficiently allocates and deallocates memory for command tokens.
# Detailed Explanation
## Tokenization
The shell reads a line of input and splits it into tokens (individual commands and arguments) using the tokenize function. This function handles spaces, tabs, and newline characters as delimiters.

## Signal Handling
A custom signal handler for SIGINT (interrupt signal) is set up using the signal_handler function. When a SIGINT signal is received, it terminates the currently running foreground process.

## Main Loop
The shell operates in an infinite loop, performing the following steps:

1. Prompting for Input: Displays a prompt ($ ) and reads a line of input from the user.
2. Tokenizing Input: Converts the input line into tokens using the tokenize function.
3. Handling Built-in Commands:
- exit: Terminates the shell.
- cd: Changes the current working directory.
4. Background Process Management: Checks if a command should be run in the background (indicated by an & at the end of the command).
5. Executing Commands: Forks a new process to execute the command using execvp. If the command is to be run in the background, it does not wait for the process to complete.
6. Waiting for Processes: Waits for background processes to finish and reports their completion.
##Memory Management
The shell ensures that all dynamically allocated memory for tokens is freed after each command execution to prevent memory leaks.
