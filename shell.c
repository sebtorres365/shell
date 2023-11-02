#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "token.h"
#include "vect.h"
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>

// Global Variables
size_t size = 255;
char* valid_commands[6] = {"ls", "exit", "cd", "source", "prev", "help"};
int prev_size = 0;
char* prev_cmd[256];
bool isSource = false;

// Define this function since executeCommands and run_source are referencing eachother circularly
void executeCommands(char** lineArray, int size);

// Checks if the given token is a delared command
int is_command(const char* token) { 
	for (int i = 0; i < sizeof(valid_commands)/8; i++) {
		if (strcmp(token, valid_commands[i]) == 0) {
			return 1;
		} 
	}
	return 0;
}

// Splits an array of characters into a sub-array from the start to end indexes into the passed in result
void split(char** input, int start, int end, char** result) {
	int j = 0;
	for (int i = start; i < end; i++) {
		result[j] = (char*) malloc(sizeof(input[i]));
		strcpy(result[j], input[i]);
		j++;
	}
	return;
}


// Prints out the text for the print command
void print_help() {
	printf("You have called for help\n");

	printf("Here are the current available commands:\n");
	for (int i = 0; i < sizeof(valid_commands)/8; i++) { //print out the valid commands
		printf("- %s\n", valid_commands[i]);
	}
	
	//now to print out and explain each one
	printf("\nls: \n- ");
	printf("ls will display all of the files in your current working directory.\n");

	printf("\nexit: \n- ");
	printf("the exit command will cause you to leave this shell, displaying 'Bye bye.'\n");

	printf("\ncd: \n- ");
	printf("This command should change the current working directory of the shell to the path specified as the argument.\n");

	printf("\nsource: \n- ");
	printf("Execute a script. Takes a filename as an argument and processes each line of the file as a command.\n");

	printf("\nprev: \n- ");
	printf("Prints the previous command line and executes it again, without becomming the new command line.\n");

	printf("\nhelp: \n- ");
	printf("Explains all the built-in commands available in your shell\n");
}



// This function will add all items in the passed in vector to the passed in array and terminate it
void vectToArray(vect_t *v, char** arr) {
	int i;
	
	// Copy the vect values into the passed in array
	for (i = 0; i < vect_size(v); i++) {
		arr[i] = vect_get_copy(v, i);
	}

	// Add terminating null
	if (vect_size(v) < 255) {
		arr[vect_size(v)] = NULL;
	}
}


// Runs our source command
void run_source(char** token_arr) {
		// opening the file
		FILE *fd = fopen(token_arr[1], "r");

		vect_t *file_lines = vect_new();
		
		char line[256];
		
		while (fgets(line, sizeof(line), fd) != NULL) {
			line[strlen(line) - 1] = '\0';
			vect_add(file_lines, line);
		}

		vect_t* file_tokens[vect_size(file_lines)]; //array of vect_ts

		for (int i = 0; i < vect_size(file_lines); i++) {
			file_tokens[i] = tokenize((char *)vect_get(file_lines, i)); //tokenize each line and store
																																	// it in our array of vects

			// Adding null character to the end of commands when new lines are found
			for (int j = 0; j < vect_size(file_tokens[i]); j++) {
				if (strcmp((char *) vect_get(file_tokens[i], j), "\n") == 0){
					vect_set(file_tokens[i], j, '\0');
				}
			}
		}
		
		// Looping through to execute every command
		char* temp_line[256];
		for (int i = 0; i < vect_size(file_lines); i++) {
			vectToArray(file_tokens[i], temp_line); //turning vect into array

			executeCommands(temp_line, sizeof(line)); // Call to execute every command ie temp_line
		}
	
		// Closing the file
		fclose(fd); 
}


