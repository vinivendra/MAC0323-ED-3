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
void printSentence (sentence *s, FILE *file, int v, int V);

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

void printAllSentences (Item *word, FILE *file, int v, int V) {
    sentence *s;
    
    for (s = word->list; s != NULL; s = s->prox) {
        printSentence(s, file, v, V);
    }
}

void printSentence (sentence *s, FILE *file, int v, int V) {
    char * c;
    fpos_t *aux = malloc(sizeof(fpos_t));
    
    fsetpos(file, &(s->position));
    
    if (v == NO && V == NO)
        fscanf (file, "Sentence #%*d (%*d tokens):");
    
    printf ("\n");
    
    while (1) {
        fgetpos(file, aux);
        *c = fgetc(file);
        if (*c == '[') {
            fscanf(file, "%s", c);
            if (strncmp(c, "Text=", 5)==0) {
                fsetpos(file, aux);
                break;
            }
            else {
                fsetpos(file, aux);
                fgetc(file);
                printf("%c", *c);
            }
        }
        else printf("%c", *c);
    }       /* while (1), imprime a sentence */
    
    if (V == YES) {
        while (1) {
            *c = fgetc(file);
            if (*c == '\n')
                break;
            printf("%c", *c);
        }
    }
    
    printf("\n");
}