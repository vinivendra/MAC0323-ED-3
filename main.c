#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ARNE.h"


#warning checar se o manifesto quebra (no e e no a), imprimir as frases na ordem certa (e não de tras para frente)

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
void printUsage ();



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
        
        for (i = 0; i < tokenNumber && !(feof(file)); i++) {    /* For só para rodar 'tokenNumber' vezes */
            Item *newWord = malloc(sizeof(Item));       /* Cria os novos itens a serem inseridos */
            Item *newLemma = malloc(sizeof(Item));
            Item *conflict = NULL;                      /* Conflict é um ponteiro que será setado
                                                            caso haja conflito na inserção */
            
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
            
            
            fscanf(file, "%s", c);                      /* Lê "[Text=<word>" */
            
            strcpy(newWord->literal, (c+6));            /* Coloca a palavra encontrada no novo item */
            
            while(1) {                                  /* Percorre o texto até o achar o lema, */
                fscanf(file, "%s", c);                  /* palavra por palavra. */
                if (strncmp(c, "Lemma=", 6)==0)
                    break;
            }
            
            for (j = 0;; j++) {                         /* O lema vai terminar com ']'; queremos que termine com '\0' */
                if (*(c+6+j) == ']') {
                    c[6+j] = '\0';
                    break;
                }
            }
            
            strcpy(newLemma->literal, (c+6));           /* Coloca o lema encontrado no novo item */
            
            k = newWord->literal;                       /* Se newWord for uma palavra (não uma pontuação), conta para a estatística */
            if ((*k >= 'a' && *k <= 'z') || (*k >= 'A' && *k <= 'Z'))
                numWords ++;
            
            words = STinsert(words, newWord);           /* Tentativa de inserir o item word na árvore T1 */
            
            conflict = *(getConflict());                /* Se a tentativa falhou é porque já existia um item igual;
                                                            getConflict() vai retornar um ponteiro para esse item */
            
            if (conflict == NULL) {                         /* A palavra ainda não existia, logo, */
                lemmas = STinsert(lemmas, newLemma);        /* o lema também não existia. Tentamos inseri-lo. */
                conflict = *(getConflict());
                
                if (!(conflict == NULL)) {                  /* Se a palavra não existia mas o lema sim, basta adicioná-la ao lema */
                    newWord->prox = conflict->prox;
                    conflict->prox = newWord;
                    
                    newWord->lema = conflict;
                }                                           /* Se ambos ainda não existiam, eles foram adicionados, sem problemas */
            }
            else {                                          /* A palavra já existia, logo, o lema também */
                if (conflict->list->position != newWord->list->position) {      /* Se ainda não contamos essa frase para essa palavra */
                    newWord->list->prox = conflict->list;         /* Só precisamos adicionar a frase em que essa palavra foi encontrada. */
                    conflict->list = newWord->list;
                }
            }
        }   /* for (i = 0; i < tokenNumber; i++) */
        
        
        
        fgetc(file);                        /* "Consumimos" o espaço, o enter e o 'S' logo depois das anotações. */
        fgetc(file);                        /* Assim, se estivermos no final, o indicador de EOF será ligado */
        fgetpos(file, sentenceLocation);    /* e sabemos que podemos sair do loop com o "feof(file)". */
        fgetc(file);                        /* Ainda assim, é importante que a sentenceLocation esteja no lugar certo,
                                             ou seja, um caracter antes da "Sentence #%d: (%d tokens)"*/
    }       /* while(!foef(file)) */
    
    
    
    
    /* Todo o arquivo já foi processado nesse ponto; os tokens já foram lidos, e os itens já foram inseridos
     nas árvores. Agora, lemos da entrada padrão os comandos que o usuário passar. */




