#include "pipeline.h"

/* Write-back stage of the pipeline CPU. Saves the values of changed
   registers.
 */
void writeBackStage(const int_reg_mw_t *W, int_reg_ret_t *w) {

  w->PC = W->PC;
  w->iCode = W->iCode;
  w->status = W->status;
  
  if (W->status == ST_EXCEPTION)
    return;
  if (W->status == ST_BUBBLE) {
    w->iCode = I_NOP;
    return;
  }
  else if (W->status == ST_STALLED)
    w->status = ST_REGULAR;

  if (W->dstE != R_NONE && W->bch)
    writeRegister(W->dstE, W->valE);

  if (W->dstM != R_NONE)
    writeRegister(W->dstM, W->valM);
}

