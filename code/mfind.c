/**
 * File: mfind.c
 * Author: Lennart Jern - ens16ljn@cs.umu.se
 *
 * Systemnära programmering, 5dv088 - Laboration 4: mfind
 *
 * Usage: ./mfind [-t {d|f|l}] [-p nrthr] start1 [start2 ...] name
 *
 * mfind can search after files, links and directories from given start paths.
 * The search can be done with more than one thread by specifying the flag
 * `-p#`, where # is the number of threads to use.
 *
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include "parser.h" // Includes list.h

void *find_file(void *s_data);
int search_path(SearchData *data, char *path);
int search_directory(SearchData *search_data, DIR *dir, char *path);
int get_dirent(struct dirent *priv_dirent, DIR *dir);
void process_file(char *file_path, char *name,
                  struct stat f_stat, SearchData *data);
int add_dir(LinkedList *list, char *dir_path);
void check_starting_dirs(SearchData *search_data);
void print_path(void *path);
void delete_path(void *path);
void SearchData_delete(SearchData *s_data);

pthread_mutex_t mtx; // Global mutex used to guard the search data.

/**
 * main - parse arguments, do the search and then clean up.
 * @param  argc -- number of arguments
 * @param  argv -- array of arguments
 * @return      0 if everything went well, a positive int otherwise
 */
int main(int argc, char *argv[]) {
  int ret = 0;

  if (pthread_mutex_init(&mtx, NULL) != 0) {
    fprintf(stderr, "mutex init\n");
    exit(EXIT_FAILURE);
  }

  SearchData *search_data = parse_arguments(argc, argv);
  check_starting_dirs(search_data);

  #ifdef DEBUG
    printf("# Search data\n==============\n");
    printf("# Threads: %d\n", search_data->num_threads);
    printf("# Type: %c\n", search_data->type);
    printf("# Needle: %s\n", search_data->needle);
    List_print(search_data->directories, print_path);
    printf("==============\n\n");
  #endif

  search_data->num_searchers = 0;
  // create threads
  pthread_t threads[search_data->num_threads];
  for (int i = 0; i < search_data->num_threads-1; i++) {
    if (pthread_create(&threads[i], NULL, find_file, (void *)search_data) != 0) {
      perror("Could not create thread");
    }
  }

  find_file(search_data);

  // Join the threads
  for (int i = 0; i < search_data->num_threads-1; i++) {
    pthread_join(threads[i], NULL);
  }

  // Check for errors
  ret = search_data->error;
  // Free allocated memory
  SearchData_delete(search_data);
  return ret;
}

/**
 * find_file - search for files and directories
 * @param  s_data -- search data, containing needle to look for and list of
 *                   directories to look in
 */
void *find_file(void *search_data) {
  unsigned int reads = 0;
  SearchData *data = search_data;
  char *path = NULL;
  int error = 0;

  pthread_mutex_lock(&mtx);
  // Keep searching while there are dirs in the list OR there are other
  // searchers still searching (since they may find more dirs).
  while((path = (char *)List_get(data->directories)) != NULL ||
        data->num_searchers > 0) {

    if (path == NULL) {
      // no dirs to search, but there are still searchers out so we cannot quit
      pthread_mutex_unlock(&mtx);
      struct timespec sleeptime = {0, 500};
      nanosleep(&sleeptime, NULL);
      pthread_mutex_lock(&mtx);
      continue;
    }

    data->num_searchers++;
    pthread_mutex_unlock(&mtx);

    reads++;
    if (search_path(data, path) != 0) {
      perror(path);
      // We don't consider permission denied or missing dir as errors
      // error = 1;
    }

    delete_path(path);
    pthread_mutex_lock(&mtx);
    data->num_searchers--;
  } // End while. No more dirs to search and all threads done.
  // Make sure caller knows if there were errors.
  data->error = error;
  pthread_mutex_unlock(&mtx);
  printf("Thread: %ld Reads: %d\n", pthread_self(), reads);
  return NULL;
}

/**
 * search_path - open and search the directory given by path
 * @param  data -- SearchData (what to search for)
 * @param  path -- path to directory to search
 * @return      0 on successful search, -1 if there were errors
 */
int search_path(SearchData *data, char *path) {
  // Open the directory. If it fails, clean up and continue with the next one.
  DIR *dir = opendir(path);
  int ret = 0;

  if (dir == NULL) {
    ret = -1;
    return ret;
  }
  #ifdef TIME
    // Test how the program would run on a slower CPU
    struct timespec sleeptime = {0, 50000};
    nanosleep(&sleeptime, NULL);
  #endif
  // Check for matches in the dir
  if (search_directory(data, dir, path) != 0) {
    ret = -1;
  }

  if (closedir(dir) != 0) {
    perror("closedir");
  }
  return ret;
}

/**
 * search_directory - check all files and folders in dir for matches and
 * add folders to the list.
 * @param search_data -- data regarding the search
 * @param dir         -- dir to look in
 * @param path        -- path to the dir (used for printing)
 * @return              0 if everything went well, a poitive int otherwise.
 */
