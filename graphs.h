#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_SIZE (256)

double gMemHist[61];
double gSwapHist[61];
double gNetSend[61];
double gNetRecieve[61];
double gCpuHist[61];

void *getMemHistory(void *id);
void *getSwapHistory(void *id);
void *getNetworkHistory(void *id);
void *getCpuHistory(void *id);

void graphInit();