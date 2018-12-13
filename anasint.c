#include "anasint.h"

Token token;
Token tokenAhead;
meuvetor v;

int linha_sint = 0, coluna_sint = 0;

int get_coluna_sint() {
    return coluna_sint;
}

int get_linha_sint() {
    return linha_sint;
}

void my_push(simbolo p){
    if(v.topo >= TAMMAX) error_message(ERROR_STACK_FULL);
    else{
    	++v.topo;
        v.simb[v.topo] = p;
    }
}

simbolo newId(int escopo, char *nome, char *tipo, char *categoria) {
    simbolo criado;
    criado.escopo = escopo;
    strcpy(criado.nome, nome);
    strcpy(criado.cat, categoria);
    strcpy(criado.tipo, tipo);
    return criado;
}

void my_pop(){
    if(v.topo > -1) {
    	v.topo--;
	} else {
		error_message(ERROR_STACK_EMPTY);
	}
}

// TODO: O retorno dessa fun��o � int ou simbolo? Tava como simbolo mas retornava int...
int findSymbol(char *t){
    int tmp = -1;
    int atual = v.topo;
    while(1) {
        tmp = strcmp(t, v.simb[atual].nome);
        if(tmp == 0) return atual;
        if(atual == -1) return -1;
   }
}


int check_declaration_var(simbolo *s) {
    *s = newId(0,"waiting","var",token.s);
    //adiciona o id da variavel dpois
    next_token();
    if(!token.cat == ID) {
        error_message(ERROR_SINTATICO);
    }
    strcpy(s->nome,token.s);
    return 1;
}

int check_param() {
    if(type()) {
        next_token();
        if(!token.cat == ID) {
            error_message(ERROR_SINTATICO);
        }

        while(next_token() && strcmp(token.s, ",") == 0 && token.cat == OP) {
            next_token();
            if(!token.cat == ID) {
                error_message(ERROR_SINTATICO);
            }
        }
    }
    return 1;
}

int check_function() {
    simbolo s;
    next_token();
    if (!token.cat == ID) {
        error_message(ERROR_SINTATICO);
    } else {
        if(token.n == ABREPARENTESE) {
            next_token();
            check_param();
            if (token.n == FECHAPARENTESE) {
                next_token();
                // declarations
                while(check_declaration_var(&s)) {
                        my_push(s);
                    while(next_token() && token.n == VIRGULA) {
                        next_token();
                        if(!token.cat == ID) error_message(ESPERANDO_ID);
                    }
                }
                // cmds
                while(check_cmd());
                if(token.n == ENDFUNC) {
                    next_token();
                    return 1;
                } else {
                    error_message(ESPERANDO_ENDFUNC);
                }
            } else {
                error_message(ESPERANDO_FECHA_PAREN);
            }
        } else {
            error_message(ESPERANDO_ABRE_PAREN);
        }
    }
}

int check_func_or_fwd() {
    if(type())
        return check_function();
    if(token.cat == PR && token.n == PROC)
        return check_procedure();
    if(token.cat == PR && token.n == FWD)
        return check_fwd();
    return 0;
}

int check_fwd() {
    next_token();
    if(type()) {
        next_token();
        if (!token.cat == ID) {
            error_message(ERROR_SINTATICO);
        } else {
            next_token();
            if(token.n == ESPERANDO_ABRE_PAREN) {
                next_token();
                if(type()) {
                    while(next_token() && token.n == VIRGULA) {
                        if(!type()) {
                            error_message(ESPERANDO_TIPO);
                        }
                    }
                }

                if(token.n == FECHAPARENTESE) {
                    next_token();
                } else {
                    error_message(ESPERANDO_FECHA_PAREN);
                }
            } else {
                error_message(ESPERANDO_ABRE_PAREN);
            }
        }
    } else {
        return check_procedure();
    }
    return 0;
}

int check_simp_exp(){
    if(token.cat == OP && (token.n == MAIS || token.n == MENOS)){
        next_token();
        if(!check_term())
            error_message(ESPERANDO_TERM);
    }else if(!check_term())
                return 0;
    while((tokenAhead.cat == OP && (tokenAhead.n == MAIS || tokenAhead.n==MENOS)) || (tokenAhead.cat == LOG && tokenAhead.n == OR)){
        next_token();
        next_token();
        if(!check_term())
            error_message(ESPERANDO_TERM);
    }
    return 1;
}

