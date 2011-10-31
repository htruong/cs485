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

//#include "truongpqsort.h"

using namespace std;

// The threshold that we want to run the recursive dist algorithm
#define DIST_THRESHOLD 2

struct read_file_to_vector_arg {
  char * source_file;
  vector<string *> * dest_vector;
};

struct dump_vector_arg {
  vector<string *> * in_vector;
};


// The dict vector that we're all gonna share
vector<string *> * dict_vector;


/* ---------------------------------------------------------------------- */
void * read_file_to_vector(void * arg)
{
  // Unpack the arg
  struct read_file_to_vector_arg *arg_pointer
    = static_cast<struct read_file_to_vector_arg *>(arg);
  char * source_file = arg_pointer->source_file;
  vector<string *> * dest_vector = arg_pointer->dest_vector;

  // Create the stream to read the file
  ifstream stream_in;
  string buffer;
  string * tmp_pointer;

  stream_in.open(source_file, ios::in);

  while(getline(stream_in, buffer)) {
    tmp_pointer = new string(buffer);
    dest_vector->push_back(tmp_pointer);
  }

  return NULL;
}

/* ---------------------------------------------------------------------- */
/* Well, we actually don't care about the real Levenshtien distance
 * we only care about the Levenshtien distance if it's not over a threshold
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
    // If we're going over the minimum threshold, don't bother
    if (min_accumulated > DIST_THRESHOLD) return min_accumulated;
    
    // Base cases :-)
    if (len_x - start_x == 0) return len_y - start_y;
    if (len_y - start_y == 0) return len_x - start_x;

    // Not one of the base cases.
    uint head_d, truncx_d, truncy_d;
    uint next_x = start_x + 1;
    uint next_y = start_y + 1;
    uint next_min = min_accumulated + 1;
    bool last_chars_matched = (x[len_x - next_x] == y[len_y - next_x]);

    // If we're "on the edge," meaning
    // min_accumulated = DIST_THRESHOLD,
    // then the only bet is when the last character matched
    // otherwise, anything else is gonna make it > DIST_THRESHOLD.

    
    if (min_accumulated == DIST_THRESHOLD) {
	if (last_chars_matched) {
	    return distim (min_accumulated, x, y, len_x, len_y, next_x, next_y);
	} else {
	    return DIST_THRESHOLD + 1; // Not being considered.
	}
    }
    
    // Otherwise, continue our normal business
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
void * dump_vector(void * arg)
{
  // Unpack the arg
  struct dump_vector_arg *arg_pointer
    = static_cast<struct dump_vector_arg *>(arg);
  vector<string *> * in_vector = arg_pointer->in_vector;

  // Just dump it?
  for (uint i=0; i < in_vector->size(); i++) {
    cerr << (*(*in_vector)[i]) << endl;
  }

  return NULL;
}

/* ---------------------------------------------------------------------- */
bool in_array(string * needle, vector<string *> * haystack)
{
    for  (uint i=0; i < haystack->size(); i++) {
	if (*needle == *((*haystack)[i])) return true;
    }
    return false;
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

  clock_t start = clock();

  // Initialize the dict vector
  dict_vector = new vector<string *>;
  
  // ---------------------------------------------------------------------
  // Read the dictionary to the vector
  //
  // While we're just waiting for the user to enter the text, let's be a bit
  // clever and wait for the input to be spell-checked at the same time.
  pthread_t * reader_thread = new pthread_t;

  struct read_file_to_vector_arg rf_param;
  rf_param.source_file = argv[2];
  rf_param.dest_vector = dict_vector;
  void *rf_packed_param_ptr = static_cast<void *>(&rf_param);
  if (pthread_create (reader_thread, NULL, read_file_to_vector, rf_packed_param_ptr) )
  { cerr << "Error creating dict reader thread." << endl; abort(); }

  if (pthread_join (*reader_thread, NULL) )
  { cerr << "Error joining dict reader thread." << endl; abort(); }
  delete reader_thread;

  cout << "Please enter the text to be spell-checked:" << endl;

  vector<string *> tokens; // Create vector to hold our words
  
  char c;
  string * buf = new string();

  while (cin.good()) {
      c = cin.get();
      if (c == ' ' || c == ',' || c == '.' || c == '\n' /* et cetera */) {
	tokens.push_back(buf);
	buf = new string();
      } else {
	buf->append(1, c);
      }
  }
  
  vector<string *> misspelled_words;
  vector<string *> ok_spelled_words;
  for (uint i=0; i < tokens.size(); i++) {
    if (in_array(tokens[i], dict_vector)) {
	ok_spelled_words.push_back(tokens[i]);
    } else {
	misspelled_words.push_back(tokens[i]);
    }
  }

  for (uint i=0; i < ok_spelled_words.size(); i++) {
    cerr << "Spelled correctly: " << *(ok_spelled_words[i]) << endl;
  }
  
  
  for (uint i=0; i < misspelled_words.size(); i++) {
    char *mispelled;
    mispelled = new char[(misspelled_words[i])->length() + 1];
    strcpy(mispelled, (misspelled_words[i])->c_str());
    
    cerr << "Not spelled correctly: " << *(misspelled_words[i]) << endl;
    cerr << "Did u mean: " ;
    for (uint j=0; j < dict_vector->size(); j++) {
	char *cmp;
	cmp = new char[((*dict_vector)[j])->length() + 1];
	strcpy(cmp, ((*dict_vector)[j])->c_str());
	uint _dist = distim(0, mispelled, cmp, strlen(mispelled), strlen(cmp), 0, 0);
	if (_dist <= DIST_THRESHOLD) {
	    cerr << *((*dict_vector)[j]) << " (" << _dist  << "), ";
	}
    }
    cerr << endl ;
  }

  
  /*
  // ---------------------------------------------------------------------
  // Printing the result
  pthread_t * dumper_thread = new pthread_t;

  struct dump_vector_arg dv_param;
  dv_param.in_vector = dict_vector;
  void *dv_packed_param_ptr = static_cast<void *>(&dv_param);
  if (pthread_create (dumper_thread, NULL, dump_vector, dv_packed_param_ptr) )
  { cerr << "Error creating dumper thread." << endl; abort(); }
  if (pthread_join (*dumper_thread, NULL) )
  { cerr << "Error joining dumper thread." << endl; abort(); }
  delete dumper_thread;
  */
  
  // ---------------------------------------------------------------------
  // Cleaning up
  for (uint i=0; i < dict_vector->size(); i++) {
    delete (*dict_vector)[i];
  }
  delete dict_vector;

  cerr << "Program execution completed in " << ((double)clock() - start) << " clock ticks" << endl;
  
  exit(0);
}
