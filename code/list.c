/**
 * File: list.c
 * Author: Lennart Jern - ens16ljn@cs.umu.se
 *
 * Systemnära programmering, 5dv088 - Laboration 4: mfind
 *
 * A simple implementation of a linked list.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/**
 * List_init
 * Create and initialize a LinkedList.
 *
 * @return  pointer to list
 */
LinkedList *List_init(void) {
    LinkedList *lst = calloc(1, sizeof(LinkedList));
    if (lst == NULL) {
      fprintf(stderr, "Allocation of memory for linked list failed\n");
      exit(EXIT_FAILURE);
    }
    return lst;
}

/**
 * Appends an element vith the specified value to the list
 * @param  l:     List to append to
 * @param  value: Pointer to value to be added
 * @return       1 on success, otherwise 0.
 */
int List_append(LinkedList *l, void *value) {

    Node *new_node = calloc(1, sizeof(Node));
    if (new_node == NULL) {
      fprintf(stderr, "Failed to allocate memory for new node.\n");
      return 0;
    }
    new_node->value = value;
    new_node->next = NULL;

    if (l->first == NULL) {
      l->first = new_node;
      return 1;
    }
    Node *node_ptr = l->first;

    while (node_ptr->next != NULL) {
      node_ptr = node_ptr->next;
    }
    node_ptr->next = new_node;
    return 1;
}

/**
 * List_get - get the value of the first node in the list and delete the node
 * from the list.
 * @param  l -- the list
 * @return   -- pointer to value, remember to free it later.
 */
void *List_get(LinkedList *l) {
  if (l->first == NULL) {
    return NULL;
  }
  Node *node = l->first;
  l->first = l->first->next;
  void *value = node->value;
  free(node);
  node = NULL;
  return value;
}

/**
 * List_sort
 * Sort the list lst by selection sort, using the comparison function comp.
 * @param lst:  LinkedList to sort
 * @param comp: function used to compare the values of two nodes to determine
 *              what order they should be placed in.
 */
void List_sort(LinkedList *lst, int (*comp)(void *value1, void *value2)) {
  if (lst->first == NULL) {
    // Empty list, nothing to do
    return;
  }
  Node *boundary = lst->first; // ordered nodes before this
  Node *smallest = lst->first; // should be placed next in order
  Node *last_sorted = NULL; // add smallest after this one
  Node *current = lst->first;
  Node *prev = NULL;
  Node *before_smallest = NULL;

  // run untill the whole list is sorted
  while (boundary->next != NULL) {
    smallest = boundary;
    current = boundary;
    prev = NULL;
    before_smallest = NULL;
    // loop through the unordered part of the list and pick out the node
    // with the "smallest" value
    while (current->next != NULL) {
      prev = current;
      current = current->next;
      if (comp(smallest->value, current->value) < 0) {
        smallest = current;
        before_smallest = prev;
      }
    }

    // Do we have to move smallest?
    if (before_smallest != NULL) {
      // cut out smallest
      before_smallest->next = smallest->next;
      if (last_sorted == NULL) {
        // place it first
        smallest->next = lst->first;
        lst->first = smallest;
      } else {
        // place it after last_sorted
        smallest->next = last_sorted->next;
        last_sorted->next = smallest;
      }
    }
    // update last_sorted and boundary
    last_sorted = smallest;
    boundary = smallest->next;
  }
}

/**
 * List_remove
 * Removes an element from the bottom of the list and frees the allocated memory
 * by calling delete_value(value).
 *
 * @param  lst: the list to remove from
 * @return     1 on successful removal, 0 if no node was removed.
 */
int List_remove(LinkedList *lst, void (*delete_value)(void *value)) {
  Node *node = lst->first;
  Node *prev = node;
  if (node == NULL) {
    return 0;
  }
  if (node->next == NULL) { // Only one element in list
    delete_value(node->value);
    free(node);
    lst->first = NULL;
    return 1;
  }
  while (node->next != NULL) {
    prev = node;
    node = node->next;
  }
  prev->next = NULL;
  delete_value(node->value);
  free(node);
  return 1;
}

/**
 * List_delete
 * Frees all memory allocated by the nodes in the LinkedList
 * using delete_value(value) and frees the list itself after that.
 *
 * @param lst:          LinkedList to free.
 * @param delete_value: function used to free the memory allocated by the value
 *                      of a node.
 */
void List_delete(LinkedList *lst, void (*delete_value)(void *value)) {
  if (lst) {
      while(List_remove(lst, delete_value));
      free(lst);
  }
}

/**
 * List_print
 * Prints the LinkedList using the function provided.
 *
 * @param lst:   LinkedList to print.
 * @param print: function used to print the node values.
 */
void List_print(LinkedList *lst, void (*print)(void *value)) {
    Node *current = lst->first;

    while (current != NULL) {
      print(current->value);
      current = current->next;
    }
}
