void freeMemMap(struct memMap *temp, int size);
void freeProcessList(struct process *temp, int size);
void freeSystemInfo(struct system temp);
void freeDetailView(struct detailView temp);
void freeMounts(struct mount *temp, int size);
void freeLoadAvg(char *temp);
void freeProcFiles(struct pFile *temp, int size);