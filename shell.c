#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

char **tokenize(char* line){

    //     tokens
    // +-------+-------+-------+-------+-------+
    // |   *   |   *   |   *   |   *   |  ...  |
    // +-------+-------+-------+-------+-------+
    //   |
    //   v
    //   "hello"  "world"  "this"  "is"  "C"

    // token
    // +---+---+---+---+---+---+---+---+---+---+
    // | h | e | l | l | o | \0|   |   |   |   |
    // +---+---+---+---+---+---+---+---+---+---+

    char** tokens = (char**)malloc(MAX_NUM_TOKENS * sizeof(char *));  //this will hold individual tokens extracted from the input line. consider it as an array of pointers to string
    char* token = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));  //this will hold a single token. temporarily holds characters as they are read from the input line.

    //memset is typiically used to initialise memory to a certain value
    //initially the tokens array contains garbage values because the memory is not initialised yet.

    // tokens initially
    // +-------+-------+-------+-------+-------+
    // | ????  | ????  | ????  | ????  |  ...  |
    // +-------+-------+-------+-------+-------+


    // tokens after memset
    // +-------+-------+-------+-------+-------+
    // |  NULL |  NULL |  NULL |  NULL |  ...  |
    // +-------+-------+-------+-------+-------+

    memset(tokens, 0, sizeof(tokens));

    int i;
    int tokenIndex = 0;
    int tokenNo = 0;

    for(i = 0; i < strlen(line); i++){
        //reading the current character
        char readChar = line[i];
        //checking for delimiters
        if(readChar == ' ' || readChar == '\n' || readChar == '\t'){

            //terminating the current token
            token[tokenIndex] = '\0';

            if(tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));

                //copying the string from token array to the new allocated memory in the tokens array
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        }
        else{
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}

void free_mem(char **tokens){
    for(int i = 0; tokens[i] != NULL; i++){
        free(tokens[i]);
    }
    free(tokens);
}

pid_t foreground_pid = -1;

void signal_handler(int sig){
	if(foreground_pid > 0){
		kill(-foreground_pid, SIGINT);
	}
}

int main(int argc, char *argv[]){
    char line[MAX_INPUT_SIZE];
    char**tokens;
    int i;

	//start
	struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    while(1){
        bzero(line, sizeof(line));
        printf("$ ");
        scanf("%[^\n]", line);
        getchar();

        line[strlen(line)] = '\n';
        tokens = tokenize(line);

        if(tokens[0] && !strcmp(tokens[0], "exit") && !tokens[2]){
            free_mem(tokens);
            kill(-getpid(), SIGTERM);
            exit(0);
        }

        int check;

        while(check = waitpid(-1, NULL, WNOHANG) > 0){
            printf("Background process finished\n");
        }

        if(!tokens[0]){
            free_mem(tokens);
            continue;
        }

        int background = -1;

        for(int i = 0; i < MAX_NUM_TOKENS; i++){
            if(tokens[i] == NULL){
                break;
            }
            else if(!strcmp(tokens[i], "&")){
                if(tokens[i+1] == NULL){
                    background = i;
                }
                else{
                    printf("Multiple commands after &\n");
                    background = -2;
                }
                break;
            }
        }
        if(background == -2){
            free_mem(tokens);
            continue;
        }

        //non cd commands
        //strcmp checks if the command is anything other than cd

        if(strcmp(tokens[0], "cd")){
            int pid = fork();
            if(pid == 0){
                if(background > 0){
                    tokens[background] = NULL;
					setpgid(0, 0);
                }
				else{
					setpgid(0, 0);
				}
                execvp(tokens[0], tokens);
                printf("Incorrect command\n");
                exit(0);
            }
            else{
                if(background == -1){
					foreground_pid = pid;
					setpgid(pid, pid);
                    waitpid(pid, NULL, 0);
					foreground_pid = -1;
                }
            }
        }
        else{
            if(background > 0){
                printf("cd cannot be executed in the background\n");
                free_mem(tokens);
                continue;
            }
            if(!tokens[1] || tokens[2]){
                printf("No directory to change to or too many arguments\n");
                free_mem(tokens);
                continue;
            }
            else{
                int ret = chdir(tokens[1]);
                if(ret){
                    printf("chdir unsuccesful\n");
                    free_mem(tokens);
                    continue;
                }
            }
        }
		//end
        free_mem(tokens);   
    }
    return 0;
}
