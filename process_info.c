#include "process_info.h"
#include "process_actions.h"
#include "free.h"
#include "graphs.h"
#include "file_systems.h"

/*
 * Function for getting system mounts
 */

struct mount *getMounts(int *count) {
    struct mount *mountList = malloc(0);
    
    char *filename = strdup("/proc/mounts");
    FILE *mountFile = fopen(filename, "r");
    free(filename);
    filename = NULL;

    char *device = malloc(MAX_LINE_SIZE);
    char *point = malloc(MAX_LINE_SIZE);
    char *type = malloc(MAX_LINE_SIZE);
    int mountCount = 0;
    while (fscanf(mountFile, "%s %s %s %*s %*d %*d", device, point, type) == 3) {
        struct mount temp;

        temp.device = strdup(device);
        temp.directory = strdup(point);
        temp.type = strdup(type);

        struct statfs stats;
        statfs(point, &stats);

        temp.total = stats.f_blocks * (stats.f_bsize / 1073741824.0);
        temp.free = stats.f_bfree * (stats.f_bsize / 1073741824.0);
        temp.available = stats.f_bavail * (stats.f_bsize / 1073741824.0);
        temp.used = temp.total - temp.free;

        mountList = realloc(mountList, sizeof(struct mount) * (++mountCount));
        mountList[mountCount - 1] = temp;
    }
    fclose(mountFile);
    free(device);
    free(point);
    free(type);
    device = NULL;
    point = NULL;
    type = NULL;


    *count = mountCount;
    return mountList;
} /* getMounts() */

/*
 * Gets open files for a process
 */

struct pFile *getProcessFiles(int pid, int *count) {
    struct pFile *fileList = malloc(0);

    char *filename = malloc(20);
    sprintf(filename, "/proc/%d/fd", pid);

    DIR *direc = opendir(filename);
    free(filename);
    filename = NULL;
    if (direc == NULL) {
        struct pFile temp;
        temp.fd = 0;
        temp.object = strdup("Permission Denied");
        temp.type = strdup("");

        fileList = malloc(sizeof(struct pFile));
        fileList[0] = temp;

        *count = 1;
        return fileList;
    }

    struct dirent *dirFile;

    int fileCount = 0;
    while ((dirFile = readdir(direc)) != NULL) {
        if (isdigit(dirFile->d_name[0])) {
            struct pFile temp;

            temp.fd = atoi(dirFile->d_name);

            char *path = malloc(30);
            sprintf(path, "/proc/%d/fd/%s", pid, dirFile->d_name);

            char buf[1024];
            memset(buf, 0, sizeof(buf));
            readlink(path, buf, sizeof(buf) - 1);
            temp.object = strdup(buf);

            struct stat stats;
            stat(path, &stats);

            free(path);
            path = NULL;

            switch (stats.st_mode & S_IFMT) {
            case S_IFSOCK:
                temp.type = strdup("Socket");
                break;
            case S_IFLNK:
                temp.type = strdup("Symbolic Link");
                break;
            case S_IFREG:
                temp.type = strdup("Regular File");
                break;
            case S_IFBLK:
                temp.type = strdup("Block Device");
                break;
            case S_IFDIR:
                temp.type = strdup("Directory");
                break;
            case S_IFCHR:
                temp.type = strdup("Character Device");
                break;
            case S_IFIFO:
                temp.type = strdup("FIFO");
                break;
            default:
                temp.type = strdup("Unknown");
                break;
            }

            fileList = realloc(fileList, sizeof(struct pFile) * (++fileCount));
            fileList[fileCount - 1] = temp;
        }
    }
    closedir(direc);

    *count = fileCount;
    return fileList;
} /* getProcessFiles() */

/*
 * Gets load avg for last 1,5,15 mins
 */ 

