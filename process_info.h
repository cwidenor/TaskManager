#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/vfs.h>

#define ALL_PROCESSES (0)
#define USER_PROCESSES (1)
#define RUNNING_PROCESSES (2)
#define MAX_PAGE_SIZE (4096)
#define MAX_LINE_SIZE (256)
#define MAX_FILE_LENGTH (128)

struct process {
    char *name;
    char *status;
    double cpuPercent;
    int id;
    char *memory;
    char *user;
};

struct system {
    char *OSVersion;
    char *kernelVersion;
    int memory;
    char *processorVersion;
    double totalDisk;
    double availableDisk;
};

struct pFile {
    int fd;
    char *type;
    char *object;
};

struct memMap {
    char *filename;
    char *VMStart;
    char *VMEnd;
    char *flags;
    char *VMOffset;
    char *dev;
    char *inode;
    int VMSize;
};

struct detailView {
    int pid;
    char *procName;
    char *user;
    char *status;
    char *memory;
    char *vMemory;
    char *rMemory;
    char *sMemory;
    char *cpuTime;
    char *startTime;
};

struct mount {
    char *device;
    char *directory;
    char *type;
    long total;
    long free;
    long available;
    long used;
};

struct system getSystemInfo();
struct process *getProcessList(int type, int *count);
struct memMap *getMemMap(int pid, int *count);
struct detailView getProcessDetails(struct process proc);
char *getLoadAvgString();
struct pFile *getProcessFiles(int pid, int *count);
struct mount *getMounts(int *count);