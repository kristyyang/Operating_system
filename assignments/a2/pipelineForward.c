#include "pipeline.h"
#include <stdbool.h>
#include <stdio.h>
#include <printf.h>
#define bool _Bool

/* Pipeline control unit. This implementation uses data forwarding to
   handle hazards, in addition to stalls when required. */

int RRwithForwarding(int regm, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W){
		//Data forwarding and checking cases
	if(regm == R_NONE){
		return  0;
		//Execution stage only check for whether tha
	}else if(regm == E->dstE && e->bch ==1){
		return e->valE;
	}else if(regm == M->dstM && M->bch ==1){
		return m->valM;
	}else if(regm ==M->dstE &&M->bch ==1){
		return M->valE;
	}else if(regm == W->dstE &&W->bch ==1){
		return W->valE;
	}else if(regm == W->dstM && W->bch ==1){
		return W->valM;
	}else{
		return readRegister(regm);
	}
			}
int pipelineControl(const int_reg_fd_t *f, const int_reg_de_t *d, const int_reg_em_t *e,
		    const int_reg_mw_t *m, const int_reg_ret_t *w,
		    int_reg_fetch_t *F, int_reg_fd_t *D, int_reg_de_t *E, int_reg_em_t *M,
		    int_reg_mw_t *W) {
  /* TO BE COMPLETED BY THE STUDENT */
   F->PC = f->nextPC;
   F->status = ST_REGULAR;
   *D = *f;
   *E = *d;
   E->valA = RRwithForwarding(d->srcA,e,m,w,E,M,W);
   E->valB = RRwithForwarding(d->srcB,e,m,w,E,M,W);
   *M = *e;
   *W = *m;

  return w->status != ST_EXCEPTION && w->iCode != I_HALT;
}
