#include "pipeline.h"

/* Decode stage of the pipeline CPU. Reads the values of required
   registers, and identifies the registers that need to be updated.
 */

void decodeStage(const int_reg_fd_t *D,  int_reg_de_t *d) {

  // If status is stalled, do nothing and keep d unmodified.
  if (D->status == ST_STALLED)
    return;

  d->PC = D->PC;
  d->iCode = D->iCode;
  d->iFun = D->iFun;
  d->valC = D->valC;
  d->status = D->status;

  if (D->status == ST_EXCEPTION)
    return;
  else if (D->status == ST_BUBBLE) {
    d->iCode = I_NOP;
    d->iFun = 0;
  }

  // srcA MUX
  switch(d->iCode) {
  case I_PUSHQ:
  case I_RMMOVQ:
  case I_RRMVXX:
  case I_OPQ:
    d->srcA = D->rA;
    break;
  // case I_RET:
  // case I_POPQ:
  //   d->srcB = R_RSP;
  //   break;
  case I_RET:
   case I_POPQ:
     d->srcA = R_RSP;
     break;
  default:
    d->srcA = R_NONE;
    break;
  }

  // srcB MUX
  switch(d->iCode) {
  case I_RMMOVQ:
  case I_MRMOVQ:
  case I_OPQ:
    d->srcB = D->rB;
    break;
  case I_RET:
  case I_CALL:
  case I_PUSHQ:
  case I_POPQ:
    d->srcB = R_RSP;
    break;
  default:
    d->srcB = R_NONE;
    break;
  }

  // dstE MUX
  switch(d->iCode) {
  case I_IRMOVQ:
  case I_RRMVXX:
  case I_OPQ:
    d->dstE = D->rB;
    break;
  case I_RET:
  case I_CALL:
  case I_PUSHQ:
  case I_POPQ:
    d->dstE = R_RSP;
    break;
  default:
    d->dstE = R_NONE;
    break;
  }

  // dstM MUX
  switch(d->iCode) {
  case I_POPQ:
  case I_MRMOVQ:
    d->dstM = D->rA;
    break;
  default:
    d->dstM = R_NONE;
    break;
  }

  // valA MUX
  switch (d->iCode) {
  case I_CALL:
  case I_JXX:
    d->valA = D->valP;
    break;
  default:
    d->valA = readRegister(d->srcA);
    break;
  }


  // valB update (no MUX)
  d->valB = readRegister(d->srcB);
}
