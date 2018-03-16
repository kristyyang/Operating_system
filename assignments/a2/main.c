#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "pipeline.h"

#define ERROR_RETURN -1
#define SUCCESS 0

int main(int argc, char **argv) {
  
  int programFD;
  int_reg_fetch_t F = { .status = ST_REGULAR };
  int_reg_fd_t f, D = { .status = ST_REGULAR, .iCode = I_NOP };
  int_reg_de_t d, E = { .status = ST_REGULAR, .iCode = I_NOP };
  int_reg_em_t e, M = { .status = ST_REGULAR, .iCode = I_NOP };
  int_reg_mw_t m, W = { .status = ST_REGULAR, .iCode = I_NOP };
  int_reg_ret_t w;
  

  // Verify that the command line has an appropriate number
  // of arguments

  if (argc < 2 || argc > 3) {
    printf("Usage: %s InputFilename [startingPC]\n", argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to read, attempt to open it 
  // for reading and verify that the open did occur.
  programFD = open(argv[1], O_RDONLY);

  if (programFD < 0) {
    perror("Failed to open input file");
    return ERROR_RETURN;
  }

  if (initializeMemory(programFD) < 0) {
    perror("Failed to read input file");
    return ERROR_RETURN;
  }
  
  // If there is a 3rd argument present it is an offset so
  // convert it to a value. 
  if (3 == argc) {
    errno = 0;
    F.PC = strtol(argv[2], NULL, 0);
    if (errno != 0) {
      perror("Invalid PC on command line");
      close(programFD);
      return ERROR_RETURN;
    }
  }

  unsigned char bogus;
  
  if (readMemoryByte(F.PC, &bogus)) {
    puts("Initial PC is not within range of program.\n");
    close(programFD);
    return ERROR_RETURN;
  }

  fprintf(stderr, "Opened %s, starting PC 0x%lX\n", argv[1], F.PC);

  // Initialize statuses for stages beyond decode with values based on
  // nop instruction.
  decodeStage(&D, &E);
  executeStage(&E, &M);
  memoryStage(&M, &W);
  writeBackStage(&W, &w);
  
  do {
    
    if (printState(&F, &D, &E, &M, &W))
      break;
    
    fetchStage(&F, &f);
    decodeStage(&D, &d);
    executeStage(&E, &e);
    memoryStage(&M, &m);
    writeBackStage(&W, &w);

  } while(pipelineControl(&f, &d, &e, &m, &w, &F, &D, &E, &M, &W));

  printf("\nGood-bye...\n");
  
  close(programFD);
  return SUCCESS;
}