#pragma mark Comandos:
    
    printf ("Por favor, digite um comando: ");
    
    while (scanf("%s", c) == 1) {   /* Aceitamos que o usuário saia do programa também ao
                                     pressionar ctrl+D, que é o modo default de encerrar
                                     input da stdin */
        char *p;                /* p vai "percorrer" a string c */
        
        if (*c != '-' || *(c+1) == '\0') {        /* Se o comando não começa com '-', imprimimos o modo de utilização. */
            printUsage();
            printf ("Por favor, digite um comando: ");
            continue;
        }
        
        p = c + 1;              /* Ignora o '-', como em "-aV" */
        
        while (*p != '\0') {    /* Lê cada letra do comando, para que possamos ter mais de uma letra
                                    no mesmo comando (como em -ev) */
            
            if (*p == 'F') {        /* -F quer dizer que o usuário deseja sair do programa */
                printf("Saindo do programa.\n");
                return 0;
            }
            
            else if (*p == 'e') {   /* Qualquer outra letra acionará uma flag, */
                e = YES;            /* para sabermos o que fazer em cada caso. */
                p++;
                if (*p == 'v') {    /* Se o comando for 'e' ou 'a', checamos a presença de 'v' ou 'V' */
                    v = YES;
                }
                else if (*p == 'V') {
                    V = YES;
                }
                else {              /* Qualquer outra coisa, imprimimos o modo de uso */
                    printUsage();
                    printf ("Por favor, digite um comando: ");
                    e = a = v = V = t = d = l = L = s = NO;
                    break;
                }
                break;
            }
            else if (*p == 'a') {   /* Lemos o 'a' do mesmo modo que o 'e' */
                a = YES;
                p++;
                if (*p == 'v') {
                    v = YES;
                }
                else if (*p == 'V') {
                    V = YES;
                }
                else {
                    printUsage();
                    printf ("Por favor, digite um comando: ");
                    e = a = v = V = t = d = l = L = s = NO;
                    break;
                }
                break;
            }
            else if (*p == 't') {   /* As outras opções são mais simples */
                t = YES;
                break;
            }
            else if (*p == 'd') {
                d = YES;
                break;
            }
            else if (*p == 'l') {
                l = YES;
                break;
            }
            else if (*p == 'L') {
                L = YES;
                break;
            }
            else if (*p == 's') {
                s = YES;
                break;
            }
            else {      /* Se o comando passado for desconhecido, imprimimos o modo de uso */
                printUsage();
                printf ("Por favor, digite um comando: ");
                e = a = v = V = t = d = l = L = s = NO;
                break;
            }
            
        }   /* while (*p != '\0') */
        
        if (e == YES || a == YES) {     /* Se o comando foi 'a' ou 'e', precisamos ler a palavra passada também. */
            scanf("%s", c);             /* c será a palavra para procurar */
        }
        
        

#pragma mark e
        if (e == YES) {             /* -e[V, v] <word> imprime todas as frases que contêm <word> */
            
            Item *word = STsearch(c, words);    /* Achamos o item correspondente à palavra a ser procurada */
            sentence *s = NULL;                 /* s percorre a lista de frases */
            
            if (word == getNULLitem()) {        /* Caso o item não seja encontrado */
                printf("A palavra procurada não está presente no texto.\n\n");
                printf ("Por favor, digite um comando: ");
                continue;
            }
            
            
            /* Vamos percorrer a lista de frases */
            
            for (s = word->list; s != NULL; s = s->prox) {  /* s percorre a lista */
                fpos_t *aux = malloc(sizeof(fpos_t));
                
                fsetpos(file, &(s->position));          /* Vamos para o começo da frase no arquivo de entrada */
                
                if (v == NO && V == NO)                 /* Se não tem 'v' nem 'V', podemos pular o cabeçalho. */
                    fscanf (file, "Sentence #%*d (%*d tokens):");
                
                printf ("\n");
                
                while (1) {
                    fgetpos(file, aux);     /* Mesma ideia do processamento do arquivo. Cada caracter que */
                    *c = fgetc(file);       /* sabemos não ser uma anotação será impresso, até acabar a frase. */
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
                }   /* while (1) */
                
                if (V == YES) {             /* Se o comando incluiu 'V', imprimimos as anotações também */
                    while (1) {
                        *c = fgetc(file);
                        if (*c == '\n')
                            break;
                        printf("%c", *c);
                    }
                }
                
                printf("\n");
            }   /* for (s = word->list; s != NULL; s = s->prox), percorre as sentences */
        }   /* if (e == YES) */
        
        
        
#pragma mark a
        if (a == YES) {     /* A ideia do 'a' é praticamente igual à do 'e',
                                mas será repetida para várias palavras.*/
            
            sentence *s = NULL;
            
            Item *word = STsearch(c, words);    /* Procuramos pela palavra que o usuário passou */
            Item *lemma = word->lema;           /* Pegamos o seu lema */
            
            
            if (word == getNULLitem()) {        /* Se a palavra não existir */
                printf("A palavra procurada não está presente no texto.\n\n");
                printf ("Por favor, digite um comando: ");
                continue;
            }
            
            for (word = lemma->prox; word != NULL; word = word->prox) { /* word então vai percorrer a lista de palavras em 'lemma'. */
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
                }   /* for (s = word->list; s != NULL; s = s->prox), percorre as sentences */
            }   /* for (word = lemma->prox; word != NULL; word = word->prox), percorre as palavras */
        }   /* if (a == YES) */
        
        