int search_directory(SearchData *search_data, DIR *dir, char *path) {
  struct dirent *priv_dirent;
  struct stat f_stat;
  char *file_path = NULL;
  int at_end = 0;
  int ret = 0;

  while (at_end != 1) {
    priv_dirent = malloc(sizeof(struct dirent));
    if (priv_dirent == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    at_end = get_dirent(priv_dirent, dir);
    if (at_end != 0) {
      // Either error or end of dir
      if (at_end == -1) {
        ret++;
      }
      free(priv_dirent);
      continue;
    }

    // Build file path string
    if (asprintf(&file_path, "%s/%s", path, priv_dirent->d_name) == -1) {
      fprintf(stderr, "Error: asprintf failed. Unable to set file_path.\n");
      free(priv_dirent);
      ret++;
      continue;
    }
    // Get stats (file type)
    if (lstat(file_path, &f_stat) != 0) {
      perror(file_path);
      free(priv_dirent);
      free(file_path);
      ret++;
      continue;
    }

    // Print matches.
    process_file(file_path, priv_dirent->d_name, f_stat, search_data);

    // Add directories to the list (not . and ..)
    if (S_ISDIR(f_stat.st_mode)
        && strcmp(priv_dirent->d_name, ".") != 0
        && strcmp(priv_dirent->d_name, "..") != 0) {

      if (add_dir(search_data->directories, file_path) != 1) {
        fprintf(stderr, "Failed to add directory to list.\n");
        return -1;
      }
    }

    free(file_path);
    file_path = NULL;
    free(priv_dirent);
    priv_dirent = NULL;
  }
  return ret;
}

/**
 * get_dirent - copy the next dirent in dir to priv_dirent in a thread safe way.
 * This private dirent is safe to use in a multi thread environment.
 * @param  priv_dirent -- pointer to dirent where the dirent will be saved.
 * @param  dir         -- dir to read from
 * @return             -1 on error, 1 when the last element was read and
 *                        0 otherwise
 */
int get_dirent(struct dirent *priv_dirent, DIR *dir) {
  struct dirent *dirent;
  errno = 0;
  // Lock mutex since readdir isn't thread safe.
  pthread_mutex_lock(&mtx);
  dirent = readdir(dir);
  if (errno != 0) {
    perror("readdir");
    pthread_mutex_unlock(&mtx);
    return -1;
  } else if (dirent == NULL) {
    // No more files to read
    pthread_mutex_unlock(&mtx);
    return 1;
  }
  // Copy dirent to private memory
  memcpy(priv_dirent, dirent, sizeof(struct dirent));
  pthread_mutex_unlock(&mtx);
  return 0;
}

/**
 * process_file - print out matching file.
 * @param file_path -- the path to the file
 * @param name      -- name of the file
 * @param f_stat    -- file stats
 * @param data      -- SearchData (what type and name are we looking for?)
 */
void process_file(char *file_path, char *name,
                  struct stat f_stat, SearchData *data) {
  int match = 0;
  char type = data->type;
  // Is the name matching?
  if (strcmp(name, data->needle) == 0) {
    match = 1;
  }

  // Check type, print if we have a match
  if (S_ISDIR(f_stat.st_mode)) {
    if (match == 1 && (type == 'd' || type == '\0') ) {
      printf("%s\n", file_path);
    }
  } else if (S_ISREG(f_stat.st_mode)) {
    if (match == 1 && (type == 'f' || type == '\0') ) {
      printf("%s\n", file_path);
    }
  } else if (S_ISLNK(f_stat.st_mode)) {
    if (match == 1 && (type == 'l' || type == '\0') ) {
      printf("%s\n", file_path);
    }
  }
}

/**
 * add_dir - add a directory to the list in a thread safe manner
 * @param  list     -- list to add to
 * @param  dir_path -- path to the directory
 * @return          1 if the dir was added, 0 if addition failed.
 */
int add_dir(LinkedList *list, char *dir_path) {
  char *new_dir = malloc(strlen(dir_path)+1);
  if (new_dir == NULL) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }
  strcpy(new_dir, dir_path);

  // Lock mutex and add new_dir to list.
  pthread_mutex_lock(&mtx);
  if (List_append(list, (void *)new_dir) != 1) {
    return 0;
  }
  pthread_mutex_unlock(&mtx);
  return 1;
}

/**
 * check_starting_dirs - check if the starting dirs match the search criterias
 * @param search_data -- data egarding the search
 */
void check_starting_dirs(SearchData *search_data) {
  char *path;
  struct stat f_stat;
  LinkedList *checked_dirs = List_init();

  // Check all starting dirs for matches
  while((path = (char *)List_get(search_data->directories)) != NULL) {
    if (lstat(path, &f_stat) != 0) {
      perror(path);
      continue;
    }
    // Print if there is a match
    process_file(path, basename(path), f_stat, search_data);

    // Add the checked dir to the new list
    char *new_dir = malloc(strlen(path)+1);
    if (new_dir == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
    strcpy(new_dir, path);

    if (List_append(checked_dirs, (void *)new_dir) == 0) {
      fprintf(stderr, "Could not add path to list.\n");
      search_data->error++;
    }
    free(path);
  }
  // Delete the old list
  List_delete(search_data->directories, delete_path);
  // Add the checked dirs
  search_data->directories = checked_dirs;
}

/**
 * print_path - print out a path
 * @param path -- a void pointer to a path string
 */
void print_path(void *path) {
  char *str = (char *)path;
  printf("%s\n", str);
}

/**
 * delete_path - delete and free any memory occupied by path
 * @param path -- path to be freed
 */
void delete_path(void *path) {
  free(path);
}

/**
 * SearchData_delete - free all memory allocated for s_data
 * @param s_data -- SearchData to free
 */
void SearchData_delete(SearchData *s_data) {
  free(s_data->needle);
  List_delete(s_data->directories, delete_path);
  free(s_data);
}
