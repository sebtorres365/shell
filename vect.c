#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vect.h"

/** Main data structure for the vector. */
struct vect {
  char **data;             /* Array containing the actual data. */
  unsigned int size;       /* Number of items currently in the vector. */
  unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
vect_t *vect_new() {

  // create a vect_t and allocate space for it
  vect_t *vectRet;
  vectRet = (vect_t*)malloc(sizeof(vect_t));

  // intialize the capacity and size to their starting values
  vectRet->capacity = VECT_INITIAL_CAPACITY;
  vectRet->size = 0;

  // allocate space for the string arry within the vector
  vectRet->data = (char**) malloc(vectRet->capacity * sizeof(char*));

  return vectRet;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {

  // looping through the vector's array to free each value
  for (int i = 0; i < v->size; i++) {
	  free(v->data[i]);
  }

  // free the vectors array and the vector itself
  free(v->data);
  free(v);
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  // Simply returning the element at the given index within the vector
  return v->data[idx];
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);

  // allocate space to hold the element at the given index to return later
  char* ret = (char*) malloc(strlen(v->data[idx]) + 1);

  // copying the string from the given index
  strcpy(ret, v->data[idx]);

  return ret;
}

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  assert(idx < v->size);

  // Free the previous data held in the given index
  free(v->data[idx]);

  // Allocate memory for the new string to be placed at the index
  v->data[idx] = (char*)malloc(sizeof(char) * strlen(elt) + 1);

  // Placing the new element at the index
  strcpy(v->data[idx], elt);

}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);

  // If more space if needed:
  if (v->size == v->capacity) {
    // Increase the size by the growth factor defined in vect.h
    v->capacity = v->capacity * VECT_GROWTH_FACTOR;

    // reallocate memory for new size of the updated array
    v->data = (char**) realloc(v->data, sizeof(char*) * v->capacity);
  }

  // allocate memory for the new string
  v->data[v->size] = (char*) malloc(sizeof(char) * strlen(elt) + 1);

  // add the new string to the vector
  strcpy(v->data[v->size], elt);

  // increase the size of the vector
  v->size = v->size + 1;

}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);

  // Free the data of the last element
  free(v->data[v->size - 1]);

  // decrease the size of the vector by one
  v->size = v->size - 1;

}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);

  // Simply returning the size from the passed in vector
  return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);

  // Simply returning the capacity from the passed in vector
  return v->capacity;
}