#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "merge_sort.h"


// Filling array with random numbers
void fill_array(int *arr, int arr_size, int digits_number) {
    srand(clock());
    for (int i = 0; i < arr_size; ++i) {
        arr[i] = rand() % digits_number;
    }
}

// Print array
void print_array(int *arr, int arr_size) {
    for (int i = 0; i < arr_size; ++i) {
        printf(" %d", arr[i]);
    }
}

int main(int args, char *argv[]) {

    if (args != 3) {
        printf("First arg must be size of array and second one - number of thread\n");
        return -1;
    }

    int arr_size = atoi(argv[1]);
    int thread_count = atoi(argv[2]);

    // Delete extra threads
    if (arr_size < thread_count) {
        thread_count= arr_size;
    }

    printf("Array size: %d\tThread count: %d\n\n", arr_size, thread_count);

    // Set values in merge_sort.h
    shift = arr_size / thread_count;
    thread_num = thread_count;
    offset = arr_size % thread_count;
    arr_len = arr_size;

    // Creating array of numbers and set it in merge_sort.h
    int *arr = (int *) malloc(arr_size * sizeof(int));
    fill_array(arr, arr_size, 1000);
    array = arr;

    // Print array before sorting
    printf("Before sorting: ");
    print_array(arr, arr_size);
    printf("\n");

    // Creating threads
    pthread_t *threads = (pthread_t *) malloc(thread_num * sizeof(pthread_t));
    for (int i = 0; i < thread_count; ++i) {
        pthread_create(&threads[i], NULL, thread_merge_sort, (void*)i);
    }
    for (int i = 0; i < thread_num; ++i) {
        pthread_join(threads[i], NULL);
    }
    merge_sections_of_array(arr, thread_num, 1);

    // Print array after sorting
    printf("After sorting: ");
    print_array(arr, arr_size);
    printf("\n");

    // Free memory
    free(arr);
    free(threads);
    return 0;
}
