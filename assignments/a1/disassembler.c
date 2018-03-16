
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "printRoutines.h"

#define ERROR_RETURN -1
#define SUCCESS 0

int getNumBytes(int n) {
  int num = 0;

  switch (n) {
    case 0x00:
    case 0x10:
    case 0x90:
      num = 1;
      break;
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0xA0:
    case 0xB0:
      num = 2;
      break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x80:
      num = 9;
      break;
    case 0x30:
    case 0x40:
    case 0x50:
      num = 10;
      break;
    default:
      break;
  }

  return num;
}

int main(int argc, char **argv) {
  FILE *machineCode, *outputFile;
  long currAddr = 0;

  // Verify that the command line has an appropriate number
  // of arguments

  if (argc < 3 || argc > 4) {
    printf("Usage: %s InputFilename OutputFilename [startingOffset]\n",
           argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to read, attempt to open it
  // for reading and verify that the open did occur.
  machineCode = fopen(argv[1], "rb");

  if (machineCode == NULL) {
    printf("Failed to open %s: %s\n", argv[1], strerror(errno));
    return ERROR_RETURN;
  }

  // Second argument is the file to write, attempt to open it
  // for writing and verify that the open did occur.
  outputFile = fopen(argv[2], "w");

  if (outputFile == NULL) {
    printf("Failed to open %s: %s\n", argv[2], strerror(errno));
    fclose(machineCode);
    return ERROR_RETURN;
  }

  // If there is a 3rd argument present it is an offset so
  // convert it to a value.
  if (4 == argc) {
    // See man page for strtol() as to why
    // we check for errors by examining errno
    errno = 0;
    currAddr = strtol(argv[3], NULL, 0);
    if (errno != 0) {
      perror("Invalid offset on command line");
      fclose(machineCode);
      fclose(outputFile);
      return ERROR_RETURN;
    }
  }

  printf("Opened %s, starting offset 0x%lX\n", argv[1], currAddr);
  printf("Saving output to %s\n", argv[2]);

  /* Comment or delete the following line and this comment before
   * handing in your final version.
   */
  // samplePrint(stderr);

  // Your code starts here.
  typedef unsigned char byte;

  int opcode = 0;
  int numBytes = 0;
  byte ins[10];
  bool skipWrite = false;

  fseek(machineCode, currAddr, SEEK_SET);
  opcode = fgetc(machineCode);

  while (opcode != EOF) {
    ins[0] = opcode & 0xff;
    if (opcode != 0x00)
      skipWrite = false;
    else
      skipWrite = true;
    numBytes = getNumBytes(opcode);
    for (int i = 1; i < numBytes; i++) {
      int n = fgetc(machineCode);
      if (n == EOF) {
        printf("# Invalid instruction encountered.\n");
        exit(PRINTERROR);
      }
      ins[i] = n & 0xff;
    }
    if (!skipWrite) Println(outputFile, currAddr, ins);
    currAddr += numBytes;
    if (opcode == 0x00)
      skipWrite = true;
    else
      skipWrite = false;
    opcode = fgetc(machineCode);
  }

  fclose(machineCode);
  fclose(outputFile);
  return SUCCESS;
}
