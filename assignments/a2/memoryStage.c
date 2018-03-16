#include "pipeline.h"

/* Memory stage of the pipeline CPU. Handles memory reads and writes.
 */
void memoryStage(const int_reg_em_t *M, int_reg_mw_t *m) {

  m->PC = M->PC;
  m->iCode = M->iCode;
  m->iFun = M->iFun;
  m->status = M->status;
  m->dstE = M->dstE;
  m->dstM = M->dstM;
  m->valE = M->valE;
  m->bch = M->bch;
  
  if (M->status == ST_EXCEPTION)
    return;
  else if (M->status == ST_BUBBLE) {
    m->iCode = I_NOP;
    m->dstE = R_NONE;
    m->dstM = R_NONE;
  }
  else if (M->status == ST_STALLED)
    m->status = ST_REGULAR;

  // memory MUX
  switch(m->iCode) {
  case I_RMMOVQ:
  case I_PUSHQ:
  case I_CALL:
    if (writeMemoryQuad(M->valE, M->valA))
      m->status = ST_EXCEPTION;
    break;
  case I_MRMOVQ:
    if (readMemoryQuad(M->valE, &m->valM))
      m->status = ST_EXCEPTION;
    break;
  case I_RET:
  case I_POPQ:
    if (readMemoryQuad(M->valA, &m->valM))
      m->status = ST_EXCEPTION;
    break;
  }
}

