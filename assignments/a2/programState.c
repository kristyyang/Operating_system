#include "pipeline.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

static unsigned char *programMemory;
static size_t programMemorySize;

static int64_t registers[NUM_REGISTERS];

static const char *instrName[16][16] = {
  [I_HALT]   = {"halt"},
  [I_NOP]    = {"nop"},
  [I_RRMVXX] = {
    [C_NC] = "rrmovq",
    [C_LE] = "cmovle",
    [C_L]  = "cmovl",
    [C_E]  = "cmove",
    [C_NE] = "cmovne",
    [C_GE] = "cmovge",
    [C_G]  = "cmovg"},
  [I_IRMOVQ] = {"irmovq"},
  [I_RMMOVQ] = {"rmmovq"},
  [I_MRMOVQ] = {"mrmovq"},
  [I_OPQ]    = {
    [A_ADDQ] = "addq",
    [A_SUBQ] = "subq",
    [A_ANDQ] = "andq",
    [A_XORQ] = "xorq",
    [A_MULQ] = "mulq",
    [A_DIVQ] = "divq",
    [A_MODQ] = "movq"
  },
  [I_JXX]    = {
    [C_NC] = "jmp",
    [C_LE] = "jle",
    [C_L]  = "jl",
    [C_E]  = "je",
    [C_NE] = "jne",
    [C_GE] = "jge",
    [C_G]  = "jg"},
  [I_CALL]   = {"call"},
  [I_RET]    = {"ret"},
  [I_PUSHQ]  = {"pushq"},
  [I_POPQ]   = {"popq"},
};

static const char *regName[R_NONE] = {
  [R_RAX] = "%rax",
  [R_RCX] = "%rcx",
  [R_RDX] = "%rdx",
  [R_RBX] = "%rbx",
  [R_RSI] = "%rsi",
  [R_RDI] = "%rdi",
  [R_RSP] = "%rsp",
  [R_RBP] = "%rbp",
  [R_R8]  = "%r8",
  [R_R9]  = "%r9",
  [R_R10] = "%r10",
  [R_R11] = "%r11",
  [R_R12] = "%r12",
  [R_R13] = "%r13",
  [R_R14] = "%r14"
};

/* Maps the input memory file into a region of memory.
   Parameters: programFD: file descriptor of the file being mapped.
   Returns: 0 in case of success, or something else in case of error.
 */
int initializeMemory(int programFD) {
  
  struct stat statBuff;
  
  if (fstat(programFD, &statBuff))
    return -2;

  // Do the actual mapping.
  programMemorySize = statBuff.st_size;
  programMemory = mmap(0, statBuff.st_size, PROT_READ|PROT_WRITE,
		       MAP_PRIVATE, programFD, 0);
  if (programMemory == MAP_FAILED)
    return -4;

  return 0;
}

/* Reads a specified number of bytes from memory.
   Parameters: address: starting memory address to read from.
               size:    number of bytes to read.
	       data:    address where the data should be copied to.
   Returns: 0 in case of success, something else in case of error.
 */
int readMemory(unsigned int address, size_t size, void *data) {
  if (address + size > programMemorySize)
    return -1;

  memcpy(data, programMemory + address, size);
  return 0;
}

/* Writes a specified number of bytes to memory.
   Parameters: address: starting memory address to write to.
               size:    number of bytes to write.
	       data:    address where the data should be copied from.
   Returns: 0 in case of success, something else in case of error.
 */
int writeMemory(unsigned int address, size_t size, void *data) {

  if (address + size > programMemorySize)
    return -1;

  memcpy(data, programMemory + address, size);
  return 0;
}

/* Reads a single byte from memory.
   Parameters: address: memory address to read from.
	       byte:    address where the data should be copied to.
   Returns: 0 in case of success, something else in case of error.
 */
int readMemoryByte(unsigned int address, unsigned char *byte) {
  return readMemory(address, 1, byte);
}

/* Reads a single quad (8-byte integer) from memory in little-endian.
   Parameters: address: starting memory address to read from.
	       quad:    address where the data should be copied to.
   Returns: 0 in case of success, something else in case of error.
 */
int readMemoryQuad(unsigned int address, int64_t *quad) {

  unsigned char data[sizeof(*quad)];
  int i;
  if ((i = readMemory(address, sizeof(*quad), data)))
    return i;

  *quad = 0;
  for (i = sizeof(*quad) - 1; i >= 0; i--) {
    *quad <<= 8;
    *quad |= data[i];
  }
  return 0;
}

/* Writes a single quad (8-byte integer) to memory in little-endian.
   Parameters: address: starting memory address to write to.
	       quad:    integer to be written in memory.
   Returns: 0 in case of success, something else in case of error.
 */
