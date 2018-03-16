#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "life.h"
#include <unistd.h>
#include <limits.h>

static struct timeval start_ts, end_ts;

#define BIRTH  3
#define TOOCROWDED 4
#define TOOEMPTY 2


void startClock() {
    gettimeofday(&start_ts, 0);
}

void endClock() {
    gettimeofday(&end_ts, 0);
}

long usecClock() {
    struct timeval res;
    timersub(&end_ts, &start_ts, &res);
    return res.tv_usec + 1000000 * res.tv_sec;
}

int checkHealth(long neighbourCount, long oldCell) {

    if ((neighbourCount < 0) || (neighbourCount > 8)) {
        printf("Invalid cell count %ld\n", neighbourCount);
        sleep(3);
    }

    if (!oldCell) {

        // Dead cell with 3 neighbours causes a birth
        if (neighbourCount == BIRTH) return 1;

        // Dead cell that is not to change
        return 0;
    }
    else {
        // The cell is alive

        // Too crowded
        if (neighbourCount >= TOOCROWDED ) return 0;

        // Too empty
        if (neighbourCount < TOOEMPTY) return 0;

        // Live on
        return 1;
    }
}

// Count the number of live cells and print that information.

void countAlive(long world[N][N]) {
    int i;
    int j;
    int count = 0;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            count += world[i][j] ? 1 : 0;
        }
    printf("Alive %d\n", count);
}


// This may be helpful for debugging if things aren't working once
// you make changes

void printWorld(long world[N][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%ld ", world[i][j]);
        }
        printf("\n");
    }
    printf("-----------------------------------\n");
    for (i = -1; i < N+1; i++) {
        for (j = -1; j < N+1; j++) {
            printf("%ld ", world[(i+N) % N ][(j+N) % N]);
        }
        printf("\n");
    }
    printf("!!!!!!!-----------------------------------\n");
}

// The basic, not so smart implementation of of the routine that
// takes an existing world and populates a new one.
int base_life(long oldWorld[N][N], long newWorld[N][N]) {

    int i, j;
    int col, row;

    //clear the new world
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            newWorld[j][i] =  0;
        }

    // Count the cells to the top left
    for (i = 0; i < N; i++) {
        col = (i -1 + N) % N;
        for (j = 0; j < N; j++) {
            row = (j - 1 + N ) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Count the cells immediately above
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            row = (j - 1 + N ) % N;
            newWorld[j][i] += oldWorld[row][i];
        }
    }

    // Count the cells to the top right
    for (i = 0; i < N; i++) {
        col = (i + 1 + N) % N;
        for (j = 0; j < N; j++) {
            row = (j - 1  + N ) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Count the cells to the immediate left
    for (i = 0; i < N; i++)
    {
        col = (i -1 + N) % N;
        for (j = 0; j < N; j++) {
            newWorld[j][i] += oldWorld[j][col];
        }
    }

    // Count the cells to the immediate right
    for (i = 0; i < N; i++)
    {
        col = (i + 1 + N) % N;
        for (j = 0; j < N; j++)
        {
            newWorld[j][i] += oldWorld[j][col];
        }
    }

// Count the cells to the bottom left
    for (i = 0; i < N; i++) {
        col = (i - 1 + N) % N;
        for (j = 0; j < N; j++) {
            row = (j + 1 + N ) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Count the cells immediately below
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            row = (j + 1  + N ) % N;
            newWorld[j][i] += oldWorld[row][i];
        }
    }

    // Count the cells to the bottom right
    for (i = 0; i < N; i++) {
        col = (i + 1 + N) % N;
        for (j = 0; j < N; j++) {
            row = (j + 1  + N ) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Check each cell to see if it should come to life, continue to live, or die
    int alive = 0;

    //printWorld(newWorld);
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
            alive += newWorld[j][i] ? 1:0;
        }
    //printWorld(newWorld);
    return alive;
}


// Fill a world with "life"

void init (long G[N][N]) {
    int i, j;
    int alive = 0;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            G[i][j] = ((random() % 100) > 70 );
            alive += G[i][j];
        }
    }
    printf("%d alive cells\n", alive);
}



// Check if two worlds are identical

