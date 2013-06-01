#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ARNE.h"




#pragma mark Defines e Variáveis globais

#define STRING_MAX 50       /* Tamanho máximo de uma string. */
/* A maior palavra no dicionário inglês é um termo técnico da biologia, que tem 45 caracteres. */
#define YES 1               /* Versão simples de um bool */
#define NO 0
typedef int BOOL;

static int numDifWords;     /* Variáveis globais por simplicidade, usadas para obter estatísticas do programa */
static int numDifTokens;
static int numDifLemmas;




#pragma mark Protótipos

void stringCopy(char *destiny, char *source);
void printItem(Item word);
void printWord(Item word);
void printWordsFromLemma(Item lemma);
void statisticCountWords(Item word);
void statisticCountLemmas(Item lemma);



#pragma mark Main

int main (int argc, char *argv[]) {
    
    #pragma mark Variáveis
    
    char *fileName = NULL;  /* Manipulação d arquivo de entrada */
    FILE *file = NULL;
    
    link words = NULL;      /* Árvore para guardar as palavras (words = T1) */
    link lemmas = NULL;     /* e os lemas (lemmas = T2) */
        
    fpos_t *sentenceLocation = malloc(sizeof(fpos_t));  /* Variáveis usadas no processamento do arquivo */
    fpos_t *aux = malloc(sizeof(fpos_t));
    int tokenNumber = 0;
    
    BOOL e, a, v, V, t, d, l, L, s;     /* Variáveis para manipular os comandos passados pelo usuário */
    
    char *c = malloc(STRING_MAX*sizeof(char));  /* Auxiliares */
    int i = 0;          
    int j = 0;
    char *k = 0;
    
    int numSentences = 0;   /* Variáveis usadas para obter os dados estatísticos */
    int numTokens = 0;
    int numWords = 0;
    



#pragma mark Inicializações
    
    numDifTokens = 0;       /* Inicialização das variáveis globais */
    numDifWords = 0;
    numDifLemmas = 0;
    
    initItem();             /* Inicializa as informações para Item e para as árvores, */
    STinit();               /* para podermos começar a criar instâncias deles. */
    
    words = initTree();     /* Inicialização das árvores em si */
    lemmas = initTree();
    
    e = a = v = V = t = d = l = L = s = NO; /* Seta todas as flags com NO,
                                             para evitar erros de inicialização */
    



#pragma mark Parâmetros
    
    if (argc < 2) {         /* Se não houver parâmetros, passamos o modo de utilização */
        fprintf(stderr, "Usage: %s -f<Nome do arquivo de entrada>\n", argv[0]);
        exit(-1);
    }
    fileName = argv[1]+2;   /* Leitura do nome do arquivo, único parâmetro passado */
    
    


#pragma mark Abertura do arquivo
    
    file = fopen(fileName,"r");     /* Abrimos o arquivo, só precisamos da leitura */
    if (file == NULL) {             /* Caso não se possa abri-lo (ex.: o arquivo não existe) */
        fprintf(stderr, "Erro na leitura do arquivo.\n");
        exit(-1);
    }
    
    /* Preparação para processar o arquivo */
    fgetpos(file, sentenceLocation);
    fgetc(file);
    
    


#pragma mark Processamento
    
    while (!feof(file)) {       /* A parte mais demorada e mais importante do programa */
        
        /* Le o cabeçalho de cada frase. O 'S' de sentence já foi */
        /* "consumido" anteriormente por um fgetc proposital */
        
        fscanf(file, "entence #%*d (%d tokens):\n", &tokenNumber);
        numSentences ++;            /* Conta mais uma frase lida */
        numTokens += tokenNumber;   /* e mais alguns tokens lidos, para a estatística */
        
        
        
        /* Vamos ler cada caracter, até acharmos um '[', que possivelmente será o começo
            de uma anotação. Se for, queremos voltar o suficiente, para que o loop seguinte
            funcione sem problemas. Se não, queremos voltar até logo depois do '[', para
            podermos continuar a leitura normalmente. */
        
        while (1) {
            fgetpos(file, aux);                 /* Guarda a posição no arquivo, para poder 
                                                 voltar depois caso necessário */
            *c = fgetc(file);                   /* Lê mais um caracter */
            if (*c == '[') {                        /* Se for um '[', talvez seja o começ da anotação */
                fscanf(file, "%s", c);
                if (strncmp(c, "Text=", 5)==0) {    /* Consideramos que a anotação começou se acharms um "[Text=" */
                    fsetpos(file, aux);             /* Volta o suficiente */
                    break;                          /* Sai desse loop para começar a leitura das anotações */
                }
                else {                          /* Se não for um '[', é só voltar */
                    fsetpos(file, aux);         /* para podermos continuar normalmente */
                    fgetc(file);
                }
            }
        }
        
        
        
        /* Analisa cada anotação, e insere as palavras e os lemas na árvore adequada. */
        /* Sabemos, a partir da leitura do cabeçalho, exatamente quantos tokens */
        /* existem nessa anotação. */
        
#warning PAREI AQUI
        
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
            
            k = newWord->literal;
            if ((*k >= 'a' && *k <= 'z') || (*k >= 'A' && *k <= 'Z'))
                numWords ++;
            
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
        
#pragma mark t, d, l, L
        if (t == YES) {
            STsort(words, printItem);
            printf("\n");
        }
        
        if (d == YES) {
            STsort(words, printWord);
            printf("\n");
        }
        if (l == YES) {
            STsort(lemmas, printWord);
            printf("\n");
        }
        if (L == YES) {
            STsort(lemmas, printWordsFromLemma);
            printf("\n");
        }
#pragma mark s
        if (s == YES) {
            STsort(words, statisticCountWords);
            STsort(lemmas, statisticCountLemmas);
            printf("Número de sentenças no texto = %d;\nNúmero total de tokens no texto: %d;\nNúmero total de palavras no texto: %d;\nNúmero total de tokens distintos: %d;\nNúmero total de palavras distintas: %d;\nNúmero total de lemas distintos: %d.\n\n", numSentences, numTokens, numWords, numDifTokens, numDifWords, numDifLemmas);
        }
        
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

void printItem(Item word) {
    printf("%s\n", key(&word));
}

void printWord(Item word) {
    char *c = key(&word);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        printf("%s\n", key(&word));
}

void printWordsFromLemma(Item lemma) {
    Item *word;
    
    char *c = key(&lemma);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z')) {
        printf("%s: ", key(&lemma));
        
        for (word = lemma.prox; word != NULL; word = word->prox) {
            
            printf("%s", key(word));
            if (word->prox != NULL)
                printf(", ");
        }
        printf("\n");
    }
}

void statisticCountWords(Item word) {
    char *c = key(&word);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        numDifWords ++;
    numDifTokens ++;
}

void statisticCountLemmas(Item lemma) {
    char *c = key(&lemma);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        numDifLemmas ++;
}




