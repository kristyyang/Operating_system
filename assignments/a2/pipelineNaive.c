#include "pipeline.h"

/* Pipeline control unit. This implementation assumes there are no
   hazards, and simply forwards the data without processing from one
   stage to the next. This is not a safe implementation, it is
   provided only as a reference for a simple implementation. */
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,
		    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W) {

  F->PC = f->nextPC;
  F->status = ST_REGULAR;
  *D = *f;
  *E = *d;
  *M = *e;
  *W = *m;

  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}
