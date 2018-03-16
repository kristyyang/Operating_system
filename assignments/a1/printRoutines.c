#include "printRoutines.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// You probably want to create a number of printing routines in this file.
// Put the prototypes in printRoutines.h

/*********************************************************************
   Details on print formatting can be found by reading the man page
   for printf. The formatting rules for the disassembler output are
   given below.  Basically it should be the case that if you take the
   output from your disassembler, remove the initial address
   information, and then take the resulting file and load it into a
   Y86-64 simulator the object code produced be the simulator should
   match what your program read.  (You may have to use a .pos
   directive to indicate the offset to where the code starts.) If the
   simulator reports an assembler error then you need to fix the
   output of your dissassembler so that it is acceptable to the
   simulator.)

   The printing rules are as follows:

     1) Each line is to begin with the hex printed value of the
        address followed immediately by a ": ". Leading zeros are to
        be printed for the address, which is 8 bytes long.

     2) After the ": " there are to be 22 characters before the start
        of the printing of the instruction mnemonic. In those 22
        characters the program is to print, left justified, the hex
        representation of the memory values corresponding to the
        assembler instruction and operands that were printed.

     3) The instruction field follows, and is 8 characters long. The
        instruction is to be printed left justified in those 8
        characters (%-8s). All instructions are to be printed in lower
        case.

     4) After the instruction field the first operand is to be
        printed. No extra space is needed, since the 8 characters for
        instructions is enough to leave a space between instruction
        and operands.

     5) If there is a second operand then there is to be a comma
        immediately after the first operand (no spaces) and then a
        single space followed by the second operand.

     6) The rules for printing operands are as follows:

         a) Registers: A register is to be printed with the % sign and
            then its name. (.e.g. %rsp, %rax etc) Register names are
            printed in lower case.

         b) All numbers are to be printed in hex with the appropriate
            leading 0x. Leading zeros are to be suppressed. A value of
            zero is to be printed as "0x0". The numbers are assumed to
            be unsigned.

         c) A base displacement address is to be printed as D(reg)
            where the printing of D follows the rules in (b), and the
            printing of reg follows the rules in (a). Note there are
            no spaces between the D and "(" or between reg and the "("
            or ")".

         d) An address such as that used by a call or jump is to be
            printed as in (b). For these instructions no "$" is
            required.

         e) A constant (immediate value), such as that used by irmovq
            is to be printed as a number in (b) but with a "$"
            immediately preceeding the 0x without any spaces.

     7) The unconditional move instruction is to be printed as rrmovq,
        while the conditional move is to be printed as cmovXX, where
        XX is the condition (e.g., cmovle).

     8) The mnemonics for the instruction are to conform to those
        described in the textbook and lecture slides.

     9) The arguments for the format string in the example printing
        are strictly for illustrating the spacing. You are free to
        construct the output however you want.

********************************************************************************/

/* This is a function to demonstrate how to do print formatting. It
 * takes the file descriptor the I/O is to be done to. You are not
 * required to use the same type of printf formatting, but you must
 * produce the same result.
 */
int samplePrint(FILE* out) {
  int res = 0;

  unsigned long addr = 0x1016;
  char* r1 = "%rax";
  char* r2 = "%rdx";
  char* inst1 = "rrmovq";
  char* inst2 = "jne";
  char* inst3 = "irmovq";
  char* inst4 = "mrmovq";
  unsigned long destAddr = 8193;

  res += fprintf(out, "%016lx: %-22s%-8s%s, %s\n", addr, "2002", inst1, r1, r2);

  addr += 2;
  res += fprintf(out, "%016lx: %-22s%-8s%#lx\n", addr, "740120000000000000",
                 inst2, destAddr);

  addr += 9;
  res += fprintf(out, "%016lx: %-22s%-8s$%#lx, %s\n", addr,
                 "30F21000000000000000", inst3, 16L, r2);

  addr += 10;
  res += fprintf(out, "%016lx: %-22s%-8s%#lx(%s), %s\n", addr,
                 "50020000010000000000", inst4, 65536L, r2, r1);

  addr += 10;
  res = fprintf(out, "%016lx: %-22s%-8s%s, %s\n", addr, "2020", inst1, r2, r1);

  return res;
}

typedef unsigned char byte;

