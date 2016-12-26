/**
 * File: parser.c
 * Author: Lennart Jern - ens16ljn@cs.umu.se
 *
 * Systemnära programmering, 5dv088 - Laboration 4: mfind
 *
 * This parser extracts search data from command line arguments.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include "parser.h"

/**
 * parse_arguments - parse command line arguments into SearchData.
 * The SearchData should be freed by calling SearchData_delete when
 * you are done.
 * @param  argc -- number of arguments
 * @param  argv -- array of arguments
 * @return      SearchData containing: dirs to look in, needle to look
 *                     for, file type and number of threads.
 */
SearchData *parse_arguments(int argc, char *argv[]) {
  SearchData *s_data;
  char *usage = "Usage: ./mfind [-t {d|f|l}] [-p nrthr] start1 [start2 ...] name";

  // No point to continue if there are less than 3 args
  if (argc < 3) {
    fprintf(stderr, "%s\n", usage);
    exit(EXIT_FAILURE);
  }

  s_data = malloc(sizeof(SearchData));
  if (s_data == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  // Set num_threads and type from given arguments
  if (set_flags(s_data, argc, argv) != 0) {
    fprintf(stderr, "%s\n", usage);
    exit(EXIT_FAILURE);
  }
  // Make sure the flags are safe
  check_flags(s_data);

  // Check that there is at least one dir to look in and a name to look for.
  if (optind >= argc -1) {
    fprintf(stderr, "%s\n", usage);
    free(s_data);
    exit(EXIT_FAILURE);
  }

  add_dirs_and_needle(s_data, argv, optind, argc-1);

  return s_data;
}

/**
 * set_flags - parse the arguments and add corresponding search data
 * @param s_data -- SearchData to add info to
 * @param argc   -- number of arguments
 * @param argv   -- array of arguments
 * @return          0 if everything went well, -1 otherwise.
 */
int set_flags(SearchData *s_data, int argc, char *argv[]) {
  char *optstr = "t:p:";
  int opt;
  char type = '\0';
  int num_threads = 1;

  // Parse flags
  while ((opt = getopt(argc, argv, optstr)) != -1) {
    char *end;
    switch (opt) {
      case 't':
        type = *optarg;
        break;
      case 'p':
        errno = 0;
        num_threads = strtol(optarg, &end, 10);
        if (errno != 0) {
          perror("strtol");
        }
        break;
      default:
        return -1;
    }
  }

  s_data->num_threads = num_threads;
  s_data->type = type;
  return 0;
}

/**
 * check_flags - make sure the flags (num_threads and type) are correct and safe
 * @param s_data -- SearchData to check.
 */
void check_flags(SearchData *s_data) {
  int num_threads = s_data->num_threads;
  char type = s_data->type;

  if (num_threads < 1) {
    fprintf(stderr, "Number of threads must be more than 0!\n");
    exit(EXIT_FAILURE);
  } else if (num_threads > MAXTHREADS) {
    fprintf(stderr, "Too many threads! Resetting to 1.\n");
    num_threads = 1;
  }
  if (type != 'd' && type != 'f' && type != 'l' && type != '\0') {
    fprintf(stderr, "Type must be d|f|l.\n");
    exit(EXIT_FAILURE);
  }

  s_data->num_threads = num_threads;
  s_data->type = type;
}

/**
 * add_dirs_and_needle - parse starting dirs and needle from arguments and
 * add them to the search data.
 * @param s_data -- search data to add to
 * @param argv   -- array of arguments
 * @param first  -- index of first directory in argv
 * @param last   -- index of last element in argv. This should be the needle.
 */
void add_dirs_and_needle(SearchData *s_data, char *argv[], int first, int last) {
  LinkedList *dirs = List_init();
  char *needle;
  int num_dirs = last - first;

  // Add all starting directories to the list of dirs.
  for (int i = 0; i < num_dirs; i++) {
    char *new_dir = malloc(strlen(argv[first+i])+1);
    if (new_dir == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
    strcpy(new_dir, argv[first+i]);

    if (List_append(dirs, (void *)new_dir) == 0) {
      fprintf(stderr, "Could not add path to list.\n");
      s_data->error++;
    }
  }

  // The last arg is the needle/name to search for
  needle = argv[last];
  s_data->needle = malloc(strlen(needle)+1);
  if (s_data->needle == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  // Add everything to s_data.
  strcpy(s_data->needle, needle);
  s_data->directories = dirs;
}
