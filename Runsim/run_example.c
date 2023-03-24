#include <stdio.h>
#include <unistd.h>

int main(int args, char *argv[]) {
    printf("Starting some process.\n");
    sleep(5);
    printf("End some process.\n");

    return 0;
}
