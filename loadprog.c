#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for use demonstration of 'extern' keyword */
extern char *filename;

struct node {               // structure of linked list node
  char instruction[20];
  struct node *next;
};

struct node *loadfile(FILE *prog);

int openfile()
{
  FILE *prog;
  struct node *firstn;

  prog = fopen(filename,"r");
  if (!prog)
  {
    printf("\nError loading file. Please try again.\n");
    return (1);
  }
  firstn = loadfile(prog);
  fclose(prog);


  return (0);
}

struct node *loadfile(FILE *prog)
{
  int loop = 0;
  int lines=0;
  char *buff;             // buffer for each line retrieved from the file
  size_t length = 20;         // size of buffer
  size_t line;                // size of line retrieved including \n
  /* three nodes are required in order to create a linked list
   * the first one, an index(current), and a new one which will be allocated
   * memory 'on demand' at each iteration of the loop retrieving the lines
   * of the file to process*/
  struct node *firstn,*currentn, *newn;
  firstn = (struct node *)malloc(sizeof(struct node));
  currentn = firstn;
  buff = (char *)malloc(length * sizeof(char *));

    do
  {
    /* using getline() as recommended by:
     * https://www.gnu.org/software/libc/manual/html_node/Line-Input.html#Line-Input
     * as 'This function is a GNU extension, but it is the recommended
     * way to read lines from a stream. The alternative standard functions
     * are unreliable.'
     * even though this isn't the 'K&R/C89 way' of doing it
     * */
    line = getline(&buff,&length,prog);
    /* testing for the end of the file */
    if (line == -1)
    {
      /* previously memory allocated for new node, in the previous iteration
       * of the loop, won't be used and is therefore freed */
      free(newn);
      // skipping the rest of the loop
      loop = 1;
      continue;
    }
    /* getting rid of '\n' at the end of the line/string */
    buff[line-1] = '\0';
    /* storing the buffer in the current node of the list
     * then allocating a new node for the next line */
    strcpy(currentn->instruction,buff);
    newn = (struct node *)malloc(sizeof(struct node));
    /* now a new node has been allocated memory, the current node can
     * be initialised to point to it, and the index/current node can be
     * moved to the newly memory allocated node,
     * for the next iteration of the loop */
    currentn->next = newn;
    currentn = newn;
    lines++;  // keeping track of the number of lines
  }
  while(!loop);

  return(firstn);
}