char *getLoadAvgString() {
    FILE *loadInfo = fopen("/proc/loadavg", "r");
    char *min1 = malloc(10);
    char *min5 = malloc(10);
    char *min15 = malloc(10);
    fscanf(loadInfo, "%s %s %s", min1, min5, min15);
    fclose(loadInfo);

    char *buf = malloc(45 + strlen(min1) + strlen(min5) + strlen(min15) + 5);
    strcpy(buf, "Load averages for the last 1, 5, 15 minutes: ");
    strcat(buf, min1);
    strcat(buf, ", ");
    strcat(buf, min5);
    strcat(buf, ", ");
    strcat(buf, min15);

    free(min1);
    free(min5);
    free(min15);
    min1 = NULL;
    min5 = NULL;
    min15 = NULL;

    return buf;
} /* getLoadAvg() */

/*
 * Get detailed view of a process
 */

struct detailView getProcessDetails(struct process proc) {
    struct detailView tempDet;

    // set values from proc struct
    tempDet.pid = proc.id;
    tempDet.procName = strdup(proc.name);
    tempDet.user = strdup(proc.user);
    tempDet.status = strdup(proc.status);
    tempDet.memory = strdup(proc.memory);

    // set virtual and resident set sizes
    char *statusFile = malloc(6 + 8 + 7 + 1);
    sprintf(statusFile, "/proc/%d/status", proc.id);

    FILE *statusInfo = fopen(statusFile, "r");
    free(statusFile);
    statusFile = NULL;

    int vFound = 0;
    int rFound = 0;
    int sFound = 0;
    char *buf = malloc(MAX_LINE_SIZE);
    while (fgets(buf, MAX_LINE_SIZE, statusInfo)) {
        if (strstr(buf, "VmSize")) {
            vFound = 1;
            char *size = strstr(buf, ":") + 1;
            while (*size == ' ' || *size == '\t') {
                size = size + 1;
            }

            int numSize = 0;
            while ((size + numSize)[0] != '\n') {
                numSize++;
            }
            tempDet.vMemory = strndup(size, numSize);
        }

        if (strstr(buf, "VmRSS")) {
            rFound = 1;
            char *size = strstr(buf, ":") + 1;
            while (*size == ' ' || *size == '\t') {
                size = size + 1;
            }

            int numSize = 0;
            while ((size + numSize)[0] != '\n') {
                numSize++;
            }
            tempDet.rMemory = strndup(size, numSize);
        }

        if (strstr(buf, "RssShmem")) {
            sFound = 1;
            char *size = strstr(buf, ":") + 1;
            while (*size == ' ' || *size == '\t') {
                size = size + 1;
            }

            int numSize = 0;
            while ((size + numSize)[0] != '\n') {
                numSize++;
            }
            tempDet.sMemory = strndup(size, numSize);

            break;
        }
    }
    fclose(statusInfo);
    free(buf);
    buf = NULL;

    if (!sFound) {
        tempDet.sMemory = strdup("0 kB");
    }
    if (!rFound) {
        tempDet.rMemory = strdup("0 kB");
    }
    if (!vFound) {
        tempDet.vMemory = strdup("0 kB");
    }    

    // set start time
    char *statFile = malloc(6 + 8 + 5 + 1);
    sprintf(statFile, "/proc/%d/stat", proc.id);

    FILE *statInfo = fopen(statFile, "r");
    free(statFile);
    statFile = NULL;

    // get runtime in jiffies and convert to seconds
    double startJiffies = 0.0;
    double umodeJiffies = 0.0;
    double kmodeJiffies = 0.0;
    char *jifStr;
    char *umode;
    char *kmode;
    int column = 0;
    buf = malloc(MAX_LINE_SIZE);
    fgets(buf, MAX_LINE_SIZE, statInfo);
    for (int i = 0; i < strlen(buf); i++) {
        if (buf[i] == ' ') {
            column++;
        } else {
            if (column == 13) {
                int ulen = 0;
                char *umodeStart = &buf[i];
                while (buf[i++] != ' ') {
                    ulen++;
                }

                umode = strndup(umodeStart, ulen);
                umodeJiffies = ((double) strtol(umode, NULL, 10)) / sysconf(_SC_CLK_TCK);
                free(umode);
                umode = NULL;
            } else if (column == 14) {
                int klen = 0;
                char *kmodeStart = &buf[i];
                while (buf[i++] != ' ') {
                    klen++;
                }

                kmode = strndup(kmodeStart, klen);
                kmodeJiffies = ((double) strtol(kmode, NULL, 10)) / sysconf(_SC_CLK_TCK);
                free(kmode);
                kmode = NULL;
            } else if (column == 21) {
                int jifLen = 0;
                char *jifStart = &buf[i];
                while (buf[i++] != ' ') {
                    jifLen++;
                }

                jifStr = strndup(jifStart, jifLen);
                startJiffies = ((double) strtol(jifStr, NULL, 10)) / sysconf(_SC_CLK_TCK);

                free(buf);
                free(jifStr);
                buf = NULL;
                jifStr = NULL;
                fclose(statInfo);
                break;
            }
        }
    }
    
