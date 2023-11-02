#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vect.h"
#include <string.h>

int is_delimiter(char ch) {
  //Checks if the current character is a white space or tab or newline
	return ch == 32 || ch == 9 || ch == 10;
}

int is_special(char ch) {
	// Checks if the current character is any of the following: ( ) < > ; |
	return ch == 40 || ch == 41 || ch == 60 || ch == 62 || ch == 59 || ch == 124;
}

// Adds token to the vect passed in
int add_token(int j, vect_t *vect, char* token) {
	if (j > 0) {
		token[j] = '\0';
		vect_add(vect, token);
	}
	return 0;
}

// Tokenizes the input
vect_t *tokenize(char *input) {
  char token[256] = {0}; // initialize token to all zeros
  int i = 0;
  int j = 0;
  vect_t *vect = vect_new();
  while (input[i] != '\0') {
		// Adding delimeters to the tokens
    if (is_delimiter(input[i])) {
			token[j] = '\0';
			j = add_token(j, vect, token);
      i++;
    } else if (is_special(input[i])) {
			j = add_token(j, vect, token); 
      token[0] = input[i]; // Take in the single special character
      token[1] = '\0';
      vect_add(vect, token);
      i++;
    } else if (input[i] == 34) {  // Dealing with quotes input
			j = add_token(j, vect, token);	
			i++;
			// While we haven't hit the other ending quote symbol
			while(input[i] != 34) {
				// In case there is no ending quote
				if (input[i] == '\0') {
					break;
				}
				
				// building the current token
				token[j] = input[i];
				i++;
				j++;
			}
			token[j] = '\0';
			j = 0;
			// Adding token once ending quote is found
			vect_add(vect,token);
			i++;
		} else {
			// Building a generic single token when nothing special is hit
      token[j] = input[i];
      i++;
      j++;
    }
  }
	// Adds the final token if there is one once we hit the end of the string
  if (j > 0) {
    token[j] = '\0';
    vect_add(vect, token);
  } 
  return vect;
}