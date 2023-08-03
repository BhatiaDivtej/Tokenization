#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
char** tokenize(char* input_string) {
    // allocate memory for the array of tokens
    char** tokens = malloc(MAX_INPUT_LENGTH * sizeof(char*));
    // initialize the number of tokens to zero
    int token_count = 0;
    // allocate memory for the current token
    char* current_token = malloc(MAX_INPUT_LENGTH * sizeof(char));
    // initialize the length of the current token to zero
    int current_token_length = 0;
    // set the flag to indicate whether we are inside quotes to false
    int in_quotes = 0;
    // loop through each character in the input string
    for (int i = 0; i < strlen(input_string) && i < MAX_INPUT_LENGTH; i++) {
        char c = input_string[i];
        // if the character is a double quote, toggle the in_quotes flag
        if (c == '"') {
            in_quotes = !in_quotes;
        // if the character is whitespace and not inside quotes, we have a complete token
        } else if (isspace(c) && !in_quotes) {
            // if the current token has length > 0, terminate it with a null character
            if (current_token_length > 0) {
                current_token[current_token_length] = '\0';
                // add the current token to the array of tokens
                tokens[token_count++] = current_token;
                // allocate memory for a new current token
                current_token = malloc(MAX_INPUT_LENGTH * sizeof(char));
                // reset the length of the new current token to zero
                current_token_length = 0;
            }
        // if the character is an operator and not inside quotes, we have a complete token
        } else if (strchr("()<;>|", c) != NULL && !in_quotes) {
            // if the current token has length > 0, terminate it with a null character
            if (current_token_length > 0) {
                current_token[current_token_length] = '\0';
                // add the current token to the array of tokens
                tokens[token_count++] = current_token;
                // allocate memory for a new current token
                current_token = malloc(MAX_INPUT_LENGTH * sizeof(char));
                // reset the length of the new current token to zero
                current_token_length = 0;
            }
            // allocate memory for an operator token
            char* operator_token = malloc(2 * sizeof(char));
            operator_token[0] = c;
            operator_token[1] = '\0';
            // add the operator token to the array of tokens
            tokens[token_count++] = operator_token;
        // otherwise, we are inside a token
        } else {
            // append the character to the current token and increment its length
            current_token[current_token_length++] = c;
        }
    }
    // if there is still a current token after the loop,
    if (current_token_length > 0) {
        current_token[current_token_length] = '\0';
        tokens[token_count++] = current_token;
    } else {
        free(current_token);
    }
    tokens[token_count] = NULL;
    return tokens;
}

/*
    The main function
*/
int main() {
    char input_string[MAX_INPUT_LENGTH];
    fgets(input_string, MAX_INPUT_LENGTH, stdin);
    char** tokens = tokenize(input_string);
    for (int i = 0; tokens[i] != NULL; i++) {
        printf("%s\n", tokens[i]);
        free(tokens[i]);
    }
    free(tokens);
    return 0;
}
