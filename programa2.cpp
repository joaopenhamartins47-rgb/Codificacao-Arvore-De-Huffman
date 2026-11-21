#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


struct Bits
{
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1;
};

union Byte_Huffman
{
    unsigned char byte;
    struct Bits bits;
};


struct Lista_de_registros
{
    int simbolo, frequencia;
    char huffman[100];
    char palavra[50];
    struct Lista_de_registros *prox;
};typedef struct Lista_de_registros listareg;

struct Arvore_Huffman
{
    int info;
    struct Arvore_Huffman *esq, *dir;
};typedef struct Arvore_Huffman huffman;

struct Registro_Arquivo
{
    int simbolo;
    char palavra[50];
    char codigo[100];
};typedef struct Registro_Arquivo regarquivo;

huffman *criar_no(int simbolo)
{
    huffman *novo = (huffman*)malloc(sizeof(huffman));
    novo->info = simbolo;
    novo->dir = NULL;
    novo->esq = NULL;
    return novo;
}

listareg *criar_registro_lido(regarquivo dados)
{
    listareg *novo =(listareg*)malloc(sizeof(listareg));
    novo->simbolo = dados.simbolo;
    novo->frequencia = 0;
    strcpy(novo->huffman, dados.codigo);
    strcpy(novo->palavra, dados.palavra);
    novo->prox = NULL;
    return novo;
}

void inserir_registro_lido(listareg **inicio, regarquivo dados)
{
	//insere no fim da lista
    listareg *aux;
    listareg *novo = criar_registro_lido(dados);
    if(*inicio == NULL)
        *inicio = novo;
    else{
        aux = *inicio;
        while(aux->prox != NULL)
            aux = aux->prox;
        aux->prox = novo;
    }
}

void ler_registros(listareg **inicio)
{
    FILE *Ptrarq = fopen("registros.bin", "rb");
    regarquivo dados;

    if(!Ptrarq)
        printf("Erro ao abrir arquivo!\n");

    else{
        while(fread(&dados, sizeof(regarquivo), 1, Ptrarq) == 1)
            inserir_registro_lido(&*inicio, dados);

        fclose(Ptrarq);
    }
}

void exibir_lista_registros(listareg *inicio)
{
    while(inicio)
    {
        printf("Simbolo: %d Codigo: %s Palavra: %s \n", inicio->simbolo, inicio->huffman, inicio->palavra);
        inicio = inicio->prox;
    }
}

void inserir_codigo_arvore(huffman **raiz, char *codigo, int simbolo)
{
    huffman *atual;
    int i = 0;
    if(!*raiz)
        *raiz = criar_no(-1);

    atual = *raiz;
    while(codigo[i] != '\0')
	{
        if(codigo[i] == '0')
        {
            if(!atual->esq)
                atual->esq = criar_no(-1);
            atual = atual->esq;
        }
        else
        {
            if(!atual->dir)
                atual->dir = criar_no(-1);
            atual = atual->dir;
        }

        i++;
    }

    atual->info = simbolo;
}

void reconstruir_arvore(listareg *inicio, huffman **raiz)
{
    *raiz = NULL;
    while(inicio)
    {
        inserir_codigo_arvore(raiz, inicio->huffman, inicio->simbolo);
        inicio = inicio->prox;
    }

}

void exibir_arvore_vertical(huffman *raiz, int nivel)
{
    int i;

    if(raiz)
    {
        for(i = 0; i < nivel; i++)
            printf("    ");

        printf("%d\n", raiz->info);

        exibir_arvore_vertical(raiz->esq, nivel + 1);
        exibir_arvore_vertical(raiz->dir, nivel + 1);
    }
}

unsigned char *ler_bytes_codificados(FILE *Ptrarq, unsigned long total_bits, int *qtd_bytes)
{
    unsigned char *bytes;
    int i;

    *qtd_bytes = total_bits / 8;
    if(total_bits % 8 != 0)
        (*qtd_bytes)++;

    bytes = (unsigned char *) malloc(*qtd_bytes * sizeof(unsigned char));
    for(i = 0; i < *qtd_bytes; i++)
        fread(&bytes[i], sizeof(unsigned char), 1, Ptrarq);
    return bytes;
}

unsigned char *ler_frase_codificada(unsigned long *total_bits, int *qtd_bytes)
{
    FILE *Ptrarq = fopen("frase_codificada.bin", "rb");
    unsigned char *bytes = NULL;

    if(!Ptrarq)
        printf("Erro ao abrir arquivo!\n");
    else
    {
        fread(total_bits, sizeof(unsigned long), 1, Ptrarq);
        bytes = ler_bytes_codificados(Ptrarq, *total_bits, qtd_bytes);

        fclose(Ptrarq);
    }

    return bytes;
}

char *extrair_bits(unsigned char *bytes, int qtd_bytes, unsigned long total_bits)
{
    char *sequencia = (char*)malloc((total_bits + 1)*sizeof(char));
    union Byte_Huffman byte;
    unsigned long pos = 0;
    int i;

    for(i = 0; i < qtd_bytes && pos < total_bits; i++)
    {
        byte.byte = bytes[i];

        if(pos < total_bits)
        {
            if(byte.bits.b7)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b6)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b5)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b4)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b3)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b2)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b1)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
        if(pos < total_bits)
        {
            if(byte.bits.b0)
                sequencia[pos++] = '1';
            else
                sequencia[pos++] = '0';
        }
    }

    sequencia[pos] = '\0';

    return sequencia;
}


int main(void)
{
    listareg *inicio = NULL;
    huffman *raiz = NULL;

    unsigned long total_bits = 0;
    int qtd_bytes = 0;

    unsigned char *bytes;
    char *bits;

    ler_registros(&inicio);

    reconstruir_arvore(inicio, &raiz);

    printf("\nLISTA DE REGISTROS LIDA:\n");
    exibir_lista_registros(inicio);
	printf("\n---------------------------------\n");
    printf("\nARVORE DE HUFFMAN RECONSTRUIDA:\n\n");
    exibir_arvore_vertical(raiz, 0);

    bytes = ler_frase_codificada(&total_bits, &qtd_bytes);
    bits = extrair_bits(bytes, qtd_bytes, total_bits);
    
	printf("\n---------------------------------\n");
    printf("\nTOTAL DE BITS: %lu\n", total_bits);
    printf("BITS EXTRAIDOS: %s\n", bits);

    return 0;
}