    // set cpu time
    double totalJif = umodeJiffies + kmodeJiffies;
    int hour = totalJif / 3600;
    totalJif -= hour * 3600;
    int minute = totalJif / 60;
    totalJif -= minute  * 60;

    char *cpuTime = malloc(12);
    sprintf(cpuTime, "%d:%02d:%02d", hour, minute, (int)totalJif);
    tempDet.cpuTime = strdup(cpuTime);

    free(cpuTime);
    cpuTime = NULL;

    // get total uptime of system
    FILE *uptimeInfo = fopen("/proc/uptime", "r");
    double uptime;
    fscanf(uptimeInfo, "%lf", &uptime);
    fclose(uptimeInfo);

    // get current time
    time_t rawtime;
    time (&rawtime );
    rawtime -= (uptime - startJiffies);
    struct tm *timeinfo = localtime(&rawtime);
    
    buf = malloc(100);
    sprintf(buf, "%s", asctime(timeinfo));
    tempDet.startTime = strndup(buf, strlen(buf));
    free(buf);
    buf = NULL;
    
    return tempDet;
} /* getProcessDetails() */

/*
 * Get memory map info for a process
 */

struct memMap *getMemMap(int pid, int *count) {
    struct memMap *mapList = malloc(0);

    char mapPath[MAX_FILE_LENGTH];
    sprintf(mapPath, "/proc/%d/maps", pid);
    FILE *memfile = fopen(mapPath, "r");
    
    // If user doesn't have file permissions
    if (!memfile || errno == 13) {
        struct memMap tempMap;
        tempMap.filename = strdup("Permission denied");
        tempMap.flags = strdup("");
        tempMap.VMEnd = strdup("");
        tempMap.VMOffset = strdup("");
        tempMap.VMStart = strdup("");
        tempMap.dev = strdup("");
        tempMap.inode = strdup("");
        tempMap.VMSize = 0;

        *count = 1;
        mapList = realloc(mapList, sizeof(struct memMap));
        mapList[0] = tempMap;
        return mapList;
    }

