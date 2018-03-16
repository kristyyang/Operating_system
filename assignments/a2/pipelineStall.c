#include <printf.h>
#include <stdbool.h>
#include <stdio.h>
#include "pipeline.h"
#define bool _Bool

bool isDataHazardOnReg(int_reg_de_t *E, int_reg_em_t *M, int_reg_mw_t *W,
                       int reg) {
  return reg != R_NONE && (E->dstE == reg || M->dstE == reg || W->dstE == reg ||
                           E->dstM == reg || M->dstM == reg || W->dstM == reg);
}

/* Pipeline control unit. This implementation uses only stalls when
   required to avoid data and control hazards. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d,
                    const int_reg_em_t *e, const int_reg_mw_t *m,
                    const int_reg_ret_t *w, int_reg_fetch_t *F, int_reg_fd_t *D,
                    int_reg_de_t *E, int_reg_em_t *M, int_reg_mw_t *W) {
  /* TO BE COMPLETED BY THE STUDENT */

  /*Data hazard***********/
  if ((isDataHazardOnReg(E, M, W, d->srcA)) ||
      (isDataHazardOnReg(E, M, W, d->srcB))) {
    F->status = ST_STALLED;
    D->status = ST_STALLED;
    E->status = ST_BUBBLE;
  }

  /* Control Hazard: Conditional JXX **************** STUDENT ADDED CODED**/
  else if ((D->iCode == I_JXX && D->iFun != C_NC) ||
      (E->iCode == I_JXX && E->iFun != C_NC)) {
    F->status = ST_STALLED;
    D->status = ST_BUBBLE;
  } else if (D->iCode == I_RET || E->iCode == I_RET || M->iCode == I_RET) {
    F->status = ST_STALLED;
    D->status = ST_BUBBLE;
  }
  else {
    F->PC = f->nextPC;
    F->status = ST_REGULAR;
    *D = *f;
    *E = *d;
    *M = *e;
    *W = *m;
  }

  *M = *e;
  *W = *m;

  if (w->status == ST_BUBBLE) {
    F->PC = f->nextPC;
    F->status = ST_REGULAR;
    *D = *f;
    *E = *d;
    *M = *e;
    *W = *m;
  }

  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}
