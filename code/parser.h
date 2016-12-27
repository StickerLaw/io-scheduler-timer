/**
 * File: parser.h
 * Author: Lennart Jern - ens16ljn@cs.umu.se
 *
 * Systemnära programmering, 5dv088 - Laboration 4: mfind
 *
 * This is a header file for a parser that extracts search data from command
 * line arguments.
 *
 */

#include "list.h"

#define MAXTHREADS (1)

typedef struct search_data SearchData;

struct search_data {
  char *needle;
  LinkedList *directories;
  int num_threads;
  char type;
  int num_searchers;
  unsigned int error;
};

SearchData *parse_arguments(int argc, char *argv[]);
int set_flags(SearchData *s_data, int argc, char *argv[]);
void check_flags(SearchData *s_data);
void add_dirs_and_needle(SearchData *s_data, char *argv[], int first, int last);
