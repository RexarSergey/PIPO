#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int args, char *argv[]) {

    if (args != 2) {
        printf("Number of \"args\" must be 2\n");
        return -1;
    }

    // Open main files with params
    FILE *file;
    file = fopen(argv[1], "r");

    if (file == NULL) {
        printf("Error while open the file\n");
        return -1;
    }

    char file_name[FILENAME_MAX] = {};
    int timer = 0;

    // While can set timer and file_name params after parsing file line
    while (fscanf(file, "%d %s", &timer, file_name) == 2) {

        if (timer < 0) {
            printf("Incorrect timer for executing file\n");
            return -1;
        }

        printf("File \"%s\" will be started in (%c.txt) seconds\n", file_name, timer);

        // Creating child process to open file with timer delay
        pid_t pid = fork();
        if (pid == -1) {
            printf("Fork error\n");
            return -1;
        } else if(pid == 0) {
            sleep(timer);
            execlp(file_name, file_name, NULL);
            printf("File \"%s\" was opened\n", file_name);
            return 0;
        }
    }

    // Waiting of child processes endings
    while (wait(NULL) != -1) {};
    // Check if file stream isn't in focus
    if (!feof(file)) {
        printf("File stream error\n");
        return -1;
    }

    // Close file stream
    fclose(file);
    return 0;
}
