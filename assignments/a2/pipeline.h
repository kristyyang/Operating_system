/* This file contains the prototypes and constants needed to use the
   routines defined in fetchStage.c, decodeStage.c, executeStage.c,
   memoryStage.c and writeBackStage.c
*/

#ifndef _STAGES_H_
#define _STAGES_H_

#include <stdio.h>
#include <stdint.h>


typedef enum y86_icode {
  I_HALT   = 0x0,
  I_NOP    = 0x1,
  I_RRMVXX = 0x2,
  I_IRMOVQ = 0x3,
  I_RMMOVQ = 0x4,
  I_MRMOVQ = 0x5,
  I_OPQ    = 0x6,
  I_JXX    = 0x7,
  I_CALL   = 0x8,
  I_RET    = 0x9,
  I_PUSHQ  = 0xA,
  I_POPQ   = 0xB
} y86_icode_t;

typedef enum y86_reg {
  R_RAX = 0x0,
  R_RCX = 0x1,
  R_RDX = 0x2,
  R_RBX = 0x3,
  R_RSP = 0x4,
  R_RBP = 0x5,
  R_RSI = 0x6,
  R_RDI = 0x7,
  R_R8  = 0x8,
  R_R9  = 0x9,
  R_R10 = 0xA,
  R_R11 = 0xB,
  R_R12 = 0xC,
  R_R13 = 0xD,
  R_R14 = 0xE,
  R_NONE = 0xF
} y86_register_t;

#define NUM_REGISTERS 15

typedef enum y86_condition {
  C_NC = 0x0,
  C_LE = 0x1,
  C_L  = 0x2,
  C_E  = 0x3,
  C_NE = 0x4,
  C_GE = 0x5,
  C_G  = 0x6
} y86_condition_t;

typedef enum y86_operation {
  A_ADDQ = 0x0,
  A_SUBQ = 0x1,
  A_ANDQ = 0x2,
  A_XORQ = 0x3,
  A_MULQ = 0x4,
  A_DIVQ = 0x5,
  A_MODQ = 0x6
} y86_operation_t;

typedef enum instr_status {
  ST_REGULAR,
  ST_EXCEPTION,
  ST_STALLED,
  ST_BUBBLE,
  ST_SPECULATIVE
} instr_status_t;

// Internal registers getting into Fetch
typedef struct int_reg_fetch {

  uint64_t PC;
  instr_status_t status;

} int_reg_fetch_t;

// Internal registers from Fetch to Decode
typedef struct int_reg_fd {

  uint64_t PC;
  unsigned int iCode:4;
  unsigned int iFun:4;
  unsigned int rA:4;
  unsigned int rB:4;

  int64_t valC;
  uint64_t valP;

  uint64_t nextPC;

  instr_status_t status;

} int_reg_fd_t;

// Internal registers from Decode to Execute
typedef struct int_reg_de {

  uint64_t PC;
  unsigned int iCode:4;
  unsigned int iFun:4;

  int64_t valC;

  instr_status_t status;

  unsigned int srcA:4;
  unsigned int srcB:4;
  unsigned int dstE:4;
  unsigned int dstM:4;

  int64_t valA;
  int64_t valB;
} int_reg_de_t;

// Internal registers from Execute to Memory
typedef struct int_reg_em {

  uint64_t PC;
  unsigned int iCode:4;
  unsigned int iFun:4;

  instr_status_t status;

  unsigned int dstE:4;
  unsigned int dstM:4;

  int64_t valA;
  int64_t valE;
  unsigned int bch:1;

} int_reg_em_t;

// Internal registers from Memory to Write-back
typedef struct int_reg_mw {

  uint64_t PC;
  unsigned int iCode:4;
  unsigned int iFun:4;

  instr_status_t status;

  unsigned int dstE:4;
  unsigned int dstM:4;

  int64_t valE;
  unsigned int bch:1;

  int64_t valM;

} int_reg_mw_t;

// Internal registers for retiring instructions
typedef struct int_reg_ret {

  uint64_t PC;
  unsigned int iCode:4;

  instr_status_t status;

} int_reg_ret_t;

void fetchStage(const int_reg_fetch_t *F, int_reg_fd_t *f);
void decodeStage(const int_reg_fd_t *D, int_reg_de_t *d);
void executeStage(const int_reg_de_t *E, int_reg_em_t *e);
void memoryStage(const int_reg_em_t *M, int_reg_mw_t *m);
void writeBackStage(const int_reg_mw_t *W, int_reg_ret_t *w);

int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,
		    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W);

int initializeMemory(int programFD);

int readMemory(unsigned int address, size_t size, void *data);
int writeMemory(unsigned int address, size_t size, void *data);
int readMemoryByte(unsigned int address, unsigned char *byte);
int readMemoryQuad(unsigned int address, int64_t *quad);
int writeMemoryQuad(unsigned int address, int64_t quad);

int64_t readRegister(y86_register_t reg);
void writeRegister(y86_register_t reg, int64_t value);

int printState(const int_reg_fetch_t *F, const int_reg_fd_t *D, const int_reg_de_t *E,
	       const int_reg_em_t *M, const int_reg_mw_t *W);

#endif /* STAGES */
