/* Parallel Quicksort Program
 * --------------------------
 * Written by Huan Truong <hnt7438@truman.edu>
 */

#ifndef TRUONG_PQSORT_H
#define TRUONG_PQSORT_H

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

/* ---------------------------------------------------------------------- */
// Arg structs

struct read_file_to_vector_arg {
  char * source_file;
  vector<string *> * dest_vector;
};

struct pqsort_vector_arg {
  vector<string *> * in_vector;
  uint start;
  uint finish;
};

struct dump_vector_arg {
  vector<string *> * in_vector;
};

// The vector that we're all gonna share
vector<string *> * sort_vector;

/* ---------------------------------------------------------------------- */
// Read a file to a vector
void * read_file_to_vector(void * arg);

// Parallel-Quick Sort a vector
void * pqsort_vector(void * arg);

// Prints the vector to stdout
void * dump_vector(void * arg);

// Shorthand function for creating a sorter thread, as this will be called a lot
void create_sorter_thread(pthread_t * sorter_thread,
     vector<string *> * in_vector, uint start, uint finish);
/* ---------------------------------------------------------------------- */
#endif

