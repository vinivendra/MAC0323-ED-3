#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ARNE.h"

#define YES 1
#define NO 0
#define STRING_MAX 50

void stringCopy(char *destiny, char *source);




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
    
    fpos_t *sentenceLocation = malloc(sizeof(fpos_t));
    fpos_t *aux = malloc(sizeof(fpos_t));
    int tokenNumber = 0;
    
    
    
    
    
    while (!feof(file)) {
        
        /* Le as sentences */
        fscanf(file, "Sentence #%*d (%d tokens):\n", &tokenNumber);
        fgetpos(file, sentenceLocation);
        
        /* Pula a frase */
        while (1) {
            *c = fgetc(file);
            if (*c == '[') {
                fgetpos(file, aux);
                fscanf(file, "%s", c);
                if (strncmp(c, "Text=", 5)==0)
                    break;
                else
                    fsetpos(file, aux);
            }
        }
        
        
        for (i = 0; i < tokenNumber; i++) {
            
            fscanf(file, "%s", c);
            
            Item *newWord = malloc(sizeof(Item));
            Item *newLemma = malloc(sizeof(Item));
            
            newWord->literal = malloc(STRING_MAX*sizeof(char));
            newWord->lema = newLemma;
            newWord->prox = NULL;
            newWord->list = NULL;
            
            newLemma->literal = malloc(STRING_MAX*sizeof(char));
            newLemma->lema = NULL;
            newLemma->prox = newWord;
            newLemma->list = NULL;
            
            Item *conflict = NULL;
            
            strcpy(newWord->literal, (c+6));             /* Coloca a palavra encontrada no wordAux */
            
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
            
            words = STinsert(words, newWord, conflict);    /* Tentativa de inserir word na árvore */
            
            if (conflict == NULL) {             /* A palavra ainda não existia */
                lemmas = STinsert(lemmas, newLemma, conflict);
                if (!(conflict == NULL)) {      /* Se a palavra não existia mas o lema sim, basta adiciona-la ao lema */
                    Item *aux = conflict->prox;
                    conflict->prox = newWord;
                    newWord->prox = aux;
                    
                    newWord->lema = conflict;
                }                               /* Se ambos ainda não existiam, eles foram adicionados, sem problemas */
            }
            else {                              /* A palavra já existia, logo, o lema também */
                /* Adiciona a frase nova à word antiga */
            }
        }           /* for (i = 0; i < tokenNumber; i++) */
        
        fgetc(file);
        fgetc(file);
        
        
    }
    
    
    
    
    
    STprint(words);
    STprint(lemmas);
    
    /* Finalizando */
    fclose(file);
    
    return 0;
}

void stringCopy(char *destiny, char *source) {
    int i = 0;
    for (i = 0; source[i]!='\0'; i++) {
        (destiny[i]) = (source[i]);
    }
}
