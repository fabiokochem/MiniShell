#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>

// Struct to represent a job
typedef struct Job {
    pid_t pid;              // Process ID of the job
    char status[16];        // Status of the job (e.g., Running, Suspended)
    char command[256];      // Command associated with the job
    bool background;        // Indicates if the job runs in the background
} Job;

// Function prototypes
int list_jobs_command(char **args);
void add_job(pid_t pid, const char *status, const char *command, bool background);
void remove_job(pid_t pid);
void resume_in_bg(int job_id);
int handle_bg_command(char **args);

// Global variables
int suspended_count = 0, job_count = 0;  // Track job and suspended counts
Job jobs[1024];                         // Array to store job information
pid_t suspended_pids[1024];             // Array to store suspended job PIDs

// Lists all jobs and their statuses
int list_jobs_command(char **args) {
    printf("Job ID    PID       Status       Command\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < job_count; i++) {
        printf("[%d]    %d       %s       %s\n", i + 1, jobs[i].pid, jobs[i].status, jobs[i].command);
    }
    return 0;
}

// Adds a job to the job list
void add_job(pid_t pid, const char *status, const char *command, bool background) {
    jobs[job_count].pid = pid;
    strncpy(jobs[job_count].status, status, 15);  // Copy job status
    strncpy(jobs[job_count].command, command, 255);  // Copy command
    jobs[job_count].background = background;
    job_count++;
}

// Removes a job from the job list by PID
void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j + 1];  // Shift remaining jobs
            }
            job_count--;
            break;
        }
    }
}

// Resumes a suspended job in the background
void resume_in_bg(int job_id) {
    Job *job = &jobs[job_id - 1];
    kill(job->pid, SIGCONT);  // Send SIGCONT signal to resume the job
}

// Handles background (bg) and foreground (fg) commands
int handle_bg_command(char **args) {
    if (args[1] == NULL) {
        // Resume the last suspended job
        for (int i = job_count - 1; i >= 0; i--) {
            if (strcmp(jobs[i].status, "Suspended") == 0 &&
                ((strcmp(args[0], "bg") == 0 && jobs[i].background) ||
                 (strcmp(args[0], "fg") == 0 && !jobs[i].background))) {
                resume_in_bg(i + 1);
                return 0;
            }
        }
    } else {
        // Resume a specific job by job ID
        int job_id = atoi(args[1]);
        resume_in_bg(job_id);
        return 0;
    }
    return 1;
}
