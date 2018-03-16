#include <stdio.h>
#include "pipeline.h"
int16_t temp_valC;
int16_t temp_valP;
int16_t ret_addr;

/* Pipeline control unit. This implementation implements data
   forwarding and branch prediction to handle hazards, in addition to
   stalls when these techniques cannot avoid the hazard. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d,
                    const int_reg_em_t *e, const int_reg_mw_t *m,
                    const int_reg_ret_t *w, int_reg_fetch_t *F, int_reg_fd_t *D,
                    int_reg_de_t *E, int_reg_em_t *M, int_reg_mw_t *W) {
  /* TO BE COMPLETED BY THE STUDENT */
  F->PC = f->nextPC;

  if (f->iCode == I_CALL) {
    F->PC = f->valC;
    ret_addr = f->valP;
  }

  if (f->iCode == I_JXX && f->iFun != C_NC) {
    temp_valP = f->valP;
  }

  if (e->iCode == I_JXX && e->iFun != C_NC) {
    temp_valC = E->valC;
  }

  if (m->iCode == I_JXX && m->iFun != C_NC) {
    switch (m->bch) {
      case 1:
        F->PC = temp_valC;
        F->status = ST_BUBBLE;
        D->status = ST_BUBBLE;
        E->status = ST_BUBBLE;
        *M = *e;
        *W = *m;
        break;
      case 0:
        F->PC = temp_valP;
        F->status = ST_REGULAR;
        D->status = ST_BUBBLE;
        E->status = ST_BUBBLE;
        M->status = ST_BUBBLE;
        *W = *m;
        break;
      default:
        break;
    }
  } else if (m->iCode == I_RET) {
    F->PC = ret_addr;
    F->status = ST_REGULAR;
    D->status = ST_BUBBLE;
    E->status = ST_BUBBLE;
    M->status = ST_BUBBLE;
    *W = *m;
  } else {
    F->PC = f->nextPC;
    F->status = ST_REGULAR;
    *D = *f;
    *E = *d;
    *M = *e;
    *W = *m;
  }

  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}
