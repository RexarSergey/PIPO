#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int args, char *argv[]) {

    if (args != 2) {
        printf("Number of \"args\" must be 2\n");
        return -1;
    }

    // Check if command limit is positive
    if (atoi(argv[1]) < 1) {
        printf("Min command limit number must be 1\n");
        return -1;
    }

    // Creating shared memory
    int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Shmget error\n");
        return -1;
    }

    // Creating counter in shared memory
    int* max_command_count = (int*)shmat(shmid, NULL, 0);
    max_command_count[0] = atoi(argv[1]);

    // Do while user on stdin stream focus (don't push "Ctrl + C" or "Ctrl + D")
    while(!feof(stdin)) {
        char command[FILENAME_MAX] = {};
        // Read command
        scanf("%s", command);

        if (max_command_count[0] > 0) {
            // Creat child process to run command and change max_command_count param
            pid_t pid = fork();
            if(pid == -1) {
                printf("Fork error\n");
                return -1;
            } else if (pid == 0) {
                --max_command_count[0];
                system(command);
                ++max_command_count[0];
                return 0;
            }
        } else {
            printf("Processes limit reached. Wait for ending of other processes\n");
        }
    }

    // Waiting of child processes endings
    while (wait(NULL) != -1) {};
    // Delete shared memory
    shmdt(max_command_count);
    shmctl(shmid, IPC_RMID, NULL);
}
