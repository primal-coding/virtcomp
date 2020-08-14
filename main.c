#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int c = 0, flush;
  char *hardcoded = "comp_prog.bin";

  printf("Please select a loading option:\n");
  printf("1_Hardcoded file\n");
  printf("2_Enter a file\n");
  printf("3_Command line parameter file\n");
  printf("=> ");

  while (!c){
    c = getchar();
    flush = c;
    while(flush != '\n' && c != EOF){
      flush = getchar();
      switch (c){
        case '1':
        printf("1_Harcoded file %s selected",hardcoded);
        break;
        case '2':
        printf("2");
        break;
        case 3:
        printf("3");
        break;
        default:
        printf("Invalid answer. Enter 1,2 or 3.");
        c = 0;
      }
    }
  }
  printf("Out");

  return (0);
}
