all: pipelineNaive pipelineStall pipelineForward pipelineBP

CC=gcc
CLIBS=-lc
CFLAGS=-g -Wall -pedantic -std=c99

OBJS=main.o programState.o fetchStage.o decodeStage.o executeStage.o memoryStage.o writeBackStage.o

pipelineNaive: pipelineNaive.o $(OBJS)
pipelineStall: pipelineStall.o $(OBJS)
pipelineForward: pipelineForward.o $(OBJS)
pipelineBP: pipelineBP.o $(OBJS)

main.o: main.c pipeline.h
programState.o: programState.c pipeline.h
fetchStage.o: fetchStage.c pipeline.h
decodeStage.o: decodeStage.c pipeline.h
executeStage.o: executeStage.c pipeline.h
memoryStage.o: memoryStage.c pipeline.h
pipelineNaive.o: pipelineNaive.c pipeline.h
pipelineStall.o: pipelineStall.c pipeline.h
pipelineForward.o: pipelineForward.c pipeline.h
pipelineBP.o: pipelineBP.c pipeline.h

clean:
	-rm -rf *.o pipelineNaive pipelineStall pipelineForward pipelineBP
