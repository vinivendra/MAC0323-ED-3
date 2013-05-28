/*
 * This code is from "Algorithms in C, Third Edition,"
 * by Robert Sedgewick, Addison Wesley Longman, 1998.
 */
#include <stdio.h>
#include <stdlib.h>
#include "Item.h"
#include <string.h>

#define STRING_MAX 50 /* A maior palavra no dicionário inglês é um termo técnico da biologia, que tem 45 caracteres. */



Item *ITEMscan(Item *x) {
    char *t = malloc(STRING_MAX*sizeof(char));
    scanf("%s", t);
    Item *palavra = malloc(sizeof(Item));
    palavra->literal = t;
    palavra->prox = palavra->lema = NULL;
    palavra->list = NULL;
    return palavra;
}

void ITEMshow(Item x) {
    printf("%s\n", x.literal);
}

Key key(Item item) {
    return item.literal;
}

int eq(Key key1, Key key2) {
    if (*key1 == *key2) return 1;
    return 0;
}