#include <stdio.h>


enum operands {MOVE, STORE, LOAD, INPUT, OUTPUT, ADD, XOR, TEST,
  JUMPEQ, JUMPNEQ, JUMP, EXIT};
enum opcodes {R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, CONST, NO_OP};

struct args {
  enum opcodes op;
  unsigned char value;
};
struct inst_line {
  enum operands operand;
  struct args op1;
  struct args op2;
  struct inst_line *next;
};
struct computer {
  // code memory
  // data memory
  // registers
  // compare flag
  // pc
}

void compute(struct inst_line *firsti){

}