    struct memMap tempMap;
    char *buf = malloc(MAX_LINE_SIZE);
    int mapCount = 0;
    while (fgets(buf, MAX_LINE_SIZE, memfile)) {
        int bufIndex = 0;
        int wordLen = 0;
        char *wordStart;

        // set vm start
        wordStart = buf;
        while (buf[bufIndex++] != '-') {
            wordLen++;
        }
        tempMap.VMStart = strndup(wordStart, wordLen);
    
        // set vm end
        wordStart = buf + bufIndex;
        wordLen = 0;
        while (buf[bufIndex] != '\t' && buf[bufIndex] != ' ') {
            bufIndex++;
            wordLen++;
        }
        bufIndex++;
        tempMap.VMEnd = strndup(wordStart, wordLen);

        // moving to perm sect
        while (buf[bufIndex] == '\t' || buf[bufIndex] == ' ') {
            bufIndex++;
        }

        // set perm
        wordStart = buf + bufIndex;
        wordLen = 0;
        while (buf[bufIndex] != '\t' && buf[bufIndex] != ' ') {
            bufIndex++;
            wordLen++;
        }
        bufIndex++;
        tempMap.flags = strndup(wordStart, wordLen);

        // moving to offset sect
        while (buf[bufIndex] == '\t' || buf[bufIndex] == ' ') {
            bufIndex++;
        }

        // set offset
        wordStart = buf + bufIndex;
        wordLen = 0;
        while (buf[bufIndex] != '\t' && buf[bufIndex] != ' ') {
            bufIndex++;
            wordLen++;
        }
        bufIndex++;
        tempMap.VMOffset = strndup(wordStart, wordLen);

        // moving to dev sect
        while (buf[bufIndex] == '\t' || buf[bufIndex] == ' ') {
            bufIndex++;
        }

        // set dev
        wordStart = buf + bufIndex;
        wordLen = 0;
        while (buf[bufIndex] != '\t' && buf[bufIndex] != ' ') {
            bufIndex++;
            wordLen++;
        }
        bufIndex++;
        tempMap.dev = strndup(wordStart, wordLen);

        // moving to inode sect
        while (buf[bufIndex] == '\t' || buf[bufIndex] == ' ') {
            bufIndex++;
        }

        // set inode
        wordStart = buf + bufIndex;
        wordLen = 0;
        while (buf[bufIndex] != '\t' && buf[bufIndex] != ' ') {
            bufIndex++;
            wordLen++;
        }
        bufIndex++;
        tempMap.inode = strndup(wordStart, wordLen);

        // set filename
        if (bufIndex >= strlen(buf)) {
            tempMap.filename = strdup("");
        } else {
            // moving to file sect
            while (buf[bufIndex] == '\t' || buf[bufIndex] == ' ') {
                bufIndex++;
            }

            // set file
            wordStart = buf + bufIndex;
            wordLen = 0;
            while (buf[bufIndex] != '\t' && buf[bufIndex] != ' ' && buf[bufIndex] != '\n') {
                bufIndex++;
                wordLen++;
            }
            bufIndex++;
            tempMap.filename = strndup(wordStart, wordLen);
        }

        // setting size
        long start = strtol(tempMap.VMStart, NULL, 16);
        long end = strtol(tempMap.VMEnd, NULL, 16);
        tempMap.VMSize = (int) ((end - start) / 1024);

        mapList = realloc(mapList, ++mapCount * sizeof(struct memMap));
        mapList[mapCount - 1] = tempMap;
        if (feof(memfile)) {
            break;
        }
    }
    free(buf);
    buf = NULL;
    fclose(memfile);

    *count = mapCount;
    return mapList;
} /* getMemMap() */

/*
 * Gets the base system info
 */

struct system getSystemInfo() {
    struct system tempSystem;

    // setting memory size (kB)
    FILE *memInfo = fopen("/proc/meminfo", "r");
    int memSize;
    fscanf(memInfo, "MemTotal:%d", &memSize);
    fclose(memInfo);
    tempSystem.memory = memSize;

    // setting processor
    FILE *procFile = fopen("/proc/cpuinfo", "r");
    char buf[MAX_FILE_LENGTH];
    char *procStr = NULL;
    while (fgets(buf, MAX_LINE_SIZE, procFile)) {
        if (strstr(buf, "model name")) {
            char *processor = strstr(buf, ":") + 2;
            procStr = strndup(processor, strlen(processor) - 1);
            break;
        }
    }
    fclose(procFile);

    // getting num of processors
    FILE *countFile = fopen("/proc/stat", "r");
    char buf2[MAX_FILE_LENGTH];
    int procCount = -1;
    while (1) {
        fscanf(countFile, "%s %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d", buf2);
        if (strstr(buf2, "cpu")) {
            procCount++;
        } else {
            break;
        }
    }
    fclose(countFile);
    tempSystem.processorVersion = malloc(strlen(procStr) + 7);
    sprintf(tempSystem.processorVersion, "%s x %d", procStr, procCount);
    free(procStr);
    procStr = NULL;

    // setting disks
    tempSystem.totalDisk = 0.0;
    tempSystem.availableDisk = 0.0;
    int mountCount;
    struct mount *temp4 = getMounts(&mountCount);
    for (int i = 0; i < mountCount; i++) {
        tempSystem.totalDisk += temp4[i].total;
        tempSystem.availableDisk += temp4[i].available;
    }
    freeMounts(temp4, mountCount);

    // set os release
    FILE *osInfo = fopen("/etc/os-release", "r");
    char *name = malloc(256);
    char *version = malloc(256);
    fscanf(osInfo, "%s\n%[^\n]s", name, version);
    fclose(osInfo);
    
