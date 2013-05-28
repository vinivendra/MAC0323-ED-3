#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include "ARNE.h"

int main (int argc, char *argv[]) {
    
    char *fileName;
    FILE *file;
    char c;
    
    /* Leitura de parâmetros */
    if (argc < 2) {
        fprintf(stderr, "Por favor, passe o nome do arquivo de entrada como parâmetro.\n");
        exit(-1);
    }
    fileName = argv[1];
    
    
    /* Abrir o arquivo */
    file = fopen(fileName,"r");
    if (file == NULL) {
        fprintf(stderr, "Erro na leitura do arquivo.\n");
        exit(-1);
    }
    
    
    /* Processamento do arquivo */
    for (fscanf(file, "%c", &c); !feof(file); fscanf(file, "%c", &c)) {
        printf("%c", c);
    }
    
    link head = STinit();
    
    
    /* Finalizando */
    fclose(file);
    
    return 0;
}
