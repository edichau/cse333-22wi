// Copyright 2022 Edison Chau - edichau@uw.edu
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

float power(float, int);

int main(int argc, char *argv[]) {
    // I decided that it would be best to give the user an error message
    // that can help them debug why their input did not work.
    // The program ends after the error so they can try again with
    // a correct/different input

    if (isdigit(*argv[1]) == 0) {
        printf("please enter only 1 int, you gave a non int input\n");
        return 0;
    }

    if (argc != 2) {
        printf("please enter only 1 int, you gave more than 1 argument\n");
        return 0;
    }

    int n = atoi(argv[1]);
    double pi = 3.0;
    for (int i = 1; i <= n; i++) {
        pi += (power(-1, i+1) * (4.0 / (2*i * (2*i+1) * (2*i+2))));
    }
    printf("Our estimate of Pi is %.20f\n", pi);
    return 0;
}

float power(float base, int exp) {
    float temp;
    if (exp == 0)
       return 1;
    temp = power(base, exp/2);
    if (exp % 2 == 0) {
        return temp*temp;
    } else {
        if (exp > 0)
            return base*temp*temp;
        else
            return (temp*temp)/base;
    }
}
