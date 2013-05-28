/*
 * This code is from "Algorithms in C, Third Edition,"
 * by Robert Sedgewick, Addison Wesley Longman, 1998.
 */
#include <stdio.h>
#include <stdlib.h>
#include "Item.h"

int ITEMscan(Item *x) {
    int t;
    t = scanf("%d", x);
    return t;
}

void ITEMshow(Item x) {
    printf("%d\n", x);
}

Item ITEMrand() {
    return rand();
}