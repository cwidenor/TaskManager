#include "process_actions.h"

int stopProc(int pid) {
    return kill(pid, SIGSTOP);
}

int continueProc(int pid) {
    return kill(pid, SIGCONT);
}

int killProc(int pid) {
    return kill(pid, SIGKILL);
}