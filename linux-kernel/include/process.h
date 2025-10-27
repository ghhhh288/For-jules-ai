#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"
#include "paging.h"

#define KERNEL_STACK_SIZE 8192

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_TERMINATED
} task_state_t;

typedef struct task {
    uint32_t pid;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    page_directory_t *page_directory;
    uint32_t kernel_stack;
    task_state_t state;
    struct task *next;
} task_t;

void tasking_init(void);
void task_switch(void);
int fork(void);
int getpid(void);
void task_exit(void);

#endif
