#pragma once
#include <sys/types.h>

typedef enum { RUNNING, STOPPED, DONE } proc_status;

typedef struct bg_proc {
    pid_t pid;
    char cmd[256];
    proc_status status;
    struct bg_proc *next;
} bg_proc;

void add_bg_proc(pid_t pid, const char *cmd);
void update_bg_status();
void print_bg_list();
void remove_done_procs();
bg_proc *find_bg_proc(pid_t pid);
void set_bg_status(pid_t pid, proc_status status);