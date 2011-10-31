/* Parallel Spell-checking Program
* --------------------------
* Written by Huan Truong <hnt7438@truman.edu>
*/


#ifndef TRUONG_PSPELL_H
#define TRUONG_PSPELL_H

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <time.h>

using namespace std;

/* ---------------------------------------------------------------------- */
// The threshold that we want to run the recursive dist algorithm
#define DIST_THRESHOLD 2
#define MARKER_END "END LOL"

/* ---------------------------------------------------------------------- */
struct read_file_to_vector_arg {
    char * source_file;
    vector<char *> * dest_vector;
};

struct read_input_to_vector_arg {
    vector<char *> * dest_vector;
    pthread_mutex_t * words_vector_lock;
    pthread_cond_t * words_not_empty;
};

struct spell_check_arg {
    vector<char *> * dict_vector;
    vector<char *> * words_vector;
    pthread_mutex_t * words_vector_lock;
    pthread_mutex_t * output_lock;
    pthread_cond_t * words_not_empty;
};

// The dict vector that we're all gonna share
// a binary tree is probably a better choice, but I didn't have time.
vector<char *> * dict_vector;

// Vector to hold our input words
vector<char *> * words;

// Mutexes
pthread_mutex_t * words_lock;
pthread_mutex_t * io_lock;

// The conditional variable
pthread_cond_t * input_not_empty;

/* ---------------------------------------------------------------------- */
/* Well, we actually don't care about the real Levenshtien distance
* we only care about the Levenshtien distance if it's not over a threshold!
*
* So here comes the distance IMproved function, saves a bunch of function calls
*
* WARNING: This distim doesn't actually return the Levenshtien dist,
* it returns the value which guarantees real_levenshtien_dist > distim
* When DIST_THRESHOLD = infinity, then real_levenshtien_dist = distim.
*/

uint distim (uint min_accumulated, const char * x, const char * y,
	    uint len_x,  uint len_y, uint start_x,  uint start_y);

/* Find a needle in a haystack, return -1 if not found */
int find_in_vector(char * needle, vector<char *> * haystack);

void * read_file_to_vector(void * arg);

void * read_input_to_vector(void * arg);

void * spell_check(void * arg);

#endif