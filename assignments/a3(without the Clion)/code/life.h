#ifndef LIFE_H
#define LIFE_H

#define N (4 * 1024)

void initLife();
int life(long oldWorld[N][N], long newWorld[N][N]);
int base_life(long oldWorld[N][N], long newWorld[N][N]);
int checkHealth(long newCell, long oldCell);
void printWorld(long world[N][N]);

#endif

