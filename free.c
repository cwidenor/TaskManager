#include "process_info.h"
#include "free.h"

/*
 * Frees memory map of a process
 */

void freeMemMap(struct memMap *temp, int size) {
    for (int i = 0; i < size; i++) {
        free(temp[i].dev);
        free(temp[i].filename);
        free(temp[i].flags);
        free(temp[i].inode);
        free(temp[i].VMEnd);
        free(temp[i].VMOffset);
        free(temp[i].VMStart);

        temp[i].dev = NULL;
        temp[i].filename = NULL;
        temp[i].flags = NULL;
        temp[i].inode = NULL;
        temp[i].VMEnd = NULL;
        temp[i].VMOffset = NULL;
        temp[i].VMStart = NULL;
    }
    free(temp);
    temp = NULL;
} /* freeMemMap() */

/*
 * Frees the main process list
 */

void freeProcessList(struct process *temp, int size) {
    for (int i = 0; i < size; i++) {
        free(temp[i].name);
        free(temp[i].status);
        free(temp[i].user);

        temp[i].name = NULL;
        temp[i].status = NULL;
        temp[i].user = NULL;
    }
    free(temp);
    temp = NULL;
} /* freeProcessList() */

/*
 * Frees the system info struct
 */

void freeSystemInfo(struct system temp) {
    free(temp.kernelVersion);
    free(temp.OSVersion);
    free(temp.processorVersion);

    temp.kernelVersion = NULL;
    temp.OSVersion = NULL;
    temp.processorVersion = NULL;
} /* freeSystemInfo */

/*
 * Function to free detail view
 */

void freeDetailView(struct detailView temp) {
    free(temp.procName);
    free(temp.user);
    free(temp.status);
    //free(temp.memory);
    free(temp.vMemory);
    free(temp.rMemory);
    free(temp.sMemory);
    free(temp.cpuTime);
    free(temp.startTime);

    temp.procName = NULL;
    temp.user = NULL;
    temp.status = NULL;
    //temp.memory = NULL;
    temp.vMemory = NULL;
    temp.rMemory = NULL;
    temp.sMemory = NULL;
    temp.cpuTime = NULL;
    temp.startTime = NULL;
} /* freeDetailView() */

/*
 * Function to free mounts list
 */

void freeMounts(struct mount *temp, int size) {
    for (int i = 0; i < size; i++) {
        free(temp[i].device);
        free(temp[i].directory);
        free(temp[i].type);

        temp[i].device = NULL;
        temp[i].directory = NULL;
        temp[i].type = NULL;
    }

    free(temp);
    temp = NULL;
} /* freeMounts() */

/*
 * Function to free load avg
 */

void freeLoadAvg(char *temp) {
    free(temp);
    temp = NULL;
} /* freeLoadAvg() */

/*
 * Function to free process files
 */

void freeProcFiles(struct pFile *temp, int size) {
    for (int i = 0; i < size; i++) {
        free(temp[i].object);
        free(temp[i].type);

        temp[i].object = NULL;
        temp[i].type = NULL;
    }

    free(temp);
    temp = NULL;
} /* freeProcFiles() */