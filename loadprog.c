#include <stdio.h>
#include <stdlib.h>
#include <string.h>       // for strcpy() & strcmp()
#include <errno.h>
#include <ctype.h>        // for toupper()

/* for use demonstration of 'extern' keyword */
extern char *filename;

enum operands {MOVE, STORE, LOAD, INPUT, OUTPUT, ADD, XOR, TEST,
  JUMPEQ, JUMPNEQ, JUMP, EXIT};
enum opcodes {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, CONST, NO_OP};
/* structure of linked list for the lines of instructions coming
 * from the source file */
struct node {
  char instruction[20];
  int length;
  struct node *next;
};
struct args {
  enum opcodes op;
  unsigned char value;
};
// linked list with the instructions parsed
struct inst_line {
  enum operands operand;
  struct args op1;
  struct args op2;
  struct inst_line *next;
};

// function definitions
struct node *loadfile(FILE *prog, int *lines);
struct inst_line *parselines(struct node *firstn);
void parseline(struct inst_line *nodei, struct node *noden);
void free_nodes(struct node *first);
int parse_inst(char *buf, int l);
struct args parse_arg(char *buf, int l);
// delete:
void check(struct inst_line *nodei);


int openfile()
{
  FILE *prog;
  struct node *firstn;
  struct inst_line *firsti;
  int l = 0;
  int *lines;   // in order to not use a global variable
  lines = &l;

  prog = fopen(filename,"r");
  if (!prog)
  {
    printf("\nError loading file. Please try again.\n");
    return (1);
  }
  firstn = loadfile(prog, lines);  // load lines into a the linked list
  fclose(prog);
  firsti = parselines(firstn);   // extract and restructure of data from lines
  printf("Dream on!");
  check(firsti);
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
  newn = firstn;
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
      // if error then exit
      if (err){
        printf("\nError while loading the source:");
        printf("\n==> %d\n",err);
        free(buff);
        free_nodes(firstn);
        fclose(prog);
        exit (2);
      }
      // no error, EOF (errno = 0)
      /* previously memory allocated for new node, in the previous iteration
       * of the loop, won't be used and is therefore freed */
      free(newn);
      free(buff);
      currentn->next = NULL;
      // skipping the rest of the loop
      loop = 1;
      continue;
    }
    // skipping empty lines
    if (line == 1)
      continue;
    printf("\n==>%s - %d",buff,line);
    /* getting rid of '\n' at the end of the line/string */
    buff[line-1] = '\0';
    currentn = newn;
    /* storing the buffer in the current node of the list
     * as well as its length
     * then allocating a new node for the next line */
    strcpy(currentn->instruction,buff);
    currentn->length = line;    // usefull for parsing the inst/args
    newn = (struct node *)malloc(sizeof(struct node));
    /* now a new node has been allocated memory,
     * the current node next pointer can be initialised to point to it,
     * and the index/current node can be moved to the newly memory
     * allocated node, for the next iteration of the loop */
    currentn->next = newn;
    (*lines)++;  // keeping track of the number of lines
  }
  while(!loop);
  return(firstn);
}

/* linked list of lines => linked list of structured data
 * (extracting the instruction and the optional arguments
 * in order to reorganise them to put them in another linked list) */
struct inst_line *parselines(struct node *firstn)
{
  struct node *currentn;
  struct inst_line *firsti,*indexi, *newi;
  int loop = 0;

  /* setting up a new linked list with the instruction
   * and the optional arguments */
  firsti = (struct inst_line *)malloc(sizeof(struct inst_line));
  newi = firsti;
  /* looping through each line to parse for instructions and args */
  do
  {
    indexi = newi;
    currentn = firstn;
    parseline(indexi, currentn);
    // checking for the end of the list (to end the loop)
    if (currentn->next == NULL)
    {
      printf("\nlast %s.%d\n",currentn->instruction,indexi->operand);
      indexi->next = NULL;
      free(currentn);
      loop = 1;
      return (firsti); // instead of continue???
    }
    else    // if not the end then getting the next line to process
      firstn = currentn->next;
    free(currentn);     // the line for the source is not needed anymore
    /* allocating a new struct to store instruction & args from
     * the next iteration of the loop */
    newi = (struct inst_line *)malloc(sizeof(struct inst_line));
    indexi->next = newi;
  }
  while(!loop);

