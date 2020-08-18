#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for use demonstration of 'extern' keyword */
extern char *filename;

int openfile()
{
  FILE *prog;
  int loop = 1;
  int lines=0;
  char *buff;
  size_t length = 20;
  size_t line;
  struct node {
    char instruction[20];
    struct node *next;
  };

  struct node *firstn, *currentn, *newn;    // start of linked list
  firstn = (struct node *)malloc(sizeof(struct node));
  currentn = firstn;
  buff = (char *)malloc(length * sizeof(char *));
  prog = fopen(filename,"r");
  if (!prog)
  {
    printf("\nError loading file. Please try again.\n");
    return (1);
  }
  do
  {
    line = getline(&buff,&length,prog);
    if (line == -1)
    {
      printf("\n<<%d>>\n",line);
      loop = 0;
      continue;
    }
//    buf = *buff;
    /* getting rid of '\n' at the end of line... or EOF */
    buff[line-1] = '\0';
    /* ... if so then the last character has to be '\n',
     * if so then the whole file has been processed and
     * the is the last line to handle, and the loop can stop */
//    buf = buff;
    strcpy(currentn->instruction,buff);
    newn = (struct node *)malloc(sizeof(struct node));
    currentn->next = newn;
    currentn = newn;
    lines++;  // keeping track of the number of lines

    printf("\n %s .%d",buff,lines);
  }
  while(loop);


  fclose(prog);
  return (0);
}