    char *nameTemp = strstr(name, "\"") + 1;
    char *versionTemp = strstr(version, "\"") + 1;
    char *combinedOS = calloc(sizeof(char), strlen(nameTemp) + strlen(versionTemp));
    strncpy(combinedOS, nameTemp, strlen(nameTemp) - 1);
    strcat(combinedOS, " ");
    strncat(combinedOS, versionTemp, strlen(versionTemp) - 1);

    tempSystem.OSVersion = strdup(combinedOS);

    free(name);
    free(version);
    free(combinedOS);
    name = NULL;
    version = NULL;
    combinedOS = NULL;

    // set kernel version
    FILE *kernelInfo = fopen("/proc/version", "r");
    char kernel[MAX_PAGE_SIZE];
    fscanf(kernelInfo, "%[^\n]s", kernel);
    fclose(kernelInfo);

    char *extraInfo = strstr(kernel, "(") - 1;
    
    tempSystem.kernelVersion = strndup(kernel, strlen(kernel) - strlen(extraInfo));

    return tempSystem;
} /* getSystemInfo() */

/*
 * Gets list of processes for main process page
 */

struct process *getProcessList(int type, int *count) {
    struct process *processList = malloc(0);
    int processCount = 0;

    char *currentUser = NULL;
    if (type == USER_PROCESSES) {
        struct passwd *pwd = getpwuid(getuid());
        currentUser = strdup(pwd->pw_name);
    }

    DIR *direc = opendir("/proc");
    struct dirent *dirFile;

    while ((dirFile = readdir(direc)) != NULL) {
        if (isdigit(dirFile->d_name[0])) {
            struct process tempProcess;
                
            // accessing stat file
            char *statFile = malloc(6 + strlen(dirFile->d_name) + 5 + 1);
            sprintf(statFile, "/proc/%s/stat", dirFile->d_name);

            FILE *processInfo = fopen(statFile, "r");
            free(statFile);
            statFile = NULL;

            if (processInfo == NULL) {
                continue;
            }

            // scanning stat file
            fscanf(processInfo, "%d", &tempProcess.id);
            fclose(processInfo);

            // accessing status file
            char *statusFile = malloc(6 + strlen(dirFile->d_name) + 7 + 1);
            sprintf(statusFile, "/proc/%s/status", dirFile->d_name);

            FILE *memInfo = fopen(statusFile, "r");
            free(statusFile);
            statFile = NULL;

            char *buf = malloc(MAX_LINE_SIZE);
            tempProcess.memory = NULL;
            while (fgets(buf, MAX_LINE_SIZE, memInfo)) {
                if (strstr(buf, "VmSize")) {
                    char *size = strstr(buf, ":") + 1;
                    while (*size == ' ' || *size == '\t') {
                        size = size + 1;
                    }

                    int numSize = 0;
                    while ((size + numSize)[0] != '\n') {
                        numSize++;
                    }
                    tempProcess.memory = strndup(size, numSize);
                }
                if (strstr(buf, "Uid")) {
                    char *size = strstr(buf, ":") + 1;
                    while (*size == ' ' || *size == '\t') {
                        size = size + 1;
                    }

                    int numSize = 0;
                    while ((size + numSize)[0] != ' ' && (size + numSize)[0] != '\t') {
                        numSize++;
                    }
                    char *uname = strndup(size, numSize);

                    struct passwd *pws = getpwuid(atoi(uname));
                    
                    tempProcess.user = strdup(pws->pw_name);
                    free(uname);
                    uname = NULL;
                }
                if (strstr(buf, "Name")) {
                    char *size = strstr(buf, ":") + 1;
                    while (*size == ' ' || *size == '\t') {
                        size = size + 1;
                    }

                    int numSize = 0;
                    while ((size + numSize)[0] != '\n') {
                        numSize++;
                    }
                    tempProcess.name = strndup(size, numSize);
                }
                if (strstr(buf, "State")) {
                    char *size = strstr(buf, ":") + 1;
                    while (*size == ' ' || *size == '\t') {
                        size = size + 1;
                    }

                    int numSize = 0;
                    while ((size + numSize)[0] != '\n') {
                        numSize++;
                    }

                    char status = size[0];

                    // set status name
                    switch (status) {
                    case 'R':
                        tempProcess.status = strdup("Running");
                        break;
                    case 'S':
                        tempProcess.status = strdup("Sleeping");
                        break;
                    case 'T':
                        tempProcess.status = strdup("Stopped");
                        break;
                    case 'D':
                        tempProcess.status = strdup("Uninterruptible Sleep");
                        break;
                    case 'W':
                        tempProcess.status = strdup("Paging");
                        break;
                    case 'X':
                        tempProcess.status = strdup("Dead");
                        break;
                    case 'Z':
                        tempProcess.status = strdup("Defunct");
                        break;
                    case 'I':
                        tempProcess.status = strdup("Idle");
                        break;
                    default:
                        tempProcess.status = strdup("Unknown");
                    }
                }
            }
            fclose(memInfo);

            free(buf);
            buf = NULL;

            if (tempProcess.memory == NULL) {
                tempProcess.memory = strdup("0 KiB");
            }

            if (type == ALL_PROCESSES) {
                processList = realloc(processList, sizeof(struct process) * (++processCount));
                processList[processCount - 1] = tempProcess;
            } else if (type == USER_PROCESSES) {
                if (strcmp(currentUser, tempProcess.user) == 0) {
                    processList = realloc(processList, sizeof(struct process) * (++processCount));
                    processList[processCount - 1] = tempProcess;
                } else {
                    free(tempProcess.memory);
                    free(tempProcess.name);
                    free(tempProcess.status);
                    free(tempProcess.user);

                    tempProcess.memory = NULL;
                    tempProcess.name = NULL;
                    tempProcess.status = NULL;
                    tempProcess.user = NULL;
                }
            } else if (type == RUNNING_PROCESSES) {
                if (strcmp(tempProcess.status, "Running") == 0) {
                    processList = realloc(processList, sizeof(struct process) * (++processCount));
                    processList[processCount - 1] = tempProcess;
                } else {
                    free(tempProcess.memory);
                    free(tempProcess.name);
                    free(tempProcess.status);
                    free(tempProcess.user);

                    tempProcess.memory = NULL;
                    tempProcess.name = NULL;
                    tempProcess.status = NULL;
                    tempProcess.user = NULL;
                }
            }
        }
    }
    closedir(direc);

