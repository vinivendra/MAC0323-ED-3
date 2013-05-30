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
    
    fpos_t *sentenceLocation = malloc(sizeof(fpos_t));
    fpos_t *aux = malloc(sizeof(fpos_t));
    int tokenNumber = 0;
    
    int i = 0;
    int j = 0;
    
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
    
    fgetpos(file, sentenceLocation);
    fgetc(file);

    /* Processamento do arquivo */
    while (!feof(file)) {
        
        /* Le as sentences. O 'S' já foi "consumido" anteriormente por um fgetc(file) proposital. */
        fscanf(file, "entence #%*d (%d tokens):\n", &tokenNumber);
        
        /* Pula a frase. Lê caracteres até achar um '['. Se for o começo de um token,
            volta até o caracter imediatamente anterior e sai do loop para começar a
            ler os tokens. Caso contrário, volta o suficiente para continuar a leitura
            caracter por caracter logo após o '['. */
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
                }
            }
        }
        
        
        /* Analisa as tokens e as insere nas árvores adequadas */
        for (i = 0; i < tokenNumber && !(feof(file)); i++) {
            Item *newWord = malloc(sizeof(Item));       /* Cria os novos itens a serem inseridos */
            Item *newLemma = malloc(sizeof(Item));
            Item *conflict = NULL;
            
                                                        /* Inicializa os novos itens */
            newWord->literal = malloc(STRING_MAX*sizeof(char));
            newWord->lema = newLemma;
            newWord->prox = NULL;
            newWord->list = malloc(sizeof(sentence));
            newWord->list->position = *sentenceLocation;
            newWord->list->prox = NULL;
            
            newLemma->literal = malloc(STRING_MAX*sizeof(char));
            newLemma->lema = NULL;
            newLemma->prox = newWord;
            newLemma->list = NULL;
            
            
            fscanf(file, "%s", c);                      /* Lê "[Text=word" */

            strcpy(newWord->literal, (c+6));            /* Coloca a palavra encontrada no novo item */
            
            while(1) {                                  /* Percorre até o achar o lema */
                fscanf(file, "%s", c);
                if (strncmp(c, "Lemma=", 6)==0)
                    break;
            }
                        
            for (j = 0;; j++) {                         /* O lema vai terminar com ']'; queremos que termine com '\0' */
                if (*(c+6+j) == ']') {
                    c[6+j] = '\0';
                    break;
                }
            }
            
            strcpy(newLemma->literal, (c+6));           /* Coloca o lema no novo item */
            
            
            words = STinsert(words, newWord, conflict);     /* Tentativa de inserir o item word na árvore T1 
                                                                Conflict é um ponteiro que vai apontar para o item
                                                                que já estava na árvore, caso tentemos adicionar
                                                                uma palavra repetida.*/
            
            if (conflict == NULL) {                         /* A palavra ainda não existia */
                lemmas = STinsert(lemmas, newLemma, conflict);
                if (!(conflict == NULL)) {                  /* Se a palavra não existia mas o lema sim, basta adicioná-la ao lema */
                    Item *aux = conflict->prox;
                    conflict->prox = newWord;
                    newWord->prox = aux;
                    
                    newWord->lema = conflict;
                }                                           /* Se ambos ainda não existiam, eles foram adicionados, sem problemas */
            }
            else {                                          /* A palavra já existia, logo, o lema também */
                sentence *aux = conflict->list;
                conflict->list = newWord->list;
                newWord->list->prox = aux;
            }
        }           /* for (i = 0; i < tokenNumber; i++) */
        
        
        
        fgetc(file);                        /* "Consumimos" o espaço, o enter e o 'S' logo depois dos tokens. */
        fgetc(file);                        /* Assim, se estivermos no final, o indicador de EOF será ligado */
        fgetpos(file, sentenceLocation);    /* E sabemos que podemos sair do loop. */
        fgetc(file);                        /* Ainda assim, é importante que a sentenceLocation esteja no lugar certo, 
                                                ou seja, um caracter antes da "Sentence #%d: (%d tokens)"*/
    }               /* while(!foef(file)) */
    
    /* Todo o arquivo já foi processado nesse ponto; os tokens já foram lidos, e os itens já foram inseridos
        nas árvores. Agora, lemos da entrada padrão os comandos que o usuário passar. */
    
    
    
    STprint(words);
    STprint(lemmas);
    
    /* Leitura de comandos a partir da stdin */
    while (1) {
        scanf("%s", c);
        
        if (c[1] == 'F') {
            printf("Saindo do programa.\n");
            return 0;
        }
        
        
    }
    
    
    
    

    
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
