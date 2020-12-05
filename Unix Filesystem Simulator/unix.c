#include "unix.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Name: William Chung
 * ID: Wchung1
 * UID: 116609968
 * Discusion Number:0101
 * 
 * This program represents a filesystem.
 * The file includes functions that use the
 * data structure that repesents the system
 * to simulate its commands of making components
 * and printing information.
 */

/*Initializes the filesystem
  by creating the root directory in it*/
void mkfs(Unix *filesystem) {

  Node *new_node=malloc(sizeof(Node));

  if (new_node == NULL)
    printf("Not enough memory");
  else {

    /*a root will print "/" when pwd is called on it*/
    new_node->name=malloc(sizeof(char) * 2);
    strcpy(new_node->name, "/");
    
    if (new_node->name == NULL) {
      printf("Not enough memory");
      return;
    }
     
    /*a root's parent is itself*/
    new_node->parent=new_node;
    new_node->child=NULL;
    new_node->next=NULL;
    new_node->is_dir=1;
    filesystem->current_dir=new_node;
  }
  
}

/*Makes a new node in the directory*/
static int make_new_node(Unix *filesystem, const char arg[], int is_dir,
			 int not_err) {

  Node* current;
  Node* new_node;

  current=filesystem->current_dir->child;

  /*if nothing's in the directory, makes the first child*/
  if (current == NULL) {
    filesystem->current_dir->child=malloc(sizeof(Node));    
    new_node = filesystem->current_dir->child;
  }
  else {

    /*returns a value if it's already in the directory*/
    if (strcmp (current->name, arg) == 0)
      return not_err;

    /*inserts first if it's first alphabetically*/
    if (strcmp (current->name, arg) > 0) {
      new_node=malloc(sizeof(Node));

      if (new_node == NULL) {
	printf("Not enough memory");
	return 0;
      }

     
      new_node->name=calloc(strlen(arg) + 1, sizeof(char));

      if (new_node->name == NULL) {
	printf ("Not enoguh memory");
	return 0;
      }
      
      strcpy(new_node->name, arg) ;
      new_node->next=filesystem->current_dir->child;
      new_node->child=NULL;
      new_node->is_dir=is_dir;
      new_node->parent=filesystem->current_dir;
      filesystem->current_dir->child=new_node;
      return 1;
    }

    /*goes through each component till the last*/
    while (current->next != NULL) {

      if (strcmp(current->next->name, arg) == 0) {
	return not_err;
      }

      /*inserts in the middle if it needs to be inserted*/
      if (strcmp(current->next->name, arg) > 0) {
	new_node=malloc(sizeof(Node));

	if (new_node == NULL) {
	  printf("Not enough memory");
	  return 0;
	}
	
	new_node->name=calloc(strlen(arg) + 1, sizeof(char));
	if (new_node->name == NULL) {
	  printf ("Not enough memory");
	  return 0;
	}
	
	strcpy(new_node->name, arg) ;
	new_node->next=current->next;
	new_node->child=NULL;
	current->next=new_node;
	new_node->is_dir=is_dir;
	new_node->parent=filesystem->current_dir;
	return 1;
      }
      
      current = current->next;
    }

    /*make a new last node in the directory*/
    current->next=malloc(sizeof(Node));
    new_node=current->next;
  }

  /*checks if there is enough memory*/
  if (new_node == NULL) {
    printf("Not enough memory");
    return 0;
  }

  /*makes the link to make this node the last node in the directory*/
  new_node->name=calloc(strlen(arg) + 1, sizeof(char));

  if (new_node->name == NULL) {
    printf("Not enough memory");
    return 0;
  }
  strcpy(new_node->name, arg) ;
  new_node->next=NULL;
  new_node->child=NULL;
  new_node->is_dir=is_dir;
  new_node->parent=filesystem->current_dir;
  return 1;
  
}

/*makes a new file in the directory*/
int touch(Unix *filesystem, const char arg[]) {
  if (filesystem == NULL || arg == NULL)
    return 0;

  if (strcmp(arg,".") == 0 || strcmp (arg, "..") == 0
      || strcmp(arg, "/") ==0)
    return 1;

  /* these are the only specific error cases*/
  if ( (strchr(arg, '/') != NULL) || strcmp(arg, "") == 0 )
    return 0;

  /*makes a new file*/
  return make_new_node (filesystem, arg, 0, 1);
}