int check_exp(){
    next_token();
    if(!check_simp_exp())
        error_message(ESPERANDO_EXP_SIMPLES);
    next_token();
    if(check_op_rel()){
        next_token();
        if(!check_simp_exp())
            error_message(ESPERANDO_EXP_SIMPLES);
    }
    return 1;
}

int check_op_rel() {
    if (token.cat == LOG && token.n < 5) return 1;
    return 0;
}

int check_term() {
    next_token();
    if((token.cat == OP && (token.n == MULTIPLICACAO || token.n == DIVISAO)) || (token.cat == LOG || token.n == NOT)) {
        next_token();
    }
    return 1;
}

int type() {
    return (token.cat == PR && (token.n == INT || token.n == REAL || token.n == CHAR));
}

int check_id(){
    simbolo tmp;
    strcpy(tmp.nome,token.s);
    int pos = findSymbol(tmp.nome);
    if (pos>-1) return 1;
    return 0;
}

int check_var() {
        // printf("1[%s] [%d]\n", token.s, token.n);
    while(type()) {
        next_token();
        if(token.cat == ID) {
            next_token();
            while(next_token() && token.cat == OP && token.n == VIRGULA) {
                if(!(token.cat == ID)) {
                    error_message(ERROR_SINTATICO);
                }
            }
        } else {
            error_message(ERROR_SINTATICO);
        }
    }
    next_token();
    if(!(token.cat == PR && token.n == ENDVAR)) {
        error_message(ESPERANDO_ENDVAR);
    }
    // next_token();
    return 1;
}

int next_token() {
    token = tokenAhead;
    printToken();
    if(token.cat == -1){
            error_message(FINAL_DO_ARQUIVO);
            //EOF
    }
    linha_sint = get_linha();
    coluna_sint = get_coluna();
    tokenAhead = verifyToken();
   // printf("Token - Categoria: [%d], valor: [%s], linha: [%d] e coluna: [%d]\n", token.cat, token.s, linha_sint, coluna_sint);
    return 1;
}

void start_Token() {
    token = verifyToken();
    tokenAhead = verifyToken();

    // printf("Token Start: [%s] [%s]\n", token.s, tokenAhead.s);

    if (token.cat == PR && token.n == PL) {
        next_token();
        if (token.cat == ID) {
                next_token();
           if (token.cat == PR && token.n == VAR) {
               next_token();
               check_var();
               next_token();
           }
           while(check_func_or_fwd()) {
               next_token();
           }
           if (token.cat == PR && token.n == PROG) {
               next_token();
               if (check_cmd()) {
                    next_token();
                    while (check_cmd()) {
                        next_token();
                    }
                    if (!(token.cat == ENDPROG)) {
                        error_message(ERROR_ENDPROG);
                    }
               } else {
                    error_message(ESPERANDO_CMD);
               }
           }
        } else {
            error_message(ERROR_NO_RESERVED_WORD);
        }
    }
}

int check_atrib() {
    printToken();
    next_token();
    if(token.cat == ID) {
       return 1;
    } else {
        return 0;
    }
}

int check_procedure(){
    simbolo tmp;
    if(token.cat == PR && token.n == PROC){
        next_token();
        if(token.cat != ID)
            error_message(ESPERANDO_ID);
        tmp = newId(1, token.s, "proc", "void");
        my_push(tmp);
        next_token();
        if(token.cat != ABREPARENTESE)
            error_message(ESPERANDO_ABRE_PAREN);
        next_token();
        if(token.cat != FECHAPARENTESE)
            error_message(ESPERANDO_FECHA_PAREN);
        while(check_declaration_var(&tmp)){
            tmp = newId(1, token.s,"var", tmp.tipo);
            next_token();
            while(token.cat == OP && token.n == VIRGULA){
                next_token();
                if(!check_id())
                    error_message(ESPERANDO_ID);//erro;
                next_token();
            }
        }
        while(check_cmd()){
                next_token();
        }
        if(!(token.cat == PR && token.n == ENDPROC)) error_message(ESPERANDO_ENDPROC);
        next_token();
        return 1;
        }
     else if(token.cat == PR && token.n == FWD){
        next_token();
        if(!(token.cat == PR && token.n == PROC))
            error_message(ESPERANDO_PROC);
        next_token();
        if(token.cat != ID)
            error_message(ESPERANDO_ID);
        next_token();
        if(token.cat != ABREPARENTESE)
            error_message( ESPERANDO_ABRE_PAREN);
        next_token();
        if(!(token.cat == PR && type()) )
            error_message( ESPERANDO_FECHA_PAREN );
        while(next_token() && token.cat == OP && token.n == VIRGULA){
            next_token();
            if(!type()) error_message(ESPERANDO_TIPO);
        }
        if(token.cat != FECHAPARENTESE)
            error_message(ESPERANDO_FECHA_PAREN);
        next_token();
        return 1;
        }
    return 0;
}