    // calculating cpu %
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;

    FILE *procFile = fopen("/proc/stat", "r");
    fscanf(procFile, "cpu  %ld %ld %ld %ld %ld %ld %ld",
    &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(procFile);
    long sysBefore = user + nice + system + idle + iowait + irq + softirq;
    
    long uBefore[processCount];
    long sBefore[processCount];
    for (int i = 0; i < processCount; i++) {
        char *path = malloc(20);
        sprintf(path, "/proc/%d/stat", processList[i].id);

        FILE *stat = fopen(path, "r");
        free(path);
        path = NULL;
        if (stat != NULL) {
            fscanf(stat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &uBefore[i], &sBefore[i]);
            fclose(stat);
        }
    }

    sleep(1);

    FILE *procFile2 = fopen("/proc/stat", "r");
    fscanf(procFile2, "cpu  %ld %ld %ld %ld %ld %ld %ld",
    &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(procFile2);
    long sysAfter = user + nice + system + idle + iowait + irq + softirq;

    for (int i = 0; i < processCount; i++) {
        char *path = malloc(20);
        sprintf(path, "/proc/%d/stat", processList[i].id);

        FILE *stat = fopen(path, "r");
        free(path);
        path = NULL;

        if (stat != NULL) {
            long utime;
            long stime;
            fscanf(stat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &utime, &stime);
            fclose(stat);

            processList[i].cpuPercent = 100 * (((double)(utime-uBefore[i]) / (sysAfter - sysBefore)) + ((double)(stime-sBefore[i]) / (sysAfter - sysBefore)));
            if (processList[i].cpuPercent < 0.0) {
                processList[i].cpuPercent = 0.0;
            }
        } else {
            processList[i].cpuPercent = 0.0;
        }
    }

    // free if current user needed
    if (type == USER_PROCESSES) {
        free(currentUser);
        currentUser = NULL;
    }

    *count = processCount;
    return processList;
} /* getProcessList */