// Runs the main commands possible in our shell
void executeCommands(char** lineArray, int size) {

	// Checks if the command is source so sequencing doesn't need to run if it is
	if (strcmp(lineArray[0], "source") == 0) {
		isSource = true;
	}
	
	// Looks for a ; in the command, and will execute sequencing functionality
	if (!isSource) {
			//this code checks if there is a ';' in the command, and will execute sequencing functionality
		for (int i = 0; i < size; i++) {
			if (strcmp(lineArray[i], ";") == 0) {
				char* left[256];
				split(lineArray, 0, i, left);	//split into left side and right side
				char* right[256];
				split(lineArray, i + 1, size, right);
				right[size - i - 1] = NULL; //make sure the last character on the right side is a null char
		

				int childA = fork();

				if (childA == 0) {
					executeCommands(left, i);	// execute left side first in childA
					exit(1);
				} else if (childA > 0) {
					wait(NULL);
					int childB = fork();
					if (childB == 0) {				//wait for left side to finish, and then fork again 
						executeCommands(right, size - (i + 1)); // execute right side in childB
						exit(1);
					} else if (childB > 0) {
						wait(NULL);
						return;
					}
				}
			}
		}
	}


	//implementation of input redirection
	if (!isSource) {
  for (int i = 0; i < size; i++) {
  	if (strcmp(lineArray[i], "<") == 0) {//find the < char and split the line array around it
    	char* left[256];
      split(lineArray, 0, i, left);
      char* right[256];
      split(lineArray, i + 1, size, right);
      const char* file = right[0];
      int child = fork();

    	if (child == 0) {
      	close(0);    //close stdin and replace it with fd
        int fd;
        if (fd = open(file, O_RDONLY) == -1) { //make sure our file opens
        	printf("Cannot read file\n");
          exit(1);
        }
        executeCommands(left, i); //run our command
     	}
      wait(NULL); //wait for child to finish
      return;
    }
  }
}


	// Looks for a '>' in the command, and will redirect output
	if (!isSource) {
	for (int i = 0; i < size; i++) {
		if (strcmp(lineArray[i], ">") == 0) {
			// Splitting up the command from the file
			char* left[256];
			split(lineArray, 0, i, left);
			char* right[256];
			split(lineArray, i + 1, size, right);
			const char* file = right[0];

			// Creating a child process
			int child_pid = fork();
			if (child_pid == 0) {
				close(1);
				// Setting file descriptor to the file from the split up command
				int fd;
				if (fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644) == -1) {
					printf("Cannot read file\n");
					exit(1);
				}
				// Execute the commands from left with new file descriptor
				executeCommands(left, i);
				exit(1);
			}
			wait(NULL);
			return;
		}
	}
}

  // Attempt at piping command
	// Looks for a '|' in the command, and will do piping
	if (!isSource) {
	for (int i = 0; i < size; i++) {
  	if (strcmp(lineArray[i], "|") == 0) {
			// Splitting up the given commands
    	char* left[256];
      split(lineArray, 0, i, left);
      char* right[256];
      split(lineArray, i + 1, size, right);
	    int childA = fork();

      if (childA == 0) {
      	int pipe_fds[2];
        pipe(pipe_fds);

        int read_fd = pipe_fds[0];
        int write_fd = pipe_fds[1];

        int childB = fork();
        if (childB == 0) {
        	close(read_fd);
          close(1);
          dup(write_fd);
          executeCommands(left, i);
        } else if (childB > 0) {
          wait(NULL);

          close(write_fd);
          close(0);
          dup(read_fd);
          executeCommands(right, size - (i + 1));
        }
     } else if (childA > 0) {
	     wait(NULL);
       return;
     }
  }
 }
}


  // creates a fork for child process to execute commands
  pid_t pid = fork();

  if (pid == 0) {  // Within the child
    // Dealing with the built in prev command
    if (strcmp(lineArray[0], "prev") == 0) {
    	for (int i = 0; i < prev_size; i++) {
      	printf("%s ", prev_cmd[i]);  //prints out each token
      }
      printf("\n");

      executeCommands(prev_cmd, size);
      exit(1);
    }
    // Dealing with the built in source command
    else if (strcmp(lineArray[0], "source") == 0) {
        run_source(lineArray);
		}
    // Dealing with the built in help command
    else if (strcmp(lineArray[0], "help") == 0) {
        print_help();
        }
    // Dealing with the basic shell commands
    else if (strcmp(lineArray[0], "cd") != 0 && strcmp(lineArray[0], "\n") != 0) {
    	execvp(lineArray[0], lineArray);
      printf("%s: Command not found\n", lineArray[0]);
      exit(1);
   	}
    exit(1);
 	}
  wait(NULL);

  // Dealing with the built in cd command
  if (strcmp(lineArray[0], "cd") == 0) {
        chdir(lineArray[1]);
  }
}


int main(int argc, char **argv) {
	// Opening welcome when running shell
	printf("Welcome to mini-shell. \n");

	// This array of strings of the users input for each line
  char* lineArray[256];
	// Boolean for while loop
	bool isRunning = true;

	while(isRunning) {
		// Prints out this for before every user input
		printf("shell $ ");

		// Variables for getline call
		int bytes;
		char *read_str;
		read_str = (char *) malloc(size);
		int vectSize;
		
		// Getting the line input from user (max of 256 characters)
		bytes = getline(&read_str, &size, stdin);

		// Adding null character to end of bytes
		if (bytes > 1) {
			read_str[bytes] = '\0';
		}
	
		// Continue shell if user just presses enter
		if (strcmp(read_str, "\n") == 0) {
			continue;
		}

		// Tokenizing the inputting line from user
		vect_t *vect = tokenize(read_str);

		// End of line is detected
		if (bytes == -1) {
			break;
		}

		// If the first token is exit then exit the mini-shell
		if (strcmp(vect_get(vect, 0), "exit") == 0) {
			break;	
		}

		if (strcmp(vect_get(vect, 0), "prev") != 0) {
			//holding the previous command here, but only if the command was a valid one and not prev
			vectToArray(vect, prev_cmd);
			prev_size = vect_size(vect);		//sets the size of the previous command 
																				//so not access to unallocated space in later loop
		}

		// convert vector to array for executing execvp later
		vectToArray(vect, lineArray);

		// get the size of the vect
		vectSize = vect_size(vect);

		// Execute the commands:
		executeCommands(lineArray, vectSize);
		isSource = false;


		// free every item in data and set them to null to reset
		for (int i = 0; i < vect_size(vect); i++) {
			free(lineArray[i]);
			lineArray[i] = NULL;
		}
		wait(NULL);
	
		// Free up memory from the vector
		vect_delete(vect);
	
	}

	printf("Bye bye.\n");

  return 0;
}