  return (firsti);
}

void parseline(struct inst_line *nodei, struct node *noden)
{
  char buff[10];    // buffer for instruction or argument
  char *line;       // line to be parsed
  int length, flag = 0;
  int c, w=0, wc=0;
  enum operands op;
  struct args a1, a2;
  a1.op = NO_OP;
  a1.value = 0;
  a2.op = NO_OP;
  a2.value = 0;


  line = noden->instruction;
  printf("\n%s<--",line);
  length = noden->length;
  // iterating through each character of the line
  for (c=0;c<length;c++)
  {
    printf("\nc:%d<<-%c",c,line[c]);
    /* checking for space and comma separator and safeguarding against multi spaces
     * as well as lines without args or space (think EXIT) */
    if ((line[c] == ' ' || line[c] == '\0' || line[c] == ',') && flag == 1)
    {
      switch (w)
      {
        case 0:
        printf("\n0=line:<%s>buff:%s",line,buff);
        op = parse_inst(buff,c-1);
        flag = 0;
        break;
        case 1:
        printf("\n1_ c=%d buff:%s",c,buff);
        a1 = parse_arg(buff,c-wc-1);
        flag = 0;
        break;
        case 2:
        printf("\n2_ c=%d buff:%s",c,buff);
        a2 = parse_arg(buff,c-wc-1);
        break;
        default:
        // NOP
        break;
      };
      strcpy(buff,"---------");    // buffer reinit for next argument
      w++;          // position of instruction/argument in the line
    }
    // no need to store commas or space in the buffer
    if ((line[c] != ',') && (line[c] != ' '))
    {
        buff[c-wc] = line[c];
        flag = 1;
    }
    else
      wc = c + 1;   // # of characters to substract for next instruct/arg

  }
//  printf("\nline:%s_op:%d",line,op);
  nodei->operand = op;
  nodei->op1 = a1;
  nodei->op2 = a2;
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
    printf("_EXIT_\n");
    break;
    case 'I': ret = INPUT;
    break;
    case 'J':
      if (l == 3)         // jump (4 chras -1)
      {
        ret = JUMP;
      }
      else if (l == 5)    // jumpeq (6 chars -1)
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

struct args parse_arg(char *buf, int l)
{
  struct args ret;

  printf("\narg buf: %s\n",buf);

  if ((*buf == 'R' || *buf == 'r') && !(*(buf + 1)<'0' || *(buf + 1)>'9'))
  {
    ret.op = *(buf + 1) - '0';
    ret.value = 0;
    return(ret);
  }
  if (*buf <'0' ||  *buf>'9')
  {
      printf("Parsing error - Invalid argument.");
      exit(4);
  }
  ret.op = CONST;
  // test if the third character is a number, then test the second one
  if (!(*(buf + 2)<'0' || *(buf + 2)>'9'))
    ret.value = (*buf - '0')*100 + (*(buf+1) - '0')*10 + (*(buf+2) - '0');
  else
  {
    if (!(*(buf + 1)<'0' || *(buf + 1)>'9'))
      ret.value = (*buf - '0') * 10 + (*(buf + 1) - '0');
    else
      ret.value = *(buf) - '0';
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






void check(struct inst_line *nodei)
{
  struct inst_line *firsti;
  int loop = 0;

  do
  {
    switch (nodei->operand)
    {
      case 0:
      printf("\nMOVE ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 1:
      printf("STORE ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 2:
      printf("LOAD ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 3:
      printf("INPUT ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 4:
      printf("OUTPUT ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 5:
      printf("ADD ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 6:
      printf("XOR ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 7:
      printf("TEST ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 8:
      printf("JUMPEQ ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 9:
      printf("JUMPNEQ ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 10:
      printf("JUMP ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;
      case 11:
      printf("EXIT ");
      printf("1: %d - %d / 2: %d - %d\n",
      nodei->op1.op,nodei->op1.value,nodei->op2.op,nodei->op2.value);
      break;

      default:
      printf("\n==> %d",nodei->operand);
      printf("\ndefault\n");
    };

    if (nodei->next == NULL)
      return;
    firsti = nodei->next;
    nodei = firsti;

  }
  while(!loop);

}

