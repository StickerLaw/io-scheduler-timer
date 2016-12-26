/**
 * File: list.h
 * Author: Lennart Jern - ens16ljn@cs.umu.se
 *
 * Systemnära programmering, 5dv088 - Laboration 4: mfind
 *
 * This is a header file for my own implementation of a liked list.
 *
 */

typedef struct linked_list LinkedList;
typedef struct node Node;

struct node {
  void *value;
  struct node *next;
};

struct linked_list {
  Node *first;
};

struct user_info {
  unsigned int uid;
  char *uname;
};

/**
 * List_init
 * Create and initialize a LinkedList.
 *
 * @return  pointer to list
 */
LinkedList * List_init(void);

/**
 * Appends an element vith the specified value to the list
 * @param  l:     List to append to
 * @param  value: Pointer to value to be added
 * @return       1 on success, otherwise 0.
 */
int List_append(LinkedList *l, void *value);

/**
 * List_get - get the first element from the list. The element is removed
 * from the list so remember to free it when you are done.
 * @param  l -- the list
 * @return   -- a pointer to the first value in the list
 */
void *List_get(LinkedList *l);

/**
 * List_sort
 * Sort the list lst by selection sort, using the comparison function comp.
 * @param lst:  LinkedList to sort
 * @param comp: function used to compare the values of two nodes to determine
 *              what order they should be placed in.
 */
void List_sort(LinkedList *lst, int (*comp)(void *value1, void *value2));

/**
 * List_remove
 * Removes an element from the bottom of the list and frees the allocated memory
 * by calling delete_value(value).
 *
 * @param  lst: the list to remove from
 * @return     1 on successful removal, 0 if no node was removed.
 */
int List_remove(LinkedList *lst, void (*delete_value)(void *value));

/**
 * List_delete
 * Frees all memory allocated by the nodes in the LinkedList
 * using delete_value(value) and frees the list itself after that.
 *
 * @param lst:          LinkedList to free.
 * @param delete_value: function used to free the memory allocated by the value
 *                      of a node.
 */
void List_delete(LinkedList *lst, void (*delete_value)(void *value));

/**
 * List_print
 * Prints the LinkedList using the function provided.
 *
 * @param lst:   LinkedList to print.
 * @param print: function used to print the node values.
 */
void List_print(LinkedList *lst, void (*print)(void *value));
