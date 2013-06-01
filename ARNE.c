/*
 * Much of this code is adapted from "Algorithms in C, Third Edition,"
 * by Robert Sedgewick, Addison Wesley Longman, 1998, and from
 * Sedgewick's own Java code for left-leaning red-black trees.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Item.h"
#include "ARNE.h"

/*
 Constantes
 */
const char no_externo[] = "\\Tn";
const char no_interno[] = "\\Tc*{3pt}";
const char tree_sep[] = "3mm";
const char level_sep[] = "5mm";

/*
 * Back to Sedgewick stuff
 */



static link z;
static Item *conflict;



Item **getConflict() {
    return &conflict;
}

link NEW(Item *item, link l, link r, int N, int red) {
    link x = malloc(sizeof *x);
    x->item = item;
    x->l = l;
    x->r = r;
    x->N = N;
    x->red = red;
    return x;
}

link initTree() {
    link head = z;
    return head;
}

void STinit() {
    z = NEW(getNULLitem(), 0, 0, 0, 0);
    conflict = NULL;
}

int STcount(link head) {
    return head->N;
}

#define hl  (h->l)
#define hr  (h->r)
#define hrl  (h->r->l)
#define hll  (h->l->l)

Item *searchR(link h, Key v) {
    Key t = key(h->item);
    if (h == z) return getNULLitem();
    if (eq(v, t)) return (h->item);
    if (less(v, t)) return searchR(hl, v);
    else return searchR(hr, v);
}

Item *STsearch(Key v, link head) {
    return searchR(head, v);
}

link fixNr(link h) {
    h->N = hl->N + hr->N +1;
    return h;
}

link rotL(link h) {
    link x = hr; hr = x->l; x->l = fixNr(h);
    x->red = x->l->red; x->l->red = 1;
    return fixNr(x);
}

link rotR(link h) {
    link x = hl; hl = x->r; x->r = fixNr(h);
    x->red = x->r->red; x->r->red = 1;
    return fixNr(x);
}

void colorFlip(link h) {
    h->red = 1 - h->red;
    hl->red = 1 - hl->red;
    hr->red = 1 - hr->red;
}

link mvRedL(link h) {
    colorFlip(h);
    if (hrl->red) { hr = rotR(hr); h = rotL(h); }
    return h;
}

link mvRedR(link h) {
    colorFlip(h);
    if (hll->red) { h = rotR(h); }
    return h;
}

link balance(link h) {
    if (hr->red) h = rotL(h);
    if (hl->red && hll->red) h = rotR(h);
    if (hl->red && hr->red) colorFlip(h);
    return fixNr(h);
}

/*
 * Insertions
 */

link LLRBinsert(link h, Item *item) {
    Key v = key(item);
    /* Insert a new node at the bottom*/
    
    conflict = NULL;
    
    if (h == z) {
        return NEW(item, z, z, 1, 1);
    }
    
    if (less(v, key(h->item)))
        hl = LLRBinsert(hl, item);
    
    else if (eq(v, key(h->item))) {   /* If the object we are trying to insert is already there,
                                        we opt to return a pointer to the existing item, so that
                                        the user may choose what to do (i.e. create a list of items) */
        conflict = h->item;
    }
    
    else
        hr = LLRBinsert(hr, item);
    
    
    /* Enforce left-leaning condition */
    if (hr->red && !hl->red) h = rotL(h);
    if (hl->red && hll->red) h = rotR(h);
    if (hl->red && hr->red) colorFlip(h);
    
    return fixNr(h);
}

link STinsert(link head, Item *item) {
    head = LLRBinsert(head, item);
    head->red = 0;
    return head;
}

/*
 * Select and sort
 */

Item selectR(link h, int r) {
    int t = hl->N;
    if (t > r) return selectR(hl, r);
    if (t < r) return selectR(hr, r-t-1);
    return *(h->item);
}

Item STselect(link head, int r) {
    return selectR(head, r);
}

void sortR(link h, void(*visit)(Item)) {
    if (h!=z) {
        sortR(hl, visit);
        visit(*(h->item));
        sortR(hr, visit);
    }
}

void STsort(link head, void(*visit)(Item)) {
    sortR(head, visit);
}

/*
 * Print a range
 */

