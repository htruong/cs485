/* Parallel Spell-checking Program
 * --------------------------
 * Written by Huan Truong <hnt7438@truman.edu>
 */

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>

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
int dist (string *x, string *y)
{
  // Find the Levenshtien distance between
  // two strings x and y.
  int xlen = x->length();
  int ylen = y->length();
  // Base cases: if either string is empty,
  // the distance is the length of the other.
  if (xlen == 0)
  return ylen;
  if (ylen == 0)
  return xlen;
  
  // Not one of the base cases.
  // Divide each string into two parts.
  // xhead = x[0 .. n-2]
  // xtail = x[n-1 .. n-1]
  string *xhead = new string(x->substr(0, xlen - 1));
  string *xtail = new string(x->substr(xlen - 1, 1));
  string *yhead = new string(y->substr(0, ylen - 1));
  string *ytail = new string(y->substr(ylen - 1, 1));
  // ... now, handle the three recursive cases.
  // These represent the dist d between the heads
  // of x and y, the dist between truncated-x
  // and y, and the dist between x and truncated-y.
  int head_d, truncx_d, truncy_d;
  // Find the distance between the string heads.
  if (*xtail == *ytail) {
  head_d = dist(xhead, yhead);
  } else {
  head_d = 1 + dist (xhead, yhead);
  }
  // Calculate distance between x[0 .. m-2] and y.
  truncx_d = 1 + dist (xhead, y);
  // Calculate distance between x and y[0 .. n-2].
  truncy_d = 1 + dist (x, yhead);
  // Find minimum of the three cases.
  int min_d = head_d;
  if (truncx_d < min_d) {
  min_d = truncx_d;
  }
  if (truncy_d < min_d) {
  min_d = truncy_d;
  }
  // Clean up and return the minimum.
  delete xhead, xtail, yhead, ytail;
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
int main (int argc, char * argv[])
{
  // Need exactly three arguments, the first one being the executable itself.
  if (argc != 3) {
    cerr << "Wrong number of arguments specified.\n"
    << "USAGE:\n\t" << argv[0] << " num_of_threads dict.txt" << endl;

    exit(1);
  }

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

  cout << "Please enter the text to be spell-checked:" << endl;

  string in_sentence;
  getline (cin,in_sentence);
  
  if (pthread_join (*reader_thread, NULL) )
  { cerr << "Error joining reader thread." << endl; abort(); }
  delete reader_thread;

  string buf;
  stringstream sentence_stream(in_sentence);

  vector<string> tokens; // Create vector to hold our words

  while (sentence_stream >> buf)
      tokens.push_back(buf);

  for (uint i=0; i < tokens.size(); i++) {
    cerr << tokens[i] << endl;
  }


  
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

  // ---------------------------------------------------------------------
  // Cleaning up
  for (uint i=0; i < dict_vector->size(); i++) {
    delete (*dict_vector)[i];
  }
  delete dict_vector;

  exit(0);
}
