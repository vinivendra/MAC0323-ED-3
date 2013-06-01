#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ARNE.h"

#define YES 1
#define NO 0
typedef int BOOL;

#define STRING_MAX 50

void stringCopy(char *destiny, char *source);




int main (int argc, char *argv[]) {
    
    
#pragma mark Variáveis
    char *fileName = NULL;
    FILE *file = NULL;
    
    link words = NULL;
    link lemmas = NULL;
    
    char *c = malloc(STRING_MAX*sizeof(char));
    
    fpos_t *sentenceLocation = malloc(sizeof(fpos_t));
    fpos_t *aux = malloc(sizeof(fpos_t));
    int tokenNumber = 0;
    
    BOOL e, a, v, V, t, d, l, L, s;
    
    int i = 0;
    int j = 0;
    
#pragma mark Inicializações
    initItem();             /* Inicializa Item e as árvores, para podermos começar a */
    STinit();               /* criar instâncias deles. */
    
    words = initTree();
    lemmas = initTree();
    
    e = a = v = V = t = d = l = L = s = NO; /* Seta todas as flags com NO,
                                                para evitar erros de inicialização */
    
#pragma mark Parâmetros
    if (argc < 2) {
        fprintf(stderr, "Usage: %s -f<Nome do arquivo de entrada>\n", argv[0]);
        exit(-1);
    }
    fileName = argv[1]+2;
    
    
#pragma mark Abrir arquivo
    file = fopen(fileName,"r");
    if (file == NULL) {
        fprintf(stderr, "Erro na leitura do arquivo.\n");
        exit(-1);
    }
    
    
    /* Preparação para processar o arquivo */
    fgetpos(file, sentenceLocation);
    fgetc(file);
    
    
#pragma mark Processamento
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
            
            
            words = STinsert(words, newWord);           /* Tentativa de inserir o item word na árvore T1 */
            
            conflict = *(getConflict());
            
            if (conflict == NULL) {                         /* A palavra ainda não existia */
                lemmas = STinsert(lemmas, newLemma);
                conflict = *(getConflict());
                if (!(conflict == NULL)) {                  /* Se a palavra não existia mas o lema sim, basta adicioná-la ao lema */
                    newWord->prox = conflict->prox;
                    conflict->prox = newWord;
                    
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
        fgetpos(file, sentenceLocation);    /* e sabemos que podemos sair do loop com o "feof(file)". */
        fgetc(file);                        /* Ainda assim, é importante que a sentenceLocation esteja no lugar certo,
                                             ou seja, um caracter antes da "Sentence #%d: (%d tokens)"*/
    }               /* while(!foef(file)) */
    
    /* Todo o arquivo já foi processado nesse ponto; os tokens já foram lidos, e os itens já foram inseridos
     nas árvores. Agora, lemos da entrada padrão os comandos que o usuário passar. */
    
#warning Adicionar informações sobre os comandos na usage, tanto lá em cima quando logo aqui em baixo
    
#pragma mark Comandos:
    
    printf ("Por favor, digite um comando: ");
    
    while (scanf("%s", c) == 1) {   /* Aceitamos que o usuário saia do programa também ao
                                        pressionar ctrl+D, que é o modo default de encerrar
                                        input da stdin */
        char *p;                /* p vai "percorrer" a string c */
        
        if (c == '\0') {        /* Se o usuário só deu enter, mostramos o modo de utilização do programa. */
            printf ("É assim que funciona: HUE.\n");
        }
        
        p = c + 1;              /* Ignora o '-', como em "-aV" */
        
        while (*p != '\0') {
            if (*p == 'F') {        /* -F quer dizer que o usuário deseja sair do programa */
                printf("Saindo do programa.\n");
                return 0;
            }
            else if (*p == 'e')     /* Qualquer outra letra acionará uma flag, */
                e = YES;            /* para sabermos o que fazer em cada caso. */
            else if (*p == 'a')
                a = YES;
            else if (*p == 'v')
                v = YES;
            else if (*p == 'V')
                V = YES;
            else if (*p == 't')
                t = YES;
            else if (*p == 'd')
                d = YES;
            else if (*p == 'l')
                l = YES;
            else if (*p == 'L')
                L = YES;
            else if (*p == 's')
                s = YES;
            
            p++;
        }
        
        if (e == YES || a == YES) {
            scanf("%s", c);     /* c será a palavra para procurar */
        }
        
        
#pragma mark e
        if (e == YES) {
            
            Item *word = STsearch(c, words);
            sentence *s = NULL;
            
            if (word == getNULLitem()) {
                printf("A palavra procurada não está presente no texto.\n\n");
                printf ("Por favor, digite um comando: ");
                continue;
            }
            
            
            
            for (s = word->list; s != NULL; s = s->prox) {
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
            }           /* for (s = word->list; s != NULL; s = s->prox), percorre as sentences */
        }               /* if (e == YES) */
        
#pragma mark a
        if (a == YES) {
            
            sentence *s = NULL;
            
            Item *word = STsearch(c, words);    /* Procuramos pela palavra que o usuário passou */
            Item *lemma = word->lema;           /* Lemma será o lema desta palavra; ele possui uma */
                                                /* lista de todas as palavras com o mesmo lema. */
                                                
            
            if (word == getNULLitem()) {
                printf("A palavra procurada não está presente no texto.\n\n");
                printf ("Por favor, digite um comando: ");
                continue;
            }
            
            for (word = lemma->prox; word != NULL; word = word->prox) { /* word então vai percorrer essa lista. */
                for (s = word->list; s != NULL; s = s->prox) {          /* s vai percorrer as sentences de cada palavra, */
                    fpos_t *aux = malloc(sizeof(fpos_t));               /* exatamente do mesmo jeito que foi feito no -e */
                    
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
                }           /* for (s = word->list; s != NULL; s = s->prox), percorre as sentences */
                
            }           /* for (word = lemma->prox; word != NULL; word = word->prox), percorre as palavras */
        }           /* if (a == YES) */
        
        
        
        printf ("Por favor, digite um comando: ");
        e = a = v = V = t = d = l = L = s = NO;
        
    }           /* while (scanf("%s", c) == 1) */
    
    
    
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
