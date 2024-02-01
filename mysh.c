/**********************************************************************************************************************************************
 * By: Frank Vanris
 * Date: 1/14/ 2024
 * Desc: I am creating my own shell command prompt within the given capabilities given to me by my professor. The internal commands that I must execute are pwd, exit, and cd. The rest of the external commands that I must execute are mkdir, ls, pl, etc.
 *********************************************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


//Character length is above 256 because its cool
#define MAX_STRING 1000

//Main Internal functions
#define EXIT "exit"
#define PWD "pwd"
#define CD "cd"


//cd functionality to change directory.
void cdFunctionality(char *directory) {
	if(chdir(directory) != 0) {
		if(errno == ENOENT) {
			//No such directory
			fprintf(stderr, "No such directory\n");
		} else {
			// Change directory failed
			fprintf(stderr, "Change directory failed\n");
		}
	}
}

//This will call external methods everytime you type in a command in the mysh command prompt.
void execSplitInArguments(char *command) {
	pid_t pid = fork();

	//error will occur if child process is not created
	if(pid == -1) {
		fprintf(stderr, "Failed to create process\n");
	} else if(pid == 0) { //Child Process 
		
		//Tokenize the command into arguments
		char *args[MAX_STRING / 2]; 

		int i = 0;

		//Tokenize the command string
		char *token = strtok(command, " ");
		while (token != NULL) {
			args[i++] = token;
			token = strtok(NULL, " ");
		}
		args[i] = NULL; //NULL-terminate the array
		
		//Execute the command with arguments
		if(execvp(args[0], args) == -1) {
			fprintf(stderr, "Failed to execute command\n");
			exit(EXIT_FAILURE);
		}
	} else { //Parent process
		wait(NULL); //Wait for the child to finish
	}
}

//Function to remove the newline character from a fget string
void removeNewLine(char *str) {
	size_t length = strlen(str);
	if(length > 0 && str[length - 1] == '\n') {
		str[length-1] = '\0'; //Replace '\n' with '\0'
	}
}

//My Shell Command prompt will run until the user exits outside of the program
void myCommandPrompt() {

	//your command input is here
	char command[MAX_STRING];
	
	//cwd prompt will pop up	
	char prompt[MAX_STRING];
	
	//getting home directory
	char *home_dir = getenv("HOME");

	
	//If there is no cwd this error will print
	if(!getcwd(prompt, sizeof(prompt))) {
		fprintf(stderr, "CWD does not work\n");
	}
	
	//while loop that runs through command prompt, until user executes the exit command
	while (1) {


		if(strcmp(home_dir, prompt) == 0) {

			//if I'm in home dir I replace it with ~
			printf("~$mysh> ");
		}else {
			//The home directory is ignored and still replaced with a ~
			printf("~/%s$mysh> ", prompt + strlen(home_dir) + 1);
		}

		if(fgets(command, MAX_STRING, stdin) == NULL) {
			//Handle end-of-file or read failure
			fprintf(stderr, "Failed to read command\n");
			continue;
		}

		//Remove the newline character
		removeNewLine(command);

		//Compare the command with EXIT
		if(strcmp(command, EXIT) == 0) {
			exit(0);	//Exit the program if the command is  "exit"
		
		//Compare the command with PWD			
		}else if(strcmp(command, PWD) == 0) {
			printf("%s\n", prompt);

		//Compare the command with CD
		}else if(strncmp(command, CD, strlen(CD)) == 0) {
			
			//checking if the command starts with cd
			char *directory = command + strlen(CD) + 1; // Skip "cd " part
			cdFunctionality(directory);
			if(!getcwd(prompt, sizeof(prompt))) {
			       fprintf(stderr, "CWD does not work\n");
			}	       
			
		} else {
			//get external command if user does not input Internal Commands
			execSplitInArguments(command);
		}
	}
	return;
}


int main(void) {
	myCommandPrompt();

	//It should never get out of the myCommandPrompt() func, but if it does this error will print out.
	fprintf(stderr, "Unexpected break out of command prompt loop.\n");
	return 1;
}

