#ifndef ED3_Item_h
#define ED3_Item_h

#include <string.h>
#include <stdio.h>

typedef int Item;
typedef int Key;

#define key(A) (A)
#define eq(A, B) ((A) == (B))
#define less(A, B) ((A)<(B))
#define NULLitem (-1)

int ITEMscan(Item *);
void ITEMshow(Item);
int ITEMrand();

#endif
