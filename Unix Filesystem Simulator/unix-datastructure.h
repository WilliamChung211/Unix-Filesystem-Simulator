/*
 *Name: William Chung
 *
 * * Name: William Chung
 * ID: Wchung1
 * UID: 116609968
 * Discusion Number:0101
 * 
 * This includes the structures of 
 * Node and Unix. Unix represents a file
 * system by having nodes that keep track
 * of their name, if it's a directory, parent
 * directory, and neighboring directory based 
 * in alphabetical order. Unix structure keeps
 * track of current directory.
 */

typedef struct node{
  char* name;
  int is_dir;
  struct node *child;
  struct node *parent;
  struct node *next;
} Node;

typedef struct {
  Node *current_dir;
} Unix;