void print_rangeR(link h, Key lo, Key hi) {
    if (h == z) return;
    if (less(lo, key(h->item)))
        print_rangeR(hl, lo, hi);
    if ((less(lo, key(h->item)) || eq(lo, key(h->item)))
        && (less(key(h->item), hi) || eq(key(h->item), hi)))
        ITEMshow(*(h->item));
    if (less(key(h->item), hi))
        print_rangeR(hr, lo, hi);
}

void STprint_range(link head, Key lo, Key hi) {
    print_rangeR(head, lo, hi);
}

/*
 * Needed for deletion
 */

Item *getMin(link h) {
    if (hl == z) return h->item;
    else return getMin(hl);
}

/*
 * Deletions
 */

link deleteMin(link h) {
    if (hl == z) { free(h); return z; }
    if (!hl->red && !hll->red) h = mvRedL(h);
    hl = deleteMin(hl);
    return balance(h);
}

void STdeleteMin(link head) {
    if (STcount(head) == 0)
    { printf("Underflow"); exit(1); }
    
    if (!head->l->red && !head->r->red) head->red = 1;
    
    head = deleteMin(head);
    if (STcount(head) > 0) head->red = 0;
}

link deleteMax(link h) {
    if (hl->red) h = rotR(h);
    if (hr == z) { free(h); return z; }
    if (!hr->red && !hrl->red) h = mvRedR(h);
    hr = deleteMax(hr);
    return balance(h);
}

void STdeleteMax(link head) {
    if (STcount(head) == 0)
        printf("Underflow"); exit(1);
    
    if (!head->l->red && !head->r->red) head->red = 1;
    
    head = deleteMax(head);
    if (STcount(head) > 0) head->red = 0;
}

link deleteR(link h, Key v) {
    Key t = key(h->item);
    
    if (less(v,t)) {
        if (!hl->red && !hll->red) h = mvRedL(h);
        hl = deleteR(hl, v);
    }
    else {
        if (hl->red) h = rotR(h);
        if (eq(v,key(h->item)) && hr == z)
            free(h); return z;
        if (!hr->red && !hrl->red) h = mvRedR(h);
        if (eq(v,key(h->item))) {
            h->item = getMin(hr);
            hr = deleteMin(hr);
        }
        else hr = deleteR(hr, v);
    }
    return balance(h);
}

void STdelete(link head, Key v) {
    if (!head->l->red && !head->r->red) head->red = 1;
    head = deleteR(head, v);
    if (STcount(head) > 0) head->red = 0;
}

/*
 * Integrity check
 
 int size(link h) { return h->N; }
 
 int height(link h)
 { int tl, tr;
 if (h==z) return 0;
 tl = height(hl);
 tr = height(hr);
 return 1 + tl > tr ? tl : tr;
 }
 
 int STheight() { return height(head); }
 
 int check()
 {
 if (!isBST()) printf("Fails the property of BST.\n");
 if (!sizeConsistent()) printf("Fails the size consistency.\n");
 if (!rankConsistent()) printf("Fails the rank consistency.\n");
 if (!is23()) printf("Not a 2-3 tree.\n");
 if (!balanced()) printf("Not balanced.\n");
 return
 isBST() && sizeConsistent() && rankConsistent()
 && is23() && balanced();
 }
 
 */

/* Drawing? */

/* faz_desenho(): deveria chamar faca_desenho();
 acho que nao deveria devolver valor nenhum...
 */

int faz_desenho(link h, int nivel) {
    int d1, d2;
    
    if (h->N == 0) {
        printf("%s\n", no_externo);
        return 1;
    }
    
    if (nivel == 0)
        printf("\\pstree[levelsep=%s, treesep=%s]{%s}{\n", level_sep,
               tree_sep, no_interno);
    else
        printf("\\pstree{%s}{\n", no_interno);
    d1 = faz_desenho(h->l, nivel+1);
    d2 = faz_desenho(h->r, nivel+1);
    printf("}\n");
    return d1 + d2 + 1;
}

void STdraw(link head) {
    printf("\\rput{90}{");
    faz_desenho(head, 0);
    printf("}\n");
}

void printR(link h, int ind) {
    int i;
    if (h != z) {
        for (i=0; i<ind; i++) putchar(' ');
        printf("%s%c\n", key(h->item), h->red?'*':' ');
        printR(hl, ind+2);
        printR(hr, ind+2);
    }
}

void STprint(link head) {
    printf("\n**** %d keys ****\n", head->N);
    printR(head, 0);
    printf("**** ****** ****\n");
}