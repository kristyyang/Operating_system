#include "pipeline.h"

/* Global variable that keeps track of the condition codes of the last
   ALU operation.
 */
static struct cond_codes {
  unsigned int zero:1;
  unsigned int sign:1;
  unsigned int overflow:1;
} CC;

/* Execute stage of the pipeline CPU. Handles the ALU and condition
   codes.
 */
void executeStage(const int_reg_de_t *E, int_reg_em_t *e) {

  e->PC = E->PC;
  e->iCode = E->iCode;
  e->iFun = E->iFun;
  e->status = E->status;
  e->dstE = E->dstE;
  e->dstM = E->dstM;
  e->valA = E->valA;

  if (E->status == ST_EXCEPTION)
    return;
  else if (E->status == ST_BUBBLE) {
    e->iCode = I_NOP;
    e->dstE = R_NONE;
    e->dstM = R_NONE;
  }
  else if (E->status == ST_STALLED)
    e->status = ST_REGULAR;

  // valE MUX (also updates CC if applicable)
  switch(e->iCode) {
  case I_OPQ:
    switch (E->iFun) {
    case A_ADDQ:
      e->valE = E->valB + E->valA;
      CC.overflow = ((E->valB < 0) == (E->valA < 0)) &&
	((e->valE < 0) != (E->valB < 0));
      break;
    case A_SUBQ:
      e->valE = E->valB - E->valA;
      CC.overflow = ((E->valB < 0) != (E->valA < 0)) &&
	((e->valE < 0) != (E->valB < 0));
      break;
    case A_ANDQ:
      e->valE = E->valB & E->valA;
      CC.overflow = 0;
      break;
    case A_XORQ:
      e->valE = E->valB ^ E->valA;
      CC.overflow = 0;
      break;
    case A_MULQ:
      e->valE = E->valB * E->valA;
      CC.overflow = E->valB != 0 && e->valE / E->valB != E->valA;
      break;
    case A_DIVQ:
      e->valE = E->valA == 0 ? E->valB : E->valB / E->valA;
      CC.overflow = E->valA == 0;
      break;
    case A_MODQ:
      e->valE = E->valA == 0 ? E->valB : E->valB % E->valA;
      CC.overflow = E->valA == 0;
      break;
    }
    CC.zero = (e->valE == 0);
    CC.sign = (e->valE < 0);
    break;
  case I_IRMOVQ:
    e->valE = 0 + E->valC;
    break;
  case I_RET:
  case I_POPQ:
    e->valE = E->valB + 8;
    break;
  case I_CALL:
  case I_PUSHQ:
    e->valE = E->valB + -8;
    break;
  case I_MRMOVQ:
  case I_RMMOVQ:
    e->valE = E->valB + E->valC;
    break;
  case I_RRMVXX:
    e->valE = 0 + E->valA;
    break;
  default:
    break;
  }

  // bch MUX
  switch(e->iCode) {
  case I_RRMVXX:
  case I_JXX:
    switch (E->iFun) {
    case C_NC:
      e->bch = 1;
      break;
    case C_LE:
      e->bch = (CC.sign != CC.overflow) || CC.zero;
      break;
    case C_L:
      e->bch = CC.sign != CC.overflow;
      break;
    case C_E:
      e->bch = CC.zero;
      break;
    case C_NE:
      e->bch = !CC.zero;
      break;
    case C_GE:
      e->bch = CC.sign == CC.overflow;
      break;
    case C_G:
      e->bch = (CC.sign == CC.overflow) && !CC.zero;
      break;
    }
    break;
  default:
    e->bch = 1;
    break;
  }
}
