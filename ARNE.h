#ifndef ED3_ARNE_h
#define ED3_ARNE_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Item.h"

typedef struct STnode* link;
struct STnode {
    Item *item;
    link l, r;
    int N;
    int red;
};


link initTree();
void STinit();
Item **getConflict();
int STcount(link head);
Item *STsearch(Key v, link head);
link STinsert(link head, Item *item);
Item STselect(link head, int r);
void STsort(link head, void(*visit)(Item));
void STprint_range(link head, Key lo, Key hi);
void STdeleteMin(link head);
void STdeleteMax(link head);
void STdelete(link head, Key v);
void STdraw(link head);
void STprint(link head);

#endif
