#include "processlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

static bg_proc *head = NULL;

void add_bg_proc(pid_t pid, const char *cmd) {
    bg_proc *p = malloc(sizeof(bg_proc));
    p->pid = pid;
    strncpy(p->cmd, cmd, sizeof(p->cmd));
    p->cmd[sizeof(p->cmd)-1] = 0;
    p->status = RUNNING;
    p->next = head;
    head = p;
}

void update_bg_status() {
    bg_proc *p = head;
    int status;
    while (p) {
        if (p->status == RUNNING) {
            pid_t ret = waitpid(p->pid, &status, WNOHANG);
            if (ret == p->pid) p->status = DONE;
        }
        p = p->next;
    }
}

void print_bg_list() {
    update_bg_status();
    bg_proc *p = head;
    while (p) {
        printf("[%d] %s - %s\n", (int)p->pid, p->cmd,
            p->status == RUNNING ? "Running" :
            p->status == STOPPED ? "Stopped" : "Done");
        p = p->next;
    }
}

void set_bg_status(pid_t pid, proc_status status) {
    bg_proc *p = find_bg_proc(pid);
    if (p) p->status = status;
}

bg_proc *find_bg_proc(pid_t pid) {
    bg_proc *p = head;
    while (p) {
        if (p->pid == pid) return p;
        p = p->next;
    }
    return NULL;
}

void remove_done_procs() {
    bg_proc **pp = &head;
    while (*pp) {
        if ((*pp)->status == DONE) {
            bg_proc *tmp = *pp;
            *pp = (*pp)->next;
            free(tmp);
        } else {
            pp = &(*pp)->next;
        }
    }
}