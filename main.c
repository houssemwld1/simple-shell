#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#define MAX_LINE 80 /* The maximum length command */
enum SpecialCommands
{
    CMD_HISTORY = 1, // Example: "!!"

};
char **parseInput(char input[], int *numWords)
{
    const char *separators = " ";
    char *strToken = strtok(input, separators);
    char **wordArray = NULL;
    int count = 0;
    while (strToken != NULL)
    {
        wordArray = (char **)realloc(wordArray, (count + 1) * sizeof(char *)); // size of (pointer to char )
        wordArray[count] = strdup(strToken);                                   // Copy the token and then gives the pointer to be stored in the array of pointer to pointer to char **
        count++;                                                               // inc number of tokens
        // printf("%s\n", strToken);
        //  On demande le token suivant.
        strToken = strtok(NULL, separators);
    }
    *numWords = count;

    return wordArray;
};

int main(void)
{
    char *history[MAX_LINE * 10];
    int record = 0;
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run)
    {
        int numTokens;
        char input[MAX_LINE];

        printf("osh>");
        fflush(stdout); // Ensure "osh>" is displayed immediately
        if (fgets(input, MAX_LINE, stdin) == NULL)
        {
            perror("Error reading input");
        }

        char **words = parseInput(input, &numTokens);
        printf("%d", numTokens);
        char *args[numTokens - 2];
        // Print the words
        for (int i = 0; i < numTokens; i++)
        {
            printf("Word %d: %s\n", i + 1, words[i]);
            args[i] = words[i];
        }

        record = record + numTokens;
        char *argss[numTokens];
        if (args[0] != NULL && strcmp(args[0], "exit") != 10)
        {
            if (strcmp(args[0], "!!") == 10)
            { // history part
                if (history[0] == NULL)
                {
                    printf("there is no history cmd \n");
                }

                else if (fork() == 0)
                { // child
                    char *command = history[0];
                    history[numTokens + 1] = NULL;
                    if (command == NULL)
                    {
                        printf("there is no history cmd");
                        break;
                    }
                    printf("Executing the command\n");
                    int status_code = execvp(command, args);

                    if (status_code == -1)
                    {
                        printf("Process did not terminate correctly\n");
                        exit(1);
                    }

                    else
                    {
                        printf("This line will not be printed if execvp() runs correctly\n");
                    }
                } //
                else
                { // parent

                    if (strcmp(history[numTokens - 1], "&") != 10)
                    {

                        wait(NULL);
                    }
                }
                for (int i = 0; i < numTokens; i++)
                {

                    history[i] = history[i];
                    free(words[i]);
                }
            }
            else
            {

                if (fork() == 0)
                { // child
                    char *command = args[0];
                    args[numTokens] = NULL;
                    printf("Executing the command\n");
                    for (int j = 0; j < numTokens - 2; j++)
                    {
                        argss[j] = args[j];
                    }
                    argss[numTokens - 2] = NULL;

                    if (strcmp(args[numTokens - 2], ">") == 0 || strcmp(args[numTokens - 2], "<") == 0)
                    {
                        int fd = open(args[numTokens - 1], O_CREAT | O_RDWR, 0666);
                        if (fd < 0)
                        {
                            printf("Couldn't open file: %d\n", errno);
                        }
                        if (dup2(fd, STDOUT_FILENO) < 0)
                        {
                            printf("Couldn't redirect stdout: %d\n", errno);
                        }
                        
                        int status_code = execvp(command, argss);

                        if (status_code == -1)
                        {
                            printf("Process did not terminate correctly\n");
                            exit(1);
                        }

                        else
                        {
                            printf("This line will not be printed if execvp() runs correctly\n");
                        }
                    }
                    else
                    {

                        int status_code = execvp(command, args);

                        if (status_code == -1)
                        {
                            printf("Process did not terminate correctly\n");
                            exit(1);
                        }

                        else
                        {
                            printf("This line will not be printed if execvp() runs correctly\n");
                        }
                    }
                } //
                else
                { // parent

                    if (strcmp(args[numTokens - 1], "&") != 10)
                    {

                        wait(NULL);
                    }
                }
                for (int i = 0; i < numTokens; i++)
                {

                    history[i] = words[i];
                    free(words[i]);
                }
            }
        }
        else
        {
            exit(0);
        }
        /**
         * After reading user input, the steps are:
         * (1) fork a child process using fork()
         * (2) the child process will invoke execvp()
         * (3) parent will invoke wait() unless command included &
         */
    }
    return 0;
}
