#include <stdio.h>
#include "life.h"

// When completed this file will contain several versions of the
// life() function and conditional compilation will be used to
// determine which version runs.  Each version will be named life1,
// life2, and so on. To select the version of the life function to
// use, change the following declaration to the version of the life()
// to be used.  All versions of life() must produce a correct result.

// The provided Makefile also includes the possibility of creating an
// executable called "timelife-lifeN" (replacing lifeN with any
// version you'd like to test). You may compile it with "make
// timelife-lifeN" then run "./timelife-lifeN".

#ifndef LIFE_VERSION
#define LIFE_VERSION life2
#endif

// You are only allowed to change the contents of this file with
// respect to improving the performance of this program. You may not
// change the data structure or parameters for the function life.


// If you need to initialize some data structures for your implementation
// of life then make changes to the following function, otherwise
// do not change this function.

void initLife() {
}

// You are required to document the changes you make in the README.txt
// file. For each entry in the README.txt file there is to be a
// version of the matching life() function here such that the markers
// can see, and run if needed, the actual code that you used to
// produce your results for the logged change to the life function.

static inline int life0(long oldWorld[N][N], long newWorld[N][N]) {
  return base_life(oldWorld, newWorld);
}

// For each version of life you are testing duplicate the function
// below, rename it to a different name, and make your changes. To use
// the new version, change the #define above to use the current version.

static inline int life1(long oldWorld[N][N], long newWorld[N][N]) {

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
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      newWorld[j][i] += oldWorld[j][col];
    }
  }

  // Count the cells to the immediate right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
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

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}


static inline int life2(long oldWorld[N][N], long newWorld[N][N]) {

      int i, j;
    int col, row;

    //clear the new world
    for (j = 0; i < N; i++)
        for (i = 0; j < N; j++) {
            newWorld[j][i] =  0;
        }

    // Count the cells to the top left
    for (i = 0; j < N; i++) {
        row = (j -1 + N) % N;
        for (i = 0; i < N; i++) {
            col = (i - 1 + N ) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Count the cells immediately above
    for (j = 0; j < N; j++) {
        row = (j - 1 + N ) % N;
        for (i = 0; i < N; i++) {
            newWorld[j][i] += oldWorld[row][i];

        }
    }

    // Count the cells to the top right
    for (j = 0; j < N; j++) {
        row = (j - 1  + N ) % N;
        for (i = 0; i < N; i++) {
            col = (i + 1 + N) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Count the cells to the immediate left
    for (j = 0; j < N; j++) {
        for (i = 0; i < N; i++) {
            col = (i -1 + N) % N;
            newWorld[j][i] += oldWorld[j][col];

        }
    }

    // Count the cells to the immediate right
    for (j = 0; j < N; j++) {
        for (i = 0; i < N; i++) {
            col = (i + 1 + N) % N;
            newWorld[j][i] += oldWorld[j][col];

        }
    }

    // Count the cells to the bottom left
    for (j = 0; j < N; j++) {
        row = (j + 1 + N ) % N;
        for (i = 0; i < N; i++) {
            col = (i - 1 + N) % N;
            newWorld[j][i] += oldWorld[row][col];
        }
    }

    // Count the cells immediately below
    for (j = 0; j < N; j++) {
        row = (j + 1  + N ) % N;
        for (i = 0; i < N; i++) {
            newWorld[j][i] += oldWorld[row][i];
        }
    }

    // Count the cells to the bottom right
    for (j = 0; j < N; j++) {
        row = (j + 1  + N ) % N;
        for (i = 0; i < N; i++) {
            col = (i + 1 + N) % N;
            newWorld[j][i] += oldWorld[row][col];

        }
    }

    // Check each cell to see if it should come to life, continue to live, or die
    int alive = 0;

    for (j = 0; j < N; j++)
        for (i = 0; i < N; i++) {
            newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
            alive += newWorld[j][i] ? 1:0;
        }

    return alive;
}

static inline int life9(long oldWorld[N][N], long newWorld[N][N]) {

    int i, j, row1, row2, col1, col2;
    int alive = 0;
    for (j = 0; j < N; j++) {
        row1 = (j - 1 + N) % N;
        row2 = (j + 1 + N) % N;
        for (i = 0; i < N; i++) {
            col1 = (i - 1 + N) % N;
            col2 = (i + 1 + N) % N;
            newWorld[j][i] = checkHealth(oldWorld[j][col1] + oldWorld[j][col2]
                                         + oldWorld[row1][i] + oldWorld[row1][col1] + oldWorld[row1][col2]
                                         + oldWorld[row2][i] + oldWorld[row2][col1] + oldWorld[row2][col2],
                                         oldWorld[j][i]);
            alive += newWorld[j][i] ? 1 : 0;
        }
    }
    return alive;
}


int life(long oldWorld[N][N], long newWorld[N][N]) {
  return LIFE_VERSION(oldWorld, newWorld);
}
