#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for use demonstration of 'extern' keyword */
extern char *filename;

int openfile()
{
  FILE *prog;
  int lines=0;
  char buf[256];
  struct node {
    char *instruction;
    struct node *next;
  };

  struct node *firstn, *currentn, *newn;    // start of linked list
  firstn = (struct node *)malloc(sizeof(struct node));

  prog = fopen(filename,"r");
  if (!prog)
  {
    printf("\nError loading file. Please try again.\n");
    return (1);
  }
  fgets(buf,256,prog);
  lines++;
  buf[strlen(buf)-1] = '\0';
  strcpy(firstn->instruction,buf);

  //do
  //{



  //}
  //while(*buf != EOF);

  fclose(prog);
  return (0);
}