#pragma mark t, d, l, L
        if (t == YES) {         /* Essas quatro opções são apenas funções diferentes passadas para o STsort,
                                    que visita cada Item e chama a nossa função para cada um. */
            STsort(words, printItem);   /* Imprime o Item, seja lá qual for */
            printf("\n");
        }
        
        if (d == YES) {
            STsort(words, printWord);   /* Imprime o Item, mas apenas se for uma palavra */
            printf("\n");
        }
        if (l == YES) {
            STsort(lemmas, printWord);
            printf("\n");
        }
        if (L == YES) {
            STsort(lemmas, printWordsFromLemma);    /* Imprime cada Item de cada lema, mas apenas se forem palavras */
            printf("\n");
        }
        
        
        
#pragma mark s
        if (s == YES) {         /* Parte da implementação do 's' está "espalhada" pelo código; o resto está aqui */
            if (numDifTokens == 0 && numDifWords == 0)      /* Se nunca rodamos o comando 's' */
                STsort(words, statisticCountWords);         /* Contamos as palavras e os tokens da árvore */
            if (numDifLemmas == 0)                      /* Se já rodamos o 's' uma vez, não é necessário contar de novo. */
                STsort(lemmas, statisticCountLemmas);   /* Contamos os lemas. */
            
            printf("Número de sentenças no texto = %d;\nNúmero total de tokens no texto: %d;\nNúmero total de palavras no texto: %d;\nNúmero total de tokens distintos: %d;\nNúmero total de palavras distintas: %d;\nNúmero total de lemas distintos: %d.\n\n", numSentences, numTokens, numWords, numDifTokens, numDifWords, numDifLemmas);
            /* Os outros dados foram contados ao longo da execução do programa. */
        }
        
        /* Fim da implementação dos comandos */
        
        /* Ao fim de cada comando, é necessário imprimir novamente a mensagem e resetar as flags*/
        printf ("Por favor, digite um comando: ");
        e = a = v = V = t = d = l = L = s = NO;
        
    }   /* while (scanf("%s", c) == 1) */
    
    
    
    /* Finalizando */
    fclose(file);
    
    return 0;
}



void stringCopy(char *destiny, char *source) {      /* Análogo ao strcpy. Copia cada caracter de um string para o outro,
                                                        até chegar ao fim ('\0'). Assume que uma string cabe na outra. */
    int i = 0;
    for (i = 0; source[i]!='\0'; i++) {
        (destiny[i]) = (source[i]);
    }
}

void printItem(Item word) {         /* Imprime a chave do Item, seja ela uma palavra ou um token qualquer */
    printf("%s\n", key(&word));
}

void printWord(Item word) {         /* Imprime a chave do Item, mas apenas se for uma palavra */
    char *c = key(&word);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))   /* Consideramos que palavras sempre começam com letras, */
        printf("%s\n", key(&word));                             /* enquanto outros tokens (pontuações e números) não. */
}

void printWordsFromLemma(Item lemma) {      /* Imprime todas as palavras de um lema */
    Item *word;
    
    char *c = key(&lemma);                  /* Se o lema for uma palavra, todos seus derivados também serão */
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

void statisticCountWords(Item word) {       /* Percorre toda a árvore. Só conta em numDifWords se for uma palavra. */
    char *c = key(&word);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        numDifWords ++;
    numDifTokens ++;
}

void statisticCountLemmas(Item lemma) {     /* Percorre a árvore de lemmas, e conta cada um */
    char *c = key(&lemma);
    if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z'))
        numDifLemmas ++;
}

void printUsage () {
    printf ("Possíveis comandos:\n-e [v | V] word\t\tImprime todas as frases em que \"word\" aparece.\n-a [v | V] word\t\tImprime todas as frases em que apareça uma palavra com o mesmo lema que \"word\".\n-t\t\tImprime todos os tokens do texto.\n-d\t\tImprime todas as palavras do texto.\n-l\t\tImprime todos os lemas do texto.\n-L\t\tImprime cada lema do texto, seguido de todas as palavras que têm aquele lema.\n-s\t\tImprime as estatísticas sobre o texto.\n\n\n");
}

