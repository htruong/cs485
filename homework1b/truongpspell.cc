/* Parallel Spell-checking Program
* --------------------------
* Written by Huan Truong <hnt7438@truman.edu>
*/

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <time.h>

#include "truongpspell.h"

using namespace std;


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
	    uint len_x,  uint len_y, uint start_x,  uint start_y)
{
    // If we're already over the minimum threshold, don't bother
    if (min_accumulated > DIST_THRESHOLD) return min_accumulated;

    // Base cases :-)
    if (len_x - start_x == 0) return len_y - start_y;
    if (len_y - start_y == 0) return len_x - start_x;

    // Not one of the base cases.
    uint head_d, truncx_d, truncy_d;
    uint next_x = start_x + 1;
    uint next_y = start_y + 1;
    uint next_min = min_accumulated + 1;

    uint last_chars_matched = (x[len_x - next_x] == y[len_y - next_y]);

    // When we're "on the edge," of going over the DIST_THRESHOLD,
    // the only bet is when last_chars_matched
    // otherwise, anything else is gonna make it > DIST_THRESHOLD.
    if (min_accumulated == DIST_THRESHOLD) {
	if (last_chars_matched) {
	    return distim (min_accumulated, x, y, len_x, len_y, next_x, next_y);
	} else {
	    return DIST_THRESHOLD + 1; // Not being considered.
	}
    }

    // If we're not over, then continue our normal business
    if (last_chars_matched) {
	head_d = distim (min_accumulated, x, y, len_x, len_y, next_x, next_y);
    } else {
	head_d = 1 + distim (next_min, x, y, len_x, len_y, next_x, next_y);
    }

    // Calculate distance between x[0 .. m-2] and y.
    truncx_d = 1 + distim (next_min, x, y, len_x, len_y, next_x, start_y);
    // Calculate distance between x and y[0 .. n-2].
    truncy_d = 1 + distim (next_min, x, y, len_x, len_y, start_x, next_y);
    // Find minimum of the three cases.

    uint min_d = head_d;

    if (truncx_d < min_d) {
	min_d = truncx_d;
    }

    if (truncy_d < min_d) {
	min_d = truncy_d;
    }
    //if ((start_x == 0) && (start_y == 0))
    //cout << x << ":" << y << " " << min_d << endl;
    return min_d;
}

/* ---------------------------------------------------------------------- */
int find_in_vector(char * needle, vector<char *> * haystack)
{
    for  (uint i=0; i < haystack->size(); i++) {
	if (strcmp(needle, (*haystack)[i]) == 0) return i;
    }
    return -1;
}

/* ---------------------------------------------------------------------- */
void * read_file_to_vector(void * arg)
{
    // Unpack the arg
    struct read_file_to_vector_arg *arg_pointer
    = static_cast<struct read_file_to_vector_arg *>(arg);
    char * source_file = arg_pointer->source_file;
    vector<char *> * dest_vector = arg_pointer->dest_vector;

    // Create the stream to read the file
    ifstream stream_in;
    string buffer;
    char *word;

    stream_in.open(source_file, ios::in);

    while(getline(stream_in, buffer)) {
	word = new char[buffer.length() + 1];
	strcpy(word, buffer.c_str());
	dest_vector->push_back(word);
    }

    return NULL;
}

/* ---------------------------------------------------------------------- */
void * read_input_to_vector(void * arg)
{
    // Unpack the arg
    struct read_input_to_vector_arg *arg_pointer
    = static_cast<struct read_input_to_vector_arg *>(arg);
    vector<char *> * dest_vector = arg_pointer->dest_vector;
    pthread_mutex_t * words_vector_lock = arg_pointer->words_vector_lock;
    pthread_cond_t * words_not_empty = arg_pointer->words_not_empty;

    char c;
    string * buf = new string();
    char * tmp;
    
    cout << "Please enter the text to be spell-checked:" << endl;

    // Don't know what is going on here
    // While I expected that getchar should let me get char by char
    // the input seems buffered line by line
    
    c = getchar();
    while (c != EOF) {
	if (c == ' ' || c == ',' ||  c == '.' || c == '\n' /* et cetera */) {
	    if (buf->length() > 0) {
		tmp = new char[buf->length() + 1];
		strcpy(tmp, buf->c_str());
		
		pthread_mutex_lock(words_vector_lock);
		dest_vector->push_back(tmp);
		pthread_cond_broadcast (words_not_empty);
		pthread_mutex_unlock(words_vector_lock);

		delete buf;
		buf = new string();
	    }
	} else {
	    buf->append(1, c);
	}
	c = getchar();
    }

    tmp = (char *)MARKER_END;
    pthread_mutex_lock(words_vector_lock);
    dest_vector->push_back(tmp);
    pthread_cond_broadcast (words_not_empty);
    pthread_mutex_unlock(words_vector_lock);

    return NULL;
}

