#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>


int factorial(int start, int end) {
    int result = 1;
    for (int i = start; i <= end; ++i) {
        result *= i;
    }
    return result;
}

int main(int args, char *argv[]) {

    int process_number = 1;

    if (args == 2) {
        process_number = atoi(argv[1]);
    } else  if (args == 1) {
        process_number = sysconf(_SC_NPROCESSORS_ONLN) + 1;
    } else {
        printf("Too many arguments\n");
        return -1;
    }

    // Set and check process number
    if (process_number < 1) {
        printf("Min process number must be 1\n");
        return -1;
    }

    // Ask for number for factorial calculation
    int n;
    printf("Enter a number: ");
    scanf("%d", &n);

    if (n == 0 || n == 1) {
        printf("%d! = 1", n);
        return 0;
    }

    // Remove extra processes
    if (n < process_number) {
        process_number = n;
    }

    // List of processes
    pid_t *pid_list = (pid_t*) malloc(process_number * sizeof(pid_t));

    // Creating shared memory
    int shmid = shmget(IPC_PRIVATE, (process_number * sizeof(int)), IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Shmget error\n");
        return -1;
    }

    // Creating param in shared memory
    int* factorial_result = (int*)shmat(shmid, NULL, 0);
    for (int i = 0; i < process_number; ++i) {
        factorial_result[i] = 1;
    }

    int len = n / process_number;
    int start = 1;
    for (int i = 0; i < process_number; ++i, start += len) {
        int end = start + len - 1;
        if (i == (process_number - 1)) {
            end = n;
        }

        // Create child process
        pid_list[i] = fork();

        if(pid_list[i] == -1) {
            printf("Fork error\n");
            return -1;
        } else if (pid_list[i] == 0) {
            // Calculate factorial part
            factorial_result[i] *= factorial(start, end);
            return 0;
        }
    }


    // Waiting of child processes endings
    while (wait(NULL) != -1) {};

    // Print result
    int result = 1;
    for (int i = 0; i < process_number; ++i) {
        result *= factorial_result[i];
    }
    printf("%d! = %d\n", n, result);

    // Free a memory
    free(pid_list);
    // Delete shared memory
    shmdt(factorial_result);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
