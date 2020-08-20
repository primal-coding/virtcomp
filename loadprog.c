#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>        // for toupper()

/* for use demonstration of 'extern' keyword */
extern char *filename;

enum operands {MOVE, STORE, LOAD, INPUT, OUTPUT, ADD, XOR, TEST,
  JUMPEQ, JUMPNEQ, JUMP, EXIT};
enum opcodes {CONST, R1, R2, R3, R4, R5, R6, R7, R8, R9, R0, NOOP};
/* structure of linked list for the lines of instructions coming
 * from the source file */
struct node {
  char instruction[20];
  int length;
  struct node *next;
};
// linked list with the instructions parsed
struct inst_line {
  unsigned char operand;
  unsigned char opcode1;
  unsigned char opcode2;
  struct inst *next;
};

// function definitions
struct node *loadfile(FILE *prog, int *lines);
void parselines(struct node *firstn);
void parseline(char *line, int length);
void free_nodes(struct node *first);
int parse_inst(char *buf, int l);
int parse_arg(char *buf, int l);


int openfile()
{
  FILE *prog;
  struct node *firstn;
  int l = 0;
  int *lines;   // in order to not use a global variable
  lines = &l;

  prog = fopen(filename,"r");
  if (!prog)
  {
    printf("\nError loading file. Please try again.\n");
    return (1);
  }
  firstn = loadfile(prog, lines);  // creation of the linked list
  fclose(prog);
  parselines(firstn);

  return (0);
}

/* getting each line from the source file and storing them
 * in a linked list after they have been stripped from '\n'*/
struct node *loadfile(FILE *prog,int *lines)
{
  int err;
  int loop = 0;
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
    /* testing for the end of the file and error */
    if (line == -1)
    {
      err = errno;
      if (err){
        printf("\nError while loading the source:");
        printf("\n==> %d\n",err);
        free(buff);
        free_nodes(firstn);
        fclose(prog);
        exit (2);
      }
      /* previously memory allocated for new node, in the previous iteration
       * of the loop, won't be used and is therefore freed */
      free(newn);   // currentn = newn
      free(buff);
      // skipping the rest of the loop
      loop = 1;
      continue;
    }
    // skipping empty lines
    if (line == 1)
      continue;
    /* getting rid of '\n' at the end of the line/string */
    buff[line-1] = '\0';
    /* storing the buffer in the current node of the list
     * as well as its length
     * then allocating a new node for the next line */
    strcpy(currentn->instruction,buff);
    currentn->length = line-1;
    newn = (struct node *)malloc(sizeof(struct node));
    /* now a new node has been allocated memory,
     * the current node next pointer can be initialised to point to it,
     * and the index/current node can be moved to the newly memory
     * allocated node, for the next iteration of the loop */
    currentn->next = newn;
    currentn = newn;
    (*lines)++;  // keeping track of the number of lines
  }
  while(!loop);
  return(firstn);
}

void parselines(struct node *firstn)
{
  struct node *indexn;
  int loop = 0;
  char *buff;

  do
  {
    indexn = firstn;
    buff = indexn->instruction;
    parseline(buff,indexn->length);
    if (indexn->next == NULL)
      loop = 1;
    else
      firstn = indexn->next;
    free(indexn);
  }
  while(!loop);

}

void parseline(char *line, int length)
{
  char buff[10];
  struct inst_line *firsti,*indexi, *newi;
  enum operands op;
  enum opcodes op1, op2;
  int c, w=0, wc=0;

  // iterating through each character of the line
  for (c=0;c<length-1;c++)
  {
    if (line[c] == ' ')   // checking for separator
    {
      switch (w)
      {
        case 0:
        op = parse_inst(buff,c-1);
        break;
        case 1:
        op1 = parse_arg(buff,c-wc-1);
        break;
        case 2:
        op2 = parse_arg(buff,c-wc-1);
        break;
        default:
        // NOP
        break;
      };
      w++;      // position of instruction/argument in the line
      wc = c;   // # of characters to substract for next instruct/arg
    }
    if (line[c] != ',')
      buff[c-wc] = line[c];


  }
}


int parse_inst(char *buf, int l)
{
  enum operands ret;
  char c;
  c = toupper(*buf);

  switch (c)
  {
    case 'A': ret = ADD;
    break;
    case 'E': ret = EXIT;
    break;
    case 'I': ret = INPUT;
    break;
    case 'J':
      if (l == 4)         // jump
      {
        ret = JUMP;
      }
      else if (l == 6)    // jumpeq
      {
         ret = JUMPEQ;
      }
      else                // jumpneq
      {
         ret = JUMPNEQ;
      }
    break;
    case 'L': ret = LOAD;
    break;
    case 'M': ret = MOVE;
    break;
    case 'O': ret = OUTPUT;
    break;
    case 'S': ret = STORE;
    break;
    case 'T': ret = TEST;
    break;
    case 'X': ret = XOR;
    break;
    default:
      printf("Parsing error - Invalid instruction.");
      exit(3);
    break;
  };
  return (ret);
}

int parse_arg(char *buf, int l)
{
  enum opcodes ret;
  char b[10];
  char *bb;
  bb = b;
  char c;
  c = toupper(*buf);

  if (l == 8 && c == 'C')
  {
    bb = buf;
    b = bb;
    b[8] = '\0';
    if (strcmp(b,"CONSTANT") == 0)
      return (CONST);
    else
    {
      printf("Parsing error - Invalid argument.");
      exit(4);
    }
  }


  return(ret);
}

// free the memory allocated for the linked list of lines
void free_nodes(struct node *first){
  struct node *temp;

  do{
    temp = first->next;
    free(first);
    first = temp;
  }
  while(first->next != NULL);
  free(first);
}