/*makes a new subdirectory in the current directory*/
int mkdir(Unix *filesystem, const char arg[]) {

  if (filesystem == NULL || arg == NULL)
    return 0;

  /*these are specific error cases*/
  if (strcmp(arg,".") == 0 || strcmp (arg, "..") == 0
      ||  strcmp(arg, "") == 0 || strchr(arg, '/') != NULL)
    return 0;

  /*Makes a new directory node*/
  return make_new_node (filesystem, arg, 1, 0);
  
}

/* Changes the current directory in the unix parameter*/
int cd(Unix *filesystem, const char arg[]) {

  Node *current;

  if (filesystem == NULL || arg == NULL)
    return 0;
  /* It does nothing if arg is "."*/
  if (strcmp(arg,".") != 0){ 

    /* Changes the current directory's parent*/
    if (strcmp(arg,"..") == 0) {
      filesystem->current_dir = (filesystem->current_dir->parent);
    }

    /* the arg "/" changes to the root directory */
    else if (strcmp(arg, "/") == 0) {

      /*The root is the  only node  where its "parent" is itself*/
      while (filesystem->current_dir != filesystem->current_dir->parent) 
	filesystem->current_dir=filesystem->current_dir->parent;
    }

    else{

      /*switches to subdirectory in this directory*/
      current=filesystem->current_dir->child;

      /* can't switch if the directory has nothing*/
      if (current == NULL) {
	return 0;
      }
      
      else {

	/* keeps looking in the sorted nodes till it shouldn't*/
	while ( ( current != NULL) && strcmp(current->name, arg) <= 0 ) {

	  /*only changes if it found the name and it is a directory*/
	  if (strcmp(current->name, arg) == 0) {
	    
	    if (current->is_dir) {
	      filesystem->current_dir=current;
	      return 1;
	    }
	    else {
	      return 0;
	    }
	
	  }
	  
	  current = current->next;
	}

	/*if it can't find the directory, it can't change*/
	return 0;
      }

    }

   
  }

  /*changed directory for the special arguments is true*/
  return 1;
}

/*prints content of the directory*/
int ls(Unix *filesystem, const char arg[]) {

  Node* directory;
  Node* current;
 
  if (filesystem == NULL || arg == NULL)
    return 0;

  /*prints all contents of current directory*/
  if (strcmp(arg, "") == 0 || strcmp(arg, ".") == 0)
    directory = filesystem->current_dir;
  /*prints all contents of current directory's parents*/
  else if (strcmp(arg, "..") == 0) {
    directory=filesystem->current_dir->parent;
  }

  /*prints all contents of the root directory*/
  else if (strcmp(arg, "/") == 0) {

    directory = filesystem->current_dir;

    /*only the root directory has itself as its parent*/
     while (directory != directory->parent) 
	directory=directory->parent;
     
  }
  else {

    directory = filesystem->current_dir->child;

    /*goes through all nodes till it founds it or it does not need to*/
    while ((directory != NULL) && strcmp(directory->name, arg) < 0) {
      directory=directory->next;	  
    }

    /*if it could not find it, it can't list anything*/
    if (directory == NULL || strcmp(directory->name, arg) >  0)
      return 0;

    /*if the thing it found was a fie, it prints out the file only.*/
    if(!directory->is_dir) {
      printf("%s\n", directory->name);
      return 1;
    }
    
  }
	 
  current = directory->child;

  /*prints everything in the directory alphabetically*/
  while (current != NULL) {
    printf("%s", current->name);

    if (current->is_dir) {
      printf("/");
    }

    printf("\n");
    current = current->next;
  }
  
  return 1;
}

/*prints the name of the current directory*/
void pwd(Unix *filesystem) {

  /*A root will still print "/" because it is its name*/
  if (filesystem != NULL) {
    printf(filesystem->current_dir->name); 
    printf("\n");
  }
  
}
