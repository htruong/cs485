/* Parallel Quicksort Program
 * --------------------------
 * Written by Huan Truong <hnt7438@truman.edu>
 */

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "truongpqsort.h"

using namespace std;

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
void * dump_vector(void * arg)
{
  // Unpack the arg
  struct dump_vector_arg *arg_pointer
    = static_cast<struct dump_vector_arg *>(arg);
  vector<string *> * in_vector = arg_pointer->in_vector;

  // Just dump it?
  for (uint i=0; i < in_vector->size(); i++) {
    cout << (*(*in_vector)[i]) << endl;
  }

  return NULL;
}


/* ---------------------------------------------------------------------- */
void * pqsort_vector(void * arg)
{
  // Unpack the arg
  struct pqsort_vector_arg *arg_pointer
    = static_cast<struct pqsort_vector_arg *>(arg);

  vector<string *> * in_vector = arg_pointer->in_vector;
  cout << "Vector: " << in_vector << endl;
  uint start = arg_pointer->start;
  uint finish = arg_pointer->finish;
  
  // Implements Matthew's quicksort pseudo-code
  uint left, right;
  string * pivot, * swap;

  if (start < finish) {
    left = start;
    right = finish + 1;

    pivot = (*in_vector)[start];
    do {
      do {
        left++;
      } while ((left < finish) && (*((*in_vector)[left]) < *pivot));

      do {
        right--;
      } while (*((*in_vector)[right]) > *pivot);

      if (left < right) {
        swap = (*in_vector)[left];
        (*in_vector)[left] = (*in_vector)[right];
        (*in_vector)[right] = swap;
      }

    } while (left < right);

    swap = (*in_vector)[start];
    (*in_vector)[start] = (*in_vector)[right];
    (*in_vector)[right] = swap;

    pthread_t * sorter_thread_1 = new pthread_t;
    pthread_t * sorter_thread_2 = new pthread_t;

    // We're passing uint, so we have to be careful here, otherwise segfault
    if (right > 0) create_sorter_thread(sorter_thread_1, in_vector, start, right - 1);
    create_sorter_thread(sorter_thread_2, in_vector, right + 1, finish);

    if (right > 0)
      if (pthread_join (*sorter_thread_1, NULL) )
        { cout << "Error joining sorter thread 1." << endl; abort(); }
    if (pthread_join (*sorter_thread_2, NULL) )
      { cout << "Error joining sorter thread 2." << endl; abort(); }

    delete sorter_thread_1;
    delete sorter_thread_2;
  }
  return NULL;
}

/* ---------------------------------------------------------------------- */
void create_sorter_thread(pthread_t * sorter_thread,
     vector<string *> * in_vector, uint start, uint finish)
{
  // Pack the arguments
  struct pqsort_vector_arg pqs_param;
  pqs_param.in_vector = in_vector;
  pqs_param.start = start;
  pqs_param.finish = finish;
  void *pqs_packed_param_ptr = static_cast<void *>(&pqs_param);
  
  struct pqsort_vector_arg *arg_pointer
    = static_cast<struct pqsort_vector_arg *>(pqs_packed_param_ptr);
    
  cout << "Calling sort from " << start << " to " << finish << " vector " << arg_pointer->in_vector << endl;
  
  if (pthread_create (sorter_thread, NULL, pqsort_vector, pqs_packed_param_ptr) )
  { cout << "Error creating sorter thread." << endl; abort(); }
}


/* ---------------------------------------------------------------------- */
int main (int argc, char * argv[])
{
  // Need exactly two arguments, the first one being the executable itself.
  if (argc != 2) {
    cerr << "Wrong number of arguments specified.\n"
    << "USAGE:\n\t" << argv[0] << " filename.txt" << endl;

    exit(1);
  }

  // Initialize the sort vector
  sort_vector = new vector<string *>;

  // ---------------------------------------------------------------------
  // Read the source file to the vector
  pthread_t * reader_thread = new pthread_t;

  struct read_file_to_vector_arg rf_param;
  rf_param.source_file = argv[1];
  rf_param.dest_vector = sort_vector;
  void *rf_packed_param_ptr = static_cast<void *>(&rf_param);
  if (pthread_create (reader_thread, NULL, read_file_to_vector, rf_packed_param_ptr) )
  { cout << "Error creating reader thread." << endl; abort(); }
  if (pthread_join (*reader_thread, NULL) )
  { cout << "Error joining reader thread." << endl; abort(); }
  delete reader_thread;


  // ---------------------------------------------------------------------
  // Sort the vector
  
  // We're passing sort_vector.size() -1 as uint so we need to be careful here.
  if (sort_vector->size() <= 0) {
    cout << "The file seems to be empty?" << endl; abort();
  }
  
  pthread_t * sorter_thread = new pthread_t;
  
  create_sorter_thread(sorter_thread, sort_vector, 0, sort_vector->size() - 1);
  if (pthread_join (*sorter_thread, NULL) )
  { cout << "Error joining sorter thread." << endl; abort(); }
  delete sorter_thread;
  
  // ---------------------------------------------------------------------
  // Printing the result
  pthread_t * dumper_thread = new pthread_t;

  struct dump_vector_arg dv_param;
  dv_param.in_vector = sort_vector;
  void *dv_packed_param_ptr = static_cast<void *>(&dv_param);
  if (pthread_create (dumper_thread, NULL, dump_vector, dv_packed_param_ptr) )
  { cout << "Error creating dumper thread." << endl; abort(); }
  if (pthread_join (*dumper_thread, NULL) )
  { cout << "Error joining dumper thread." << endl; abort(); }
  delete dumper_thread;

  // ---------------------------------------------------------------------
  // Cleaning up
  for (uint i=0; i < sort_vector->size(); i++) {
    delete (*sort_vector)[i];
  }
  delete sort_vector;

  exit(0);
}
