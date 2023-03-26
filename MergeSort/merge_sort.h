#pragma once

#include <malloc.h>


typedef struct thread_args thread_args;
static int shift;
static int thread_num;
static int offset;
static int arr_len;
static int *array;


// Merge left and right parts into array
void merge(int *arr, int left, int middle, int right) {
    int left_len = middle - left + 1;
    int right_len = right - middle;
    int left_arr[left_len];
    int right_arr[right_len];

    // Fill left part
    for (int i = 0; i < left_len; i ++) {
        left_arr[i] = arr[left + i];
    }

    // Fill right part
    for (int j = 0; j < right_len; j ++) {
        right_arr[j] = arr[middle + 1 + j];
    }

    int i = 0;
    int j = 0;
    int k = 0;
    // Chose from right and left arrays and copy
    while (i < left_len && j < right_len) {
        if (left_arr[i] <= right_arr[j]) {
            arr[left + k] = left_arr[i];
            i ++;
        } else {
            arr[left + k] = right_arr[j];
            j ++;
        }
        k ++;
    }

    // Copy the remaining values to the array
    while (i < left_len) {
        arr[left + k] = left_arr[i];
        k ++;
        i ++;
    }
    while (j < right_len) {
        arr[left + k] = right_arr[j];
        k ++;
        j ++;
    }
}

// Perform merge sort
void merge_sort(int *arr, int left, int right) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        merge_sort(arr, left, middle);
        merge_sort(arr, middle + 1, right);
        merge(arr, left, middle, right);
    }
}

// Merge sort function for threads
void *thread_merge_sort(void *arg) {
    int thread_id = (int)arg;
    int left = thread_id * shift;
    int right = (thread_id + 1) * shift - 1;

    if(thread_id == thread_num - 1) {
        right += offset;
    }
    int middle = left + (right - left) / 2;
    if(left < right) {
        merge_sort(array, left, right);
    }
}

// Merge locally sorted sections
void merge_sections_of_array(int *arr, int number, int aggregation) {
    for(int i = 0; i < number; i += 2) {
        int left = i * (shift * aggregation);
        int right = ((i + 2) * shift * aggregation) - 1;
        int middle = left + (shift * aggregation) - 1;
        if (right >= arr_len) {
            right = arr_len - 1;
        }
        merge(arr, left, middle, right);
    }
    if (number / 2 >= 1) {
        merge_sections_of_array(arr, number / 2, aggregation * 2);
    }
}