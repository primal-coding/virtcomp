#include <stdio.h>
#include <stdlib.h>
#include "loadprog.c"

char *filename;

void main_selection(int, char**);
void rt(void);

int main(int argc, char *argv[])
{
  filename = "comp_prog.bin";

  printf("Please select a loading option:\n");
  printf("1_Hardcoded file\n");
  printf("2_Enter a file\n");
  printf("3_Command line parameter file\n");
  printf("=> ");

  main_selection(argc, argv);
  if (!openfile())
    return(0);



  return (0);
}


/* main menu selection function
 * argc and argv are passed to check their validity for the third option
 * returns the selected number after having given a user feedback */
void main_selection(int argc, char *argv[])
{
  int c = 0, flush;

/* input loop based on
 * gnu libc manual
 * http://www.gnu.org/software/libc/manual/html_node/Character-Input.html
 */
  while (!c){
    c = getchar();
    flush = c;
    while(flush != '\n' && c != EOF){
      flush = getchar();
    }
    switch (c)
    {
      case '1':
      c = 1;
      printf("1_Harcoded file \'%s\' selected.\n",filename);
      break;

      case '2':
      c = 2;
      rt();
      printf("2_Runtime input file \'%s\' selected.\n",filename);
      break;

      case '3':
      if (argc == 1){
        printf("\nNo parameter from the command line provided.\n!");
        printf("Make another selection.\n==> ");
        c = 0;
        break;
      }
      c = 3;
      filename = argv[1]; // extra arguments after are ignored
      printf("3_Command line file \'%s\' selected.\n",filename);
      break;

      default:
      printf("\nInvalid answer. Enter 1,2 or 3.");
      c = 0;
    }
    if (c > 0 && c < 4)
      break;
  }
}

/* runtime input - selection 2
 */
void rt(){
  int c, i = 0, loop = 1;
  char file[256];

  printf("2_Enter file name:\n-->");
  while (loop)
  {
    c = getchar();
    while(c != '\n' && c != EOF){
      file[i] = c;
      i++;
      c = getchar();
    }
    if (i == 0)   // no entry
      printf("\nInvalid entry.\nPlease try again -->:");
    else loop = 0;
  }
  file[i] = '\0';
  filename = file;
}

