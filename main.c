#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include "ARNE.h"

#define STRING_MAX 50

int main (int argc, char *argv[]) {
    
    char *fileName = NULL;
    FILE *file = NULL;
    char *c = malloc(STRING_MAX*sizeof(char));
    link words = NULL;
    link lemmas = NULL;
    
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
    for (fscanf(file, "%s", c); !feof(file); fscanf(file, "%c", c)) {
        printf("%c", *c);
    }
    
    initItem();
    
    words = STinit();
    lemmas = STinit();
    
    
    /* Finalizando */
    fclose(file);
    
    return 0;
}
