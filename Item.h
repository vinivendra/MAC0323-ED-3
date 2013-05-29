#ifndef ED3_Item_h
#define ED3_Item_h

#include <string.h>
#include <stdio.h>

typedef struct sentence {
    int number;
    struct sentence *prox;
} sentence;

struct palavra {
    char *literal;
    struct palavra *lema;
    struct palavra *prox;
    sentence *list;
};

typedef struct palavra Item;
typedef char* Key;

#define NULLitem NULL

Item *ITEMscan(Item *item);
void ITEMshow(Item item);
Key key(Item item);
int eq(Key key1, Key key2);
int less(Key key1, Key key2);

#endif
