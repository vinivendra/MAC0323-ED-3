#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include "ARNE.h"

#define YES 1
#define NO 0
#define STRING_MAX 50

int main (int argc, char *argv[]) {
    
    char *fileName = NULL;
    FILE *file = NULL;
    
    link words = NULL;
    link lemmas = NULL;
    
    char *c = malloc(STRING_MAX*sizeof(char));
    char *textPattern = "[Text=";
    char *lemmaPattern = "Lemma=";
    
    int i = 0;
    
    /* Inicializações */
    
    initItem();
    
    STinit();
    words = initTree();
    lemmas = initTree();
    
    /* Leitura de parâmetros */
    if (argc < 2) {
        fprintf(stderr, "Por favor, passe o nome do arquivo de entrada como parâmetro.\n");
        exit(-1);
    }
    fileName = argv[1]+2;
    
    
    /* Abrir o arquivo */
    file = fopen(fileName,"r");
    if (file == NULL) {
        fprintf(stderr, "Erro na leitura do arquivo.\n");
        exit(-1);
    }
    
    
    /* Processamento do arquivo */
    for (fscanf(file, "%s", c); !feof(file); fscanf(file, "%s", c)) {
        if (strncmp(c, textPattern, 6)==0) {    /* Se entramos num bloco [Text=... Lemma=...] */
            
            Item *newWord = malloc(sizeof(Item));
            newWord->literal = malloc(STRING_MAX*sizeof(char));
            newWord->lema = newWord->prox = NULL;
            newWord->list = NULL;
            Item *newLemma = malloc(sizeof(Item));
            newLemma->literal = malloc(STRING_MAX*sizeof(char));
            newLemma->lema = newLemma->prox = NULL;
            newLemma->list = NULL;
            
            Item *conflict = NULL;
            
            strncpy(newWord->literal, (c+6), 40);             /* Coloca a palavra encontrada no wordAux */
            
            while(1) {                          /* Percorre até o lema */
                fscanf(file, "%s", c);
                if (strncmp(c, lemmaPattern, 6)==0)
                    break;
            }
            
            for (i = 0;; i++) {                 /* O lema vai terminar com ']'; queremos que termine com '\0' */
                if (*(c+6+i) == ']') {
                    c[6+i] = '\0';
                    break;
                }
            }
            
            strcpy(newLemma->literal, (c+6));            /* Coloca o lema em lemmaAux */
            
            words = STinsert(words, *newWord, conflict);    /* Tentativa de inserir word na árvore */
            
            printf("BLA\n");
            
            printf("BLA2\n");
            
            if (conflict == NULL) {
                
            }
            
            
        }
    }
    
    printf("HUE: %s\n", words->item->literal);

    STprint(words);
    
    /* Finalizando */
    fclose(file);
    
    return 0;
}