int writeMemoryQuad(unsigned int address, int64_t quad) {

  if (address + sizeof(quad) > programMemorySize)
    return -1;

  for (int i = 0; i < sizeof(quad); i++) {
    programMemory[address++] = quad;
    quad >>= 8;
  }

  return 0;
}

/* Reads the value of a general-purpose register.
   Parameters: reg: number of the register to read.
   Returns: the current value of the register, or zero if the
            register doesn't exist.
 */
int64_t readRegister(y86_register_t reg) {
  if (reg > NUM_REGISTERS)
    return 0;
  return registers[reg];
}

/* Writes a new value in a general-purpose register.
   Parameters: reg:   number of the register to be written. If
                      invalid, no register will be written.
               value: new value of the register.
 */
void writeRegister(y86_register_t reg, int64_t value) {
  if (reg != R_NONE)
    registers[reg] = value;
}

static inline char *statusString(instr_status_t status) {
  switch (status) {
  case ST_REGULAR:     return "-------------------";
  case ST_EXCEPTION:   return "**** EXCEPTION ****";
  case ST_BUBBLE:      return "****** BUBBLE *****";
  case ST_STALLED:     return "***** STALLED *****";
  case ST_SPECULATIVE: return "*** SPECULATIVE ***";
  }
  return "???????????????????";
}

/* Prints the current stage of the CPU. Receives as parameters the
   input register values of all stages. Memory and register values are
   obtained from global variables.

   Also prompts the user if the program should proceed. Returns 0 to
   proceed, or something else to terminate.
 */
int printState(const int_reg_fetch_t *F, const int_reg_fd_t *D, const int_reg_de_t *E,
	       const int_reg_em_t *M, const int_reg_mw_t *W) {

  char line[100];
  
  // Clear screen. Exclusive for ANSI-based terminals (e.g., Linux).
  printf("\e[1;1H\e[2J");

  unsigned char valAtPC;
  readMemoryByte(F->PC, &valAtPC);
  
  // Print internal registers
  printf("Fetch        %6.6s Decode       %6.6s Execute      %6.6s "
	 "Memory       %6.6s Write-back   %6.6s\n"
	 "%-19s %-19s %-19s %-19s %-19s\n"
	 "PC=%-16lx iCode:iFun=%1x:%1x      iCode:iFun=%1x:%1x      iCode=%-13x iCode=%-13x\n"
	 "                    rA:rB=%1x:%1x           valC=%-14lx dstE=%-14x dstE=%-14x\n"
	 "                    valC=%-14lx srcA:srcB=%1x:%-7x dstM=%-14x dstM=%-14x\n"
	 "                    valP=%-14lx dstE:dstM=%1x:%-7x valA=%-14lx valE=%-14lx\n"
	 "                                        valA=%-14lx valE=%-14lx bch=%-15x\n"
	 "                                        valB=%-14lx bch=%-15x valM=%-15lx\n",
	 instrName[valAtPC >> 4][valAtPC & 0xF],
	 instrName[D->iCode][D->iFun], instrName[E->iCode][E->iFun],
	 instrName[M->iCode][M->iFun], instrName[W->iCode][W->iFun],
	 statusString(F->status), statusString(D->status), statusString(E->status),
	 statusString(M->status), statusString(W->status), 
	 F->PC, D->iCode, D->iFun, E->iCode, E->iFun, M->iCode, W->iCode,
	 D->rA, D->rB, E->valC, M->dstE, W->dstE,
	 D->valC, E->srcA, E->srcB, M->dstM, W->dstM,
	 D->valP, E->dstE, E->dstM, M->valA, W->valE,
	 E->valA, M->valE, W->bch,
	 E->valB, M->bch, W->valM);

  // Print register values
  for (int i = 0; i < NUM_REGISTERS; i++) {
    if (!(i % 4)) putchar('\n');
    printf("%4.4s = 0x%-16lx ", regName[i], registers[i]);
  }
  printf("\n\nMemory:\n");

  uint64_t address = 0, delta;
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  for (unsigned int line = 16; address < programMemorySize && line < w.ws_row - 1; line++) {
    printf("%08lx:", address);
    for (delta = 0; address + delta < programMemorySize && delta < 32; delta++) {
      if (!(delta % 8)) putchar(' ');
      printf("%02x", programMemory[address + delta]);
    }
    address += delta;
    printf("\n");
  }

  // Prompt to continue
  printf("Press ENTER to proceed to next clock, or Ctrl-D to exit...");
  do {
    if (fgets(line, sizeof(line), stdin) == NULL)
      return -1;
  } while (!strchr(line, '\n'));
  
  return 0;
}