int check_cmd(){
    if(token.cat == PR){
        switch(token.n){
        //comando call
        case CALL:
            next_token();
            if(token.cat != ID)error_message(ESPERANDO_ID);
            if(findSymbol(token.s) == 0)
                error_message(NAO_INDENTIFICADO);
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN);
            next_token();
            if(check_exp()){
                next_token();
                while(token.cat == OP && token.n == VIRGULA){
                    next_token();
                    if(!check_exp())
                        error_message(ESPERANDO_EXP);
                    next_token();
                }
            }
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN);
            return 1;
        //comando if
        case IF:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN);
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP);
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN);
            next_token();
            while(check_cmd()){
                next_token();
            }
            if(token.cat == PR && token.n == ELSE){
                    printf("else\n");
                next_token();
                if(!check_cmd())
                    error_message(ESPERANDO_CMD);
                next_token();
                while( check_cmd()){
                 next_token();
                }
            }
            if(!(token.cat == PR && token.n == ENDIF)){
                error_message(ESPERANDO_ENDIF);
            }
            next_token();
            return 1;
        case FOR:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN);
            next_token();
            if(!check_atrib())
                error_message(ESPERANDO_ATRIB);
            next_token();
            if(!(token.cat == OP && token.n == VIRGULA))
                error_message(ESPERANDO_VIRGULA);
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP);
            next_token();
            if(!(token.cat == OP && token.n == VIRGULA))
                error_message(ESPERANDO_VIRGULA);
            next_token();
            if(!check_atrib())
                error_message(ESPERANDO_ATRIB);
            next_token();
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN);
            next_token();
            if(!check_cmd())
                error_message(ESPERANDO_CMD);
            next_token();
            while(check_cmd()){
                next_token();
            }
            if(!(token.cat == PR && token.n == ENDFOR))
                error_message(ESPERANDO_ENDFOR);
            return 1;
        case WHILE:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN);
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP);
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN);
            next_token();
            if(!check_cmd())
                error_message(ESPERANDO_CMD);
            next_token();
            while(check_cmd())
                next_token();
            if(!(token.cat == PR && token.n ==  ENDWHILE))
                error_message(ESPERANDO_ENDWHILE);
            return 1;
        case KEYBOARD:
            next_token();
            if(!(token.cat == ID))
                error_message(ESPERANDO_ID);
            next_token();
            while(token.cat == OP && token.n == VIRGULA){
                next_token();
                if(!(token.cat == ID))
                    error_message(ESPERANDO_ID);
                next_token();
            }
            return 1;
        case DISPLAY:
            next_token();
            if(!(check_id() || token.cat == CT_I || token.cat == CT_R || token.cat == CT_S || token.cat == CT_CH))
                error_message(ESPERANDO_ID_OU_CT_NO_CMD_DISPLAY);
            next_token();
            if(token.cat == PR && token.n == DUP){
                next_token();
                if(!(check_id() || token.cat == CT_I))
                    error_message(ESPERANDO_ID_OU_CT_DUP);
            }
            while(tokenAhead.cat == OP && tokenAhead.n == VIRGULA){
                next_token();
                next_token();
                if(!(check_id() || token.cat == CT_I || token.cat == CT_R || token.cat == CT_S || token.cat == CT_CH))
                    error_message(ESPERANDO_ID_OU_CT_NO_CMD_DISPLAY);
                 if(token.cat == PR && token.n == DUP){
                    next_token();
                    next_token();
                    if(!(check_id() || token.cat == CT_I))
                        error_message(ESPERANDO_ID_OU_CT_DUP);
                 }
            }
            return 1;
        case RETURN:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN);
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP);
            next_token();
            if(!(token.cat == OP && token.n ==  FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN);
            return 1;
        default:
        printToken();
        printf("   esse token nao e cmd\n");
          return 0;
        }//end switch
    }
        //atribuiçao
    if(token.cat == ID){
            printf("entrei aqui");
        if(tokenAhead.cat == OP && tokenAhead.n == ATRIBUICAO){
            next_token();
            return check_atrib();
        }
        else return 0;
    }
    if(token.cat == OP && token.n == PONTO_VIRGULA){
        return 1;
    }
    return 0;
}