int Println(FILE* out, long addr, byte* ins) {
  // int res = 0;
  // byte opcode = ins[0];
  // byte ra = 0;
  // byte rb = 0;
  // unsigned long valC = 0;
  // unsigned long dstAddr = 0;
  // char* reg[15] = {"%rax", "%rcx", "%rdx", "%rbx", "%rsp",
  //                  "%rbp", "%rsi", "%rdi", "%r8",  "%r9",
  //                  "%r10", "%r11", "%r12", "%r13", "%r14"};
  // char* ops[7] = {"addl", "subl", "andl", "xorl", "mull", "divl", "modl"};
  // char* js[7] = {"jmp", "jle", "jl", "je", "jne", "jge", "jg"};
  // char* movs[7] = {"rrmovq", "cmovle",  "cmovl", "cmove",
  //                  "cmovge", "cmovege", "cmovg"};
  int res = 0;
  byte opcode = ins[0];
  byte ra = 0;
  byte rb = 0;
  long valC = 0;
  long dstAddr = 0;

  char* regs[15] = {"%rax", "%rcx", "%rdx", "%rbx", "%rsp",
                    "%rbp", "%rsi", "%rdi", "%r8",  "%r9",
                    "%r10", "%r11", "%r12", "%r13", "%r14"};
  char* movIns[7] = {"rrmovq", "cmovle", "cmovl", "cmove",
                     "cmovne", "cmovge", "cmovg"};
  char* opIns[7] = {"addq", "subq", "andq", "xorq", "mulq", "divq", "modq"};
  char* jmpIns[7] = {"jmp", "jle", "jl", "je", "jne", "jge", "jg"};

  switch (opcode) {
    case 0x30:
    case 0x40:
    case 0x50:
      ra = ins[1] >> 4;
      rb = ins[1] & 0xf;
      if (opcode == 0x30) {
        if (ra != 0xf || rb == 0xf) {
          printf("# Invalid instruction encountered.\n");
          printf("0x30");
          exit(PRINTERROR);
        }
      } else {
        if (ra == 0xf || rb == 0xf) {
          printf("# Invalid instruction encountered.\n");
          printf("0x31");
          exit(PRINTERROR);
        }
      }
      for (int i = 9; i >= 2; i--) {
        valC += ins[i];
        if (i != 2) {
          valC = valC << 8;
        }
      }
      break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x80:
      for (int i = 8; i >= 1; i--) {
        dstAddr += ins[i];
        if (i != 1) {
          dstAddr = dstAddr << 8;
        }
      }
      break;

    // case 0x00:
    // case 0x10:
    // case 0x90:
    //   if (opc != 0x00 && opc != 0x10 && opc != 0x90) {
    //     printf("Exicting the program.\n");
    //     exit(-1);
    //   }
    //   break;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0xA0:
    case 0xB0:
      ra = ins[1] >> 4;
      rb = ins[1] & 0xf;
      if (opcode == 0xA0 || opcode == 0xB0) {
        if (ra == 0xf || rb != 0xf) {
          printf("# Invalid instruction encountered.\n");
          printf("0x33");
          exit(PRINTERROR);
        }
      } else {
        if (ra == 0xf || rb == 0xf) {
          printf("# Invalid instruction encountered.\n");
          printf("0x34");
          exit(PRINTERROR);
        }
      }
      break;
    default:
      if (opcode != 0x00 && opcode != 0x10 && opcode != 0x90) {
        printf("# Invalid instruction encountered.\n");
        printf("0x35");
        exit(PRINTERROR);
      }
      break;
  }

  switch (opcode) {
    case 0x00:
      res = fprintf(out, "%016lx: %-22s%-8s\n", addr, "00", "halt");
      break;
    case 0x10:
      res = fprintf(out, "%016lx: %-22s%-8s\n", addr, "10", "nop");
      break;
    case 0x90:
      res = fprintf(out, "%016lx: %-22s%-8s\n", addr, "90", "ret");
      break;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
      res = fprintf(out, "%016lx: %hhx%hhx%-19hhx%-8s%s, %s\n", addr, opcode,
                    ra, rb, movIns[opcode & 0xf], regs[ra], regs[rb]);
      break;
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
      res = fprintf(out, "%016lx: %hhx%hhx%-19hhx%-8s%s, %s\n", addr, opcode,
                    ra, rb, opIns[opcode & 0xf], regs[ra], regs[rb]);
      break;
    case 0xA0:
      res = fprintf(out, "%016lx: %hhx%hhx%-19hhx%-8s%s\n", addr, opcode, ra,
                    rb, "pushq", regs[ra]);
      break;
    case 0xB0:
      res = fprintf(out, "%016lx: %hhx%hhx%-19hhx%-8s%s\n", addr, opcode, ra,
                    rb, "popq", regs[ra]);
      break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
      res = fprintf(out,
                    "%016lx: "
                    "%hhx",
                    addr, opcode);
      for (int i = 1; i <= 8; i++) {
        if (ins[i] == 0x00)
          res = fprintf(out, "00");
        else if (ins[i] <= 0x0f) {
          res = fprintf(out, "0");
          res = fprintf(out, "%hhx", ins[i]);
        } else
          res = fprintf(out, "%hhx", ins[i]);
      }
      res = fprintf(out, "    %-8s0x%lx\n", jmpIns[opcode & 0xf], dstAddr);
      break;
    case 0x80:
      res = fprintf(out,
                    "%016lx: "
                    "%hhx",
                    addr, opcode);
      for (int i = 1; i <= 8; i++) {
        if (ins[i] == 0x00)
          res = fprintf(out, "00");
        else if (ins[i] <= 0x0f) {
          res = fprintf(out, "0");
          res = fprintf(out, "%hhx", ins[i]);
        } else
          res = fprintf(out, "%hhx", ins[i]);
      }
      fprintf(out, "    %-8s0x%lx\n", "call", dstAddr);
      break;

    case 0x30:
    case 0x40:
    case 0x50:
      res = fprintf(out,
                    "%016lx: "
                    "%hhx%hhx%hhx",
                    addr, opcode, ra, rb);
      for (int i = 2; i <= 9; i++) {
        if (ins[i] == 0x00)
          res = fprintf(out, "00");
        else if (ins[i] <= 0x0f) {
          res = fprintf(out, "0");
          res = fprintf(out, "%hhx", ins[i]);
        } else
          res = fprintf(out, "%hhx", ins[i]);
      }
      if (opcode == 0x30) {
        res = fprintf(out,
                      "  %-8s"
                      "0x%lx, %s\n",
                      "irmovq", valC, regs[rb]);
      }
      if (opcode == 0x40) {
        res = fprintf(out,
                      "  %-8s%s, "
                      "0x%lx(%s)\n",
                      "rmmovq", regs[ra], valC, regs[rb]);
      }
      if (opcode == 0x50) {
        res = fprintf(out,
                      "  %-8s"
                      "0x%lx(%s), %s\n",
                      "mrmovq", valC, regs[rb], regs[ra]);
      }
      break;
    default:
      break;
  }

  if (res <= 0) return PRINTERROR;

  return PRINTSUCCESS;
}
