#include "../include/process.h"
#include "../include/kmalloc.h"
#include "../include/string.h"
#include "../include/paging.h"

static task_t *current_task = 0;
static task_t *ready_queue = 0;
static uint32_t next_pid = 1;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;
extern void perform_task_switch(uint32_t eip, uint32_t physical_page_dir, uint32_t ebp, uint32_t esp);

void tasking_init(void) {
    __asm__ volatile("cli");
    
    current_task = (task_t*)kmalloc(sizeof(task_t));
    current_task->pid = next_pid++;
    current_task->esp = 0;
    current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->kernel_stack = 0;
    current_task->state = TASK_RUNNING;
    current_task->next = 0;
    
    ready_queue = current_task;
    
    __asm__ volatile("sti");
}

void task_switch(void) {
    if (!current_task) return;
    
    uint32_t esp, ebp, eip;
    
    __asm__ volatile("mov %%esp, %0" : "=r"(esp));
    __asm__ volatile("mov %%ebp, %0" : "=r"(ebp));
    
    __asm__ volatile("lea 1f, %0\n\t"
                     "1:" : "=r"(eip));
    
    if (eip == 0x12345) {
        return;
    }
    
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;
    
    current_task = current_task->next;
    if (!current_task) current_task = ready_queue;
    
    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;
    
    current_directory = current_task->page_directory;
    
    __asm__ volatile("         \
        cli;                   \
        mov %0, %%ecx;         \
        mov %1, %%esp;         \
        mov %2, %%ebp;         \
        mov %3, %%cr3;         \
        mov $0x12345, %%eax;   \
        sti;                   \
        jmp *%%ecx             "
        : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));
}

int fork(void) {
    __asm__ volatile("cli");
    
    task_t *parent_task = current_task;
    
    page_directory_t *directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(directory, 0, sizeof(page_directory_t));
    
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->pid = next_pid++;
    new_task->esp = 0;
    new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->kernel_stack = kmalloc(KERNEL_STACK_SIZE);
    new_task->state = TASK_READY;
    new_task->next = 0;
    
    task_t *tmp_task = ready_queue;
    while (tmp_task->next) {
        tmp_task = tmp_task->next;
    }
    tmp_task->next = new_task;
    
    uint32_t eip;
    __asm__ volatile("lea 1f, %0\n\t"
                     "1:" : "=r"(eip));
    
    if (current_task == parent_task) {
        uint32_t esp, ebp;
        __asm__ volatile("mov %%esp, %0" : "=r"(esp));
        __asm__ volatile("mov %%ebp, %0" : "=r"(ebp));
        
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        
        __asm__ volatile("sti");
        return new_task->pid;
    } else {
        return 0;
    }
}

int getpid(void) {
    return current_task->pid;
}

void task_exit(void) {
    current_task->state = TASK_TERMINATED;
    task_switch();
}
