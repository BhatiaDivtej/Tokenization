// Project 1
// No other contributor
// I have implemented Task 1.1, 1.2, Task 2.1 and Task 2.2

// The code implements a shell that can execute commands and perform certain 
// built-in functionalities such as changing directories, executing a script file, 
// displaying help messages, and recalling the previous command.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 255

/*
 * This function takes an input string and a delimiter string as arguments and returns
 * an array of strings, where each string is a token extracted from the input string based
 * on the delimiter. The delimiter is a string of characters that separate the tokens.
 * 
 * Example:
 * 
 * char** tokens = tokenize("hello,world", ",");
 * 
 * The above call will return an array containing two strings:
 * 
 * tokens[0] = "hello"
 * tokens[1] = "world"
 * tokens[2] = NULL
 * 
 * The caller of this function is responsible for freeing the memory allocated for the
 * returned array and its contents.
 */
char** tokenize(char* input_string, const char* delimiter) {
    // Allocate memory for the array of tokens
    char** tokens = malloc(MAX_INPUT_LENGTH * sizeof(char*));
    // Initialize the token count and the current token buffer
    int token_count = 0;
    char* current_token = malloc(MAX_INPUT_LENGTH * sizeof(char));
    int current_token_length = 0;
    // Initialize the flag that keeps track of whether we're inside quotes
    int in_quotes = 0;
    // Loop over the input string
    for (int i = 0; i < strlen(input_string) && i < MAX_INPUT_LENGTH; i++) {
        char c = input_string[i];
        // If we encounter a double quote, flip the in_quotes flag
        if (c == '"') {
            in_quotes = !in_quotes;
        }
        // If we're not inside quotes and we encounter a delimiter character, add the current token to the tokens array
        else if (!in_quotes && strstr(delimiter, &c) != NULL) {
            // If the current token buffer is not empty, null-terminate it and add it to the tokens array
            if (current_token_length > 0) {
                current_token[current_token_length] = '\0';
                tokens[token_count++] = current_token;
                // Allocate a new buffer for the next token
                current_token = malloc(MAX_INPUT_LENGTH * sizeof(char));
                current_token_length = 0;
            }
        }
        // Otherwise, add the current character to the current token buffer
        else {
            current_token[current_token_length++] = c;
        }
    }
    // If there's any remaining text in the current token buffer, add it to the tokens array
    if (current_token_length > 0) {
        current_token[current_token_length] = '\0';
        tokens[token_count++] = current_token;
    } else {
        // If the current token buffer is empty, free it
        free(current_token);
    }
    // Add a null terminator to the end of the tokens array
    tokens[token_count] = NULL;
    // Return the array of tokens
    return tokens;
}

/*
    Main function for the Shell
    Listens for input in a while loop until 'exit has been inputted by user'
*/
int main(int argc, char **argv) {

    //initial print
    printf("Welcome to mini-shell.\n");

    // to store the last command input from the user
    char prev_command[20];

    // while loop to listen for commands
    while (1) {
        printf("shell $ ");
        char input_string[MAX_INPUT_LENGTH];
        if (fgets(input_string, MAX_INPUT_LENGTH, stdin) == NULL) {
            printf("\nBye bye.\n");
            exit(0);
        }
        input_string[strcspn(input_string, "\n")] = '\0'; // remove newline character
        char** commands = tokenize(input_string, ";");

        for (int i = 0; commands[i] != NULL; i++) {
            char** tokens = tokenize(commands[i], " \t\n\r\f\v");
            if (tokens[0] == NULL) { // empty command line
                free(tokens);
                continue;
            }
            if (strcmp(tokens[0], "exit") == 0) { // exit shell
                strcpy(prev_command, "exit");
                free(tokens);
                free(commands);
            
                printf("Bye bye.\n");
                exit(0);

            }

            if (strcmp(tokens[0], "cd") == 0) { // change directory
                strcpy(prev_command, "cd");
                if (tokens[1] == NULL) { // no argument provided
                    printf("cd: missing argument\n");
                } else if (chdir(tokens[1]) < 0) { // change directory
                    perror("cd");
                }
                free(tokens);
                continue;
            }

            if (strcmp(tokens[0], "source") == 0) { // execute a script
                strcpy(prev_command, "source");
                if (tokens[1] == NULL) { // no argument provided
                    printf("source: missing argument\n");
                } else {
                    FILE* script = fopen(tokens[1], "r"); // open script file
                    if (script == NULL) {
                        perror("source");
                    } else {
                        char script_line[MAX_INPUT_LENGTH];
                        while (fgets(script_line, MAX_INPUT_LENGTH, script) != NULL) { // process each line
                            script_line[strcspn(script_line, "\n")] = '\0'; // remove newline character
                            char** script_tokens = tokenize(script_line, " \t\n\r\f\v");
                            if (script_tokens[0] == NULL) { // empty line
                                free(script_tokens);
                                continue;
                            }
                            if (strcmp(script_tokens[0], "exit") == 0) { // exit shell
                                free(script_tokens);
                                free(commands);
                                printf("Bye bye.\n");
                                exit(0);
                            }
                            pid_t pid = fork();
                            if (pid < 0) {
                                perror("fork");
                                exit(1);
                            } else if (pid == 0) { // child process
                                if (execvp(script_tokens[0], script_tokens) < 0) { // execute command
                                    printf("%s: command not found\n", script_tokens[0]);
                                    exit(1);
                                }
                            } else { // parent process
                                int status;
                                if (waitpid(pid, &status, 0) < 0) { // wait for child process to complete
                                    perror("waitpid");
                                    exit(1);
                                }
                                free(script_tokens);
                            }
                        }
                        fclose(script);
                    }
                }
                free(tokens);
                continue;
            }

            // prev command
            if (strcmp(tokens[0], "prev") == 0) {

                if (prev_command[0] == '\0') {
                    printf("No previous command found.\n");
                } 
                else {
                    printf("%s\n", prev_command);
                    char** prev_tokens = tokenize(prev_command, " \t\n\r\f\v");
                    if (strcmp(prev_tokens[0], "prev") != 0) { // prevent infinite loop
                        pid_t pid = fork();
                        if (pid < 0) {
                            perror("fork");
                            exit(1);
                        } else if (pid == 0) { // child process
                            if (execvp(prev_tokens[0], prev_tokens) < 0) { // execute command
                                printf("%s: command not found\n", prev_tokens[0]);
                                exit(1);
                            }
                        } else { // parent process
                            int status;
                            if (waitpid(pid, &status, 0) < 0) { // wait for child process to complete
                                perror("waitpid");
                                exit(1);
                            }
                        }
                        free(prev_tokens);
                    }
                }
                strcpy(prev_command, "prev");
                free(tokens);
                continue;
            }

            // help command
            if (strcmp(tokens[0], "help") == 0) {
                strcpy(prev_command, "help");
                printf("Available built-in commands:\n");
                printf("- cd <directory>: Change the current working directory to <directory>\n");
                printf("- exit: Terminate the shell\n");
                printf("- help: Display this help message\n");
                printf("- prev: Print the previous command and execute it again\n");
                printf("- source <filename>: Process each line of <filename> as a command\n");
                free(tokens);
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(1);
            } else if (pid == 0) { // child process
                if (execvp(tokens[0], tokens) < 0) { // execute command
                    printf("%s: command not found\n", tokens[0]);
                    exit(1);
                }
            } else { // parent process
                int status;
                if (waitpid(pid, &status, 0) < 0) { // wait for child process to complete
                    perror("waitpid");
                    exit(1);
                }
                free(tokens);
            }
        }
        free(commands);
    }
    return 0;
}
