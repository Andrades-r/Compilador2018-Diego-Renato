#ifndef ANALEX_H
#define ANALEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TAM 20

// CATEGORIA
typedef enum{ ID, PR, OP, CT_I, CT_R, CT_CH, CT_S, LOG } categoria;

// OPERADORES
typedef enum{
    MAIS, MENOS, ABREPARENTESE, FECHAPARENTESE,
    ABRECOLCHETE, FECHACOLCHETE, PONTO_VIRGULA,
    VIRGULA, DIVISAO,ATRIBUICAO,MULTIPLICACAO
} operador;

typedef enum{
 IGUAL, HASTAG, MAIOR, MENOR, MENOROUIGUAL, MAIROUIGUAL,
  AND, OR, NOT
}log;

// PALAVRAS RESERVADAS
typedef  enum {
    BOOL, CALL, CHAR, DISPLAY, DUP,
    ELSE, ENDFOR, ENDFUNC, ENDIF, ENDPROC, ENDPROG,
    ENDVAR, ENDWHILE, FOR,
    FWD, IF, INT, KEYBOARD, NOPARAM,
    PL, PROC, PROG, REAL, RETURN,
    VAR, WHILE
} palavra;

// ESTRUTURA DO TOKEN
typedef struct {
    categoria cat;
    union{ int n; float r; char s[10]; };
} Token;

FILE* openFile();
Token createToken(categoria type, void *buffer);
int isPalavraRes(char s[]);
int buscaBinaria(char *palavra, char palavrasRes[][TAM], int ini, int fim);
char getCaracter();
Token verifyToken();
void concat(char *string, char c);
int get_linha();
int get_coluna();
void close_file();
//void printToken();
#endif
