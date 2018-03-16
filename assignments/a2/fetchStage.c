#include "pipeline.h"

static int validInstruction(unsigned char iCode, unsigned char iFun);

/* Fetch stage of the pipeline CPU. Reads the next instruction, and
   identifies the components (arguments) of that instruction.
 */
void fetchStage(const int_reg_fetch_t *F, int_reg_fd_t *f) {

  size_t instrSize = 1;
  unsigned char instruction, registers;

  // If Fetch is stalled, do nothing and keep f unmodified
  if (F->status == ST_STALLED)
    return;

  if (readMemoryByte(F->PC, &instruction))
    f->status = ST_EXCEPTION;
  else {
    f->PC = F->PC;
    f->iCode = (instruction >> 4) & 0xF;
    f->iFun = instruction & 0xF;
    f->status = validInstruction(f->iCode, f->iFun) ? F->status : ST_EXCEPTION;
  }

  if (f->status != ST_EXCEPTION) {

    // rA:rB MUX
    switch (f->iCode) {
    case I_RRMVXX:
    case I_RMMOVQ:
    case I_MRMOVQ:
    case I_IRMOVQ:
    case I_OPQ:
    case I_PUSHQ:
    case I_POPQ:
      if (readMemoryByte(F->PC + 1, &registers))
	f->status = ST_EXCEPTION;
      else {
	instrSize++;

	f->rA = (registers >> 4) & 0xF;
	f->rB = registers & 0xF;

	switch (f->iCode) {
	case I_RRMVXX:
	case I_RMMOVQ:
	case I_MRMOVQ:
	case I_OPQ:
	  if (f->rA == R_NONE || f->rB == R_NONE)
	    f->status = ST_EXCEPTION;
	  break;
	case I_IRMOVQ:
	  if (f->rA != R_NONE || f->rB == R_NONE)
	    f->status = ST_EXCEPTION;
	  break;
	case I_PUSHQ:
	case I_POPQ:
	  if (f->rA == R_NONE || f->rB != R_NONE)
	    f->status = ST_EXCEPTION;
	  break;
	}
      }
      break;
    default:
      // Nothing to do, remaining instructions don't read registers
      break;
    }
  }

  if (f->status != ST_EXCEPTION) {

    // valC MUX
    switch (f->iCode) {
    case I_IRMOVQ:
    case I_RMMOVQ:
    case I_MRMOVQ:
    case I_JXX:
    case I_CALL:
      if (readMemoryQuad(F->PC + instrSize, &f->valC))
	f->status = ST_EXCEPTION;
      else
	instrSize += 8;
      break;
    default:
      // Nothing to do, remaining instructions don't have valC
      break;
    }
  }

  f->valP = F->PC + instrSize;

  // nextPC MUX
  switch (f->iCode) {
  case I_HALT:
    f->nextPC = F->PC;
    break;
  case I_CALL:
  case I_JXX:
    // Default for jumps is valC. Conditional jumps are handled by
    // pipeline control unit.
    f->nextPC = f->valC;
    break;
  case I_RET:
    // Return has no default behaviour in fetch for next PC. Update of
    // PC handled by pipeline control unit. Using default behaviour
    // here.
  default:
    f->nextPC = f->valP;
    break;
  }

  if (f->status == ST_EXCEPTION)
    f->nextPC = F->PC;
}

/* Checks if the iCode-iFun combo is valid. */
static int validInstruction(unsigned char iCode, unsigned char iFun) {
  switch (iCode) {
  case I_HALT:
  case I_NOP:
  case I_IRMOVQ:
  case I_RET:
  case I_PUSHQ:
  case I_POPQ:
  case I_CALL:
  case I_RMMOVQ:
  case I_MRMOVQ:
    if (iFun)
      return 0;
    return 1;
  case I_RRMVXX:
  case I_JXX:
    switch (iFun) {
    case C_NC:
    case C_LE:
    case C_L:
    case C_E:
    case C_NE:
    case C_GE:
    case C_G:
      return 1;
    default:
      return 0;
    }
  case I_OPQ:
    switch (iFun) {
    case A_ADDQ:
    case A_SUBQ:
    case A_ANDQ:
    case A_XORQ:
    case A_MULQ:
    case A_DIVQ:
    case A_MODQ:
      return 1;
    default:
      return 0;
    }
  default:
    return 0;
  }

}
