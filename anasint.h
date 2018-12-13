#ifndef ANASINT_H
#define ANASINT_H

#include "analex.h"
#include "error.h"

#define TAMMAX 200

//estrutura do dado da tabela de simbolos
typedef struct{
    int escopo;
    char nome[20];
    char cat[10];
    char tipo[10];
} simbolo;

//Vetor utilizado para a tabela de simbolos
typedef struct{
    int topo;
    simbolo simb[200];
} meuvetor;

void my_push(simbolo p);
void my_pop();
simbolo newId(int escopo,char *nome,char *cat,char *tipo);
int findSymbol(char *t);
int next_token();
int type();
int check_var();
int check_declaration_var(simbolo *s);
int check_param();
int check_function();
int check_func_or_fwd();
int check_fwd();
int check_simp_exp();
int check_id();
int check_exp();
int check_op_rel();
int check_term();
void start_Token();
int check_atrib();
int check_procedure();
int check_cmd();
void printToken();




#endif
