#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

// Creating/coping files from one directory to another
int copy_file(char *path_from, char *path_to) {

    int file_read = open(path_from, O_RDONLY);
    if (file_read == -1) {
        printf("Can't open file to read\n");
        return 0;
    }

    int file_write = open(path_to, O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
    if (file_write == -1) {
        printf("Can't create/write file\n");
        return 0;
    }

    // Files content
    char buff[4096];
    int read_b;
    int write_b;

    do {
        read_b = read(file_read, &buff, sizeof(buff));
        if (read_b == -1) {
            printf("Reading error\n");
            return 0;
        }

        write_b = write(file_write, &buff, read_b);
        if (write_b == -1) {
            printf("Writing error\n");
            return 0;
        }

        if (write_b != read_b) {
            printf("Smt wrong with writing\n");
            return 0;
        }

    } while (read_b > 0);


    close(file_read);
    close(file_write);
    // Copy finished without problems
    return 1;
}


// Compare content files for both directories
int compare_files(char *path_from, char *path_to) {
    // File content
    char buff_f[4096];
    char buff_t[4096];
    int bytes_f;
    int bytes_t;

    memset(buff_f, 0, sizeof(buff_f));
    memset(buff_t, 0, sizeof(buff_t));

    int file_f = open(path_from, O_RDONLY);
    if (file_f == -1) {
        printf("Can't read file_f to compare content\n");
        return 0;
    }

    int file_t = open(path_to, O_RDONLY);
    if (file_t == -1) {
        printf("Can't read file_t to compare content\n");
        return 0;
    }

    while ((bytes_f = read(file_f, &buff_f, sizeof(buff_f))) > 0 &&
           (bytes_t = read(file_t, &buff_t, sizeof(buff_t))) > 0) {
        // Check if there are some content change
        if (memcmp(buff_f, buff_t, 4096) != 0 || bytes_f != bytes_t) {
            // File has changed
            return 1;
        }
    }

    close(file_f);
    close(file_t);
    // File didn't changed
    return 0;
}

// Backup realization
void backup(char *path_from, char *path_to) {
    // Open directory for coping files
    DIR *dir = opendir(path_from);
    struct dirent *entry = readdir(dir);
    if (!dir || !entry) {
        printf("Can't open directory for coping files\n");
        return;
    }

    do {
        // Path for files to copy
        char file_path_f[FILENAME_MAX];
        int len = snprintf(file_path_f, sizeof(file_path_f) - 1, "%s/%s", path_from, entry->d_name);
        file_path_f[len] = 0;

        // Path for files to create/update
        char file_path_t[FILENAME_MAX];
        len = snprintf(file_path_t, sizeof(file_path_t) - 1, "%s/%s", path_to, entry->d_name);
        file_path_t[len] = 0;

        // Path for files to zip
        char file_path_gz[FILENAME_MAX];
        len = snprintf(file_path_gz, sizeof(file_path_gz) - 1, "%s/%s.gz", path_to, entry->d_name);
        file_path_gz[len] = 0;

        // Directory
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            DIR *dir = opendir(file_path_t);
            if (ENOENT == errno) {
                if (mkdir(file_path_t, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
                    printf("Can't create directory\n");
                }
            }
            backup(file_path_f, file_path_t);
            closedir(dir);
        } else // Files
        {
            // If it's new file to copy
            if (access(file_path_gz, F_OK)) {
                pid_t pid = fork();
                if (pid < 0) {
                    printf("Fork error\n");
                    return;
                } else if (pid == 0) {
                    // Create new file and copy content to them
                    if (copy_file(file_path_f, file_path_t)) {
                        // Create zip for file
                        execlp("gzip", "gzip", file_path_t, NULL);
                    }
                } else {
                    // Wait process
                    wait(NULL);
                }
            } else // Copy or update files
            {
                pid_t pid = fork();
                if (pid < 0) {
                    printf("Fork error\n");
                    return;
                } else if (pid == 0) {
                    pid_t pid2 = fork();
                    if (pid2 < 0) {
                        printf("Fork error\n");
                        return;
                    } else if (pid2 == 0) {
                        execlp("gzip", "gzip", "-d", "-k", "-f", file_path_gz, NULL);
                    } else {
                        wait(NULL);
                        // If file was changed then try copy content from one file to another
                        if (compare_files(file_path_f, file_path_t)) {
                            if (copy_file(file_path_f, file_path_t)) {
                                // Create zip for file
                                execlp("gzip", "gzip", "-f", file_path_t, NULL);
                            }
                        } else {
                            remove(file_path_t);
                        }
                    }
                } else {
                    wait(NULL);
                }
            }
        }
    } while (entry = readdir(dir));
    closedir(dir);
}

int main(int args, char *argv[]) {

    if (args != 3) {
        printf("Number of \"args\" must be 3\n");
        return -1;
    }
    
    backup(argv[1], argv[2]);

    // Waiting of child processes endings
    while (wait(NULL) != -1) {};
    return 0;
}
