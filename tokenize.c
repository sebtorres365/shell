#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vect.h"
#include "token.h"
#include <string.h>


int main(int argc, char **argv) {

		char read_str[256];

		// Taking in stdin when calling ./tokenize
		fgets(read_str, 256, stdin); //returns length of file
		read_str[strlen(read_str)] = '\0';
		
		// Calling tokenize on the vector
		vect_t *vect = tokenize(read_str);

		// Looping through the vector and printing out each token
		for (int i = 0; i < vect_size(vect); i++) {
			printf("%s\n", vect_get(vect, i));
		}

		// freeing the vector from memory
    vect_delete(vect);
    return 0;

}