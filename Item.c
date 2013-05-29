/*
 * This code is from "Algorithms in C, Third Edition,"
 * by Robert Sedgewick, Addison Wesley Longman, 1998.
 */
#include <stdio.h>
#include <stdlib.h>
#include "Item.h"
#include <string.h>

#define YES 1
#define NO 0
#define STRING_MAX 50 /* A maior palavra no dicionário inglês é um termo técnico da biologia, que tem 45 caracteres. */

static Item *NULLitem;

Item *newItem() {
    Item *item = malloc(sizeof(Item));
    item->literal = malloc(STRING_MAX*sizeof(char));
    item->prox = item->lema = NULL;
    item->list = NULL;
    return item;
}

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

Key key(Item *item) {
    return item->literal;
}

int eq(Key key1, Key key2) {
    if (strcmp(key1, key2) == 0) return 1;
    return 0;
}

int less(Key key1, Key key2) {
    if (strcmp(key1, key2) < 0) return 1;
    return 0;
}

void initItem () {
    NULLitem = malloc(sizeof(Item));
    NULLitem->literal = NULL;
    NULLitem->lema = NULLitem->prox = NULL;
    NULLitem->list = NULL;
}

Item *getNULLitem () {
    return NULLitem;
}

int isNULLitem (Item item) {
    if (item.literal == NULL) return YES;
    else return NO;
}