/* ---------------------------------------------------------------------- */
void * spell_check(void * arg)
{
    // Unpack the arg
    struct spell_check_arg *arg_pointer
    = static_cast<struct spell_check_arg *>(arg);
    vector<char *> * dict_vector = arg_pointer->dict_vector;
    vector<char *> * words_vector = arg_pointer->words_vector;
    pthread_mutex_t * words_vector_lock = arg_pointer->words_vector_lock;
    pthread_mutex_t * output_lock = arg_pointer->output_lock;
    pthread_cond_t * words_not_empty = arg_pointer->words_not_empty;

    while (true) {
	pthread_mutex_lock (words_vector_lock);
	
	while (words_vector->empty()) {
	    pthread_cond_wait (words_not_empty, words_vector_lock);
	}

	char * word = words_vector->front();
	
	if (strcmp(word, MARKER_END) != 0) {
	    words_vector->erase (words_vector->begin());
	}

	pthread_mutex_unlock(words_vector_lock);

	if (strcmp(word, MARKER_END) == 0) {
	    return NULL;
	}
	
	bool in_dict = (find_in_vector(word, dict_vector) != -1);

	vector<char *> suggestions;

	if (!in_dict) {
	    for (uint j=0; j < dict_vector->size(); j++) {
		uint _dist = distim(0, word, (*dict_vector)[j],
				    strlen(word), strlen((*dict_vector)[j]), 0, 0);

		if (_dist <= DIST_THRESHOLD) {
		    suggestions.push_back((*dict_vector)[j]);
		}
	    }
	}

	pthread_mutex_lock(output_lock);
	if (in_dict) {
	    cout << word  << " is spelled correctly. ";
	} else {
	    cout << word << " is not in the dictionary. ";
	    if (suggestions.size() > 0) {
		cout << " Did you mean: ";
		for (uint j=0; j < suggestions.size(); j++) {
		    cout << suggestions[j] <<
		    ((j == suggestions.size() - 1) ? "? " : ", ");
		}
	    } else {
		cout << " Can't find any suggestions. ";
	    }
	}
	pthread_mutex_unlock(output_lock);

	// There is something that prevented me from deleting word
	// This is the last chance that we'll be able to access word
	// If we don't do that now it will be garbage.
	// Can't figure out what.
	// delete word;
	
    }
    

    // should never reach here
    return NULL;
}


/* ---------------------------------------------------------------------- */
int main (int argc, char * argv[])
{
    // Need exactly three arguments, the first one being the executable itself.
    if (argc != 3) {
	cerr << "Wrong number of arguments specified.\n"
	<< "USAGE:\n\t" << argv[0] << " num_of_threads dict.txt" << endl;

	exit(1);
    }
    
    #ifndef SUPRESS_CLOCK
    clock_t start = clock();
    #endif

    uint nthreads = atoi(argv[1]);

    // Initialize the shared vectors
    dict_vector = new vector<char *>;
    words = new vector<char *>;

    // Initialize the mutexes
    words_lock = new pthread_mutex_t;
    io_lock = new pthread_mutex_t;
    input_not_empty = new pthread_cond_t;

    pthread_mutex_init(words_lock, NULL);
    pthread_mutex_init(io_lock, NULL);

    pthread_cond_init (input_not_empty, NULL);

    // ---------------------------------------------------------------------
    // Read the dictionary and keyboard input to the corresponding vectors
    // While we're just waiting for the user to enter the text, let's be a bit
    // clever and wait for the input from keyboard at the same time.
    pthread_t * reader_thread = new pthread_t;
    pthread_t * input_thread = new pthread_t;
    pthread_t **speller_thread = new pthread_t*[nthreads];
    for (uint i = 0; i < nthreads; i++) {
	speller_thread[i] = new pthread_t;
    }

    struct read_file_to_vector_arg rf_param;
    rf_param.source_file = argv[2];
    rf_param.dest_vector = dict_vector;
    void *rf_param_ptr = static_cast<void *>(&rf_param);
    if (pthread_create (reader_thread, NULL, read_file_to_vector, rf_param_ptr) )
    { cerr << "Error creating dict reader thread." << endl; abort(); }

    struct read_input_to_vector_arg ri_param;
    ri_param.dest_vector = words;
    ri_param.words_vector_lock = words_lock;
    ri_param.words_not_empty = input_not_empty;
    void *ri_param_ptr = static_cast<void *>(&ri_param);

    if (pthread_create (input_thread, NULL, read_input_to_vector, ri_param_ptr) )
    { cerr << "Error creating input reader thread." << endl; abort(); }

    if (pthread_join (*reader_thread, NULL) )
    { cerr << "Error joining dict reader thread." << endl; abort(); }
    delete reader_thread;

    // ---------------------------------------------------------------------
    // Let's create a whole bunch of spell checker threads
    // We don't need for the input thread to complete, it can continue
    // churning out words while the spell checkers are checking for the words!
    struct spell_check_arg sp_param;
    sp_param.dict_vector = dict_vector;
    sp_param.words_vector = words;
    sp_param.words_vector_lock = words_lock;
    sp_param.output_lock = io_lock;
    sp_param.words_not_empty = input_not_empty;
    void *sp_param_ptr = static_cast<void *>(&sp_param);

    for (uint i = 0; i < nthreads; i++) {
	if (pthread_create (speller_thread[i], NULL,
	    spell_check, sp_param_ptr) ) {
	    cout << "Error creating speller thread " << i << '.' << endl;
	abort();
	    }
    }

    // Now wait for the input and spelling threads to be done.
    if (pthread_join (*input_thread, NULL) )
    { cerr << "Error joining input reader thread." << endl; abort(); }
    delete input_thread;

    for (uint i = 0; i < nthreads; i++) {
	if (pthread_join (*speller_thread[i], NULL) ) {
	    cout << "Error joining speller threads." << endl;
	    abort();
	}
    }

    // ---------------------------------------------------------------------
    // Cleaning up
    for (uint i = 0; i < nthreads; i++) {
	delete speller_thread[i];
    }
    
    for (uint i=0; i < dict_vector->size(); i++) {
	delete (*dict_vector)[i];
    }
    delete dict_vector;

    delete words; delete words_lock; delete io_lock; delete input_not_empty;

    #ifndef SUPRESS_CLOCK
    cerr << "\n\nProgram execution completed in "
	<< ((double)clock() - start) << " clock ticks "
	<< "(" <<  ((double)clock() - start) / CLOCKS_PER_SEC
	<< " secs)."
	<< endl;
    #endif

    exit(0);
}
