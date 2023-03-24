#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


int result = 1;
pthread_mutex_t mutex;

typedef struct thread_args thread_args;
struct thread_args {
    int start;
    int end;
};

void *factorial(void *arg) {
    thread_args *args = (thread_args*) arg;

    int a = 1;
    for (int i = args->start; i <= args->end; ++i) {
        a *= i;
    }

    // Prevent incorrect access to the param
    pthread_mutex_lock(&mutex);
    result *= a;
    pthread_mutex_unlock(&mutex);

}

int main(int args, char *argv[]) {

    int thread_number = 1;

    if (args == 2) {
        thread_number = atoi(argv[1]);
    } else  if (args == 1) {
        thread_number = sysconf(_SC_NPROCESSORS_ONLN) + 1;
    } else {
        printf("Too many arguments\n");
        return -1;
    }

    // Set and check thread number
    if (thread_number < 1) {
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

    // Remove extra threads
    if (n < thread_number) {
        thread_number = n;
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex init error\n");
        return -1;
    }

    // List of threads an their params
    pthread_t *thread_id_list = (pthread_t*) malloc(thread_number * sizeof(pthread_t));
    thread_args *t_args = (thread_args*) malloc(thread_number * sizeof(thread_args));

    int len = n / thread_number;
    int start = 1;
    int err;
    for (int i = 0; i < thread_number; ++i, start += len) {
        int end = start + len - 1;
        if (i == (thread_number - 1)) {
            end = n;
        }

        thread_args *ta = &t_args[i];
        ta->start = start;
        ta->end = end;

        err = pthread_create(&thread_id_list[i], NULL, factorial, ta);

        if (err) {
            printf("Thread creation failed\n");
            return -1;
        }
    }

    for (int i = 0; i < thread_number; ++i) {
        pthread_join(thread_id_list[i], NULL);
    }

    // Print result
    printf("%d! = %d\n", n, result);

    // Free a memory
    free(thread_id_list);
    free(t_args);
}
