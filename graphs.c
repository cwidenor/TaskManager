#include "graphs.h"

/*
 * Thread function for keeping track of memory history
 */

void *getMemHistory(void *id) {
    int total;
    int freed;
    
    while (1) {
        sleep(1);
        FILE *meminfo = fopen("/proc/meminfo", "r");
        fscanf(meminfo, "MemTotal: %d kB MemFree: %d", &total, &freed);
        fclose(meminfo);

        for (int i = 0; i < 60; i++) {
            gMemHist[i] = gMemHist[i + 1];
        }
        gMemHist[60] = (1 - (double)freed / total) * 100;
    }
    return NULL;
} /* getMemHistory() */

/*
 * Thread function for keeping track of swap history
 */

void *getSwapHistory(void *id) {
    int total;
    int freed;
    
    while (1) {
        sleep(1);
        FILE *memInfo = fopen("/proc/meminfo", "r");

        char *buf = malloc(MAX_LINE_SIZE);
        while (fgets(buf, MAX_LINE_SIZE, memInfo)) {
            if (strstr(buf, "SwapT")) {
                char *size = strstr(buf, ":") + 1;
                while (*size == ' ' || *size == '\t') {
                    size = size + 1;
                }
                int numSize = 0;
                while ((size + numSize)[0] != ' ' && (size + numSize)[0] != '\t') {
                    numSize++;
                }
                
                char *temp = strndup(size, numSize);
                total = atoi(temp);
                
                free(temp);
                temp = NULL;
            } else if (strstr(buf, "SwapF")) {
                char *size = strstr(buf, ":") + 1;
                while (*size == ' ' || *size == '\t') {
                    size = size + 1;
                }
                int numSize = 0;
                while ((size + numSize)[0] != ' ' && (size + numSize)[0] != '\t') {
                    numSize++;
                }
                
                char *temp = strndup(size, numSize);
                freed = atoi(temp);
                
                free(temp);
                temp = NULL;
                break;
            }
        }
        fclose(memInfo);
        free(buf);
        buf = NULL;

        for (int i = 0; i < 60; i++) {
            gSwapHist[i] = gSwapHist[i + 1];
        }
        gSwapHist[60] = (1 - (double)freed / total) * 100;
    }
    return NULL;
} /* getSwapHistory() */

/*
 * Function to get network history graph
 */

void *getNetworkHistory(void *id) {
    char *buf = malloc(MAX_LINE_SIZE);
    char *type = malloc(MAX_LINE_SIZE);

    FILE *devFile = fopen("/proc/net/dev", "r");
    fgets(buf, MAX_LINE_SIZE, devFile);
    fgets(buf, MAX_LINE_SIZE, devFile);    
    
    long rec;
    long tran;
    long recTotal;
    long tranTotal;
    long oldRec = 0;
    long oldTran = 0;
    while (fscanf(devFile, "%s %ld %*d %*d %*d %*d %*d %*d %*d %ld %*d %*d %*d %*d %*d %*d %*d", type, &rec, &tran) == 3) {
        if (strstr(type, "eth")) {
            oldRec += rec;
            oldTran += tran;        
        }
    }
    fclose(devFile);

    while (1) {
        sleep(1);
        FILE *devFile = fopen("/proc/net/dev", "r");
        fgets(buf, MAX_LINE_SIZE, devFile);
        fgets(buf, MAX_LINE_SIZE, devFile);    
        
        recTotal = 0;
        tranTotal = 0;
        while (fscanf(devFile, "%s %ld %*d %*d %*d %*d %*d %*d %*d %ld %*d %*d %*d %*d %*d %*d %*d", type, &rec, &tran) == 3) {
            if (strstr(type, "eth")) { 
                recTotal += rec;
                tranTotal += tran;           
            }
        }
        fclose(devFile);

        for (int i = 0; i < 60; i++) {
            gNetRecieve[i] = gNetRecieve[i + 1];
            gNetSend[i] = gNetSend[i + 1];
        }
        gNetRecieve[60] = (double) (recTotal - oldRec) / 1024;
        gNetSend[60] = (double) (tranTotal - oldTran) / 1024;
        
        oldRec = recTotal;
        oldTran = tranTotal;
    }
} /* getNetworkHistory() */

/*
 * Function to get CPU history graph
 */

void *getCpuHistory(void *id) {
    long user;
    long system;
    long idle;
    long oldCpu;
    long oldTotal;

    FILE *statFile = fopen("/proc/stat", "r");
    fscanf(statFile, "%*s %ld %*d %ld %ld", &user, &system, &idle);
    fclose(statFile);

    oldCpu = user + system;
    oldTotal = oldCpu + idle;

    while (1) {
        sleep(1);
        FILE *statFile = fopen("/proc/stat", "r");
        fscanf(statFile, "%*s %ld %*d %ld %ld", &user, &system, &idle);
        fclose(statFile);

        for (int i = 0; i < 60; i++) {
            gCpuHist[i] = gCpuHist[i + 1];
        }
        gCpuHist[60] = ((double)(user + system - oldCpu) / (user + system + idle - oldTotal)) * 100;

        oldCpu = user + system;
        oldTotal = oldCpu + idle;
    }

} /* getCpuHistory() */

/*
 * Function to initialize all graph arrays
 */

void graphInit() {
    for (int i = 0; i < 61; i++) {
        gMemHist[i] = 0.0;
        gSwapHist[i] = 0.0;
        gNetSend[i] = 0.0;
        gNetRecieve[i] = 0.0;
        gCpuHist[i] = 0.0;
    }
} /* graphInit() */
