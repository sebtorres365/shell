#ifndef _TOKEN_H
#define _TOKEN_H

#include <limits.h>
#include "vect.h"

/** Checks if the character is a delimeter  */
int is_delimiter(char ch);

/** Checks if the character is special  */
int is_special(char ch);

/** adds a token */
int add_token(int j, vect_t *vect, char* token); 

/** tokenize main function */
vect_t *tokenize(char *input);


#endif /* ifndef _TOKEN_H */