int notIdentical(long G[N][N], long G2[N][N]) {
    int i, j;
    int ret_val = 0;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            if (G[i][j] != G2[i][j]) {
                printf("Differs at G[%d][%d] %ld %ld \n", i, j, G[i][j], G2[i][j]);
                ret_val++;
            }
        }
    }
    return ret_val;
}


// copy one world to another world. The assumption is that they are the
// same size.

void copy(long G[N][N], long G2[N][N]) {
    int i, j;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            G2[i][j] = G[i][j];
        }
    }
}

#define VERBOSE 1
#define PARANOID 1


// NOTE the complete lack of error checking, which isn't particularly good style,
// in general you shouldn't write programs with the lack of meaningful comments and
// error checking.

int main(int argc, char **argv) {

    int alive;

    //Various data spaces to hold different worlds

    long (*baseWorld)[N], (*baseWorld1)[N], (*baseAnswer)[N],
    (*trialWorld)[N], (*trialWorld1)[N], (*trialWorldAnswer)[N];
    baseWorld = calloc(N * N, sizeof(long));
    trialWorld = calloc(N * N, sizeof(long));
    baseAnswer = calloc(N * N, sizeof(long));
    baseWorld1 = calloc(N * N, sizeof(long));
    trialWorld1 = calloc(N * N, sizeof(long));
    trialWorldAnswer = calloc(N * N, sizeof(long));

    // Number trials to to run
    int trial, ntrials = 10;
    long thistime, besttime = LONG_MAX, times[ntrials];

    // Populate a world. If you want to use the same sequence
    // of random numbers for testing/debugging change the following
    // line to random()
    startClock();
    srandom(start_ts.tv_usec);
    init(baseWorld);

    initLife();

    if (PARANOID) copy(baseWorld, trialWorld);
    if (PARANOID && notIdentical(baseWorld, trialWorld))
        printf("Got the wrong answer on copy\n");
    if (PARANOID) base_life(baseWorld, baseWorld1);
    if (PARANOID) base_life(baseWorld1, baseAnswer);

    printf("Starting the game of life with an %d x %d world for %d trials\n\n", N, N, ntrials);

    for (trial = 0; trial < ntrials; ++trial) {
        startClock();
        base_life(trialWorld, trialWorld1);
        alive = base_life(trialWorld1, trialWorldAnswer);
        endClock();
        thistime = usecClock();
        times[trial] = thistime;

        printf("Base implementation run  %d done, alive %d \n", trial + 1 , alive);
        //      printWorld(trialWorldAnswer);

        // Check that things weren't corrupted
        if (PARANOID && notIdentical(baseWorld, trialWorld))
            printf("Corruption\n");

        if (PARANOID && notIdentical(trialWorldAnswer, baseAnswer))
            printf("problem\n");

        if (thistime < besttime)
            besttime = thistime;

    }
    printf("\nThe best time for the base implementation round of life took: %ld usec\n", besttime);
    if (VERBOSE && ntrials > 1) {
        for (trial = 0; trial < ntrials; ++trial) {
            printf(" Basic run[%d] = %ld\n", trial, times[trial]);
        }
    }

    printf("\nStarting the optimized version\n\n");

    besttime = LONG_MAX;
    for (trial = 0; trial < ntrials; ++trial) {
        startClock();
        life(trialWorld, trialWorld1);
        alive = life(trialWorld1, trialWorldAnswer);
        endClock();
        thistime = usecClock();
        times[trial] = thistime;

        printf("Optimized game of life run %d done, alive %d \n", trial, alive);

        if (PARANOID && notIdentical(baseWorld, trialWorld))
            printf("Corruption\n");

        if (PARANOID && notIdentical(trialWorldAnswer, baseAnswer))
            printf("problem\n");

        if (thistime < besttime)
            besttime = thistime;
    }
    printf("\nThe best time for the optimized run of the game of life took: %ld usec\n", besttime);
    if (VERBOSE && ntrials > 1) {
        for (trial = 0; trial < ntrials; ++trial) {
            printf(" Optimized run[%d] = %ld\n", trial + 1, times[trial]);
        }
    }

    return 0;
}
