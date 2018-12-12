#include "anasint.h"

Token token;
Token tokenAhead;
meuvetor v;

void my_push(simbolo p){
    if(v.topo >= TAMMAX) error_message(ERROR_STACK_FULL, 0, 0);
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
		error_message(ERROR_STACK_EMPTY, 0, 0);
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

int next_token() {
    token = tokenAhead;
    tokenAhead = verifyToken();
    return 1;
}

int type() {
    return(token.cat == PR && (token.cat == INT || token.cat == REAL || token.cat == CHAR));
}

int check_var() {
    while(type()) {
        next_token();
        if(token.cat == ID) {
            while(next_token() && token.cat == OP && strcmp(token.s, ",") == 0) {
                next_token();
                if(!token.cat == ID) {
                    error_message(ERROR_SINTATICO, get_linha(), get_coluna());
                }
            }
            if(token.cat != PR && strcmp(token.s, "endvar") != 0) {
                error_message(ESPERANDO_ENDVAR, get_linha(), get_coluna());
            }
        } else {
            error_message(ERROR_SINTATICO, get_linha(), get_coluna());
        }
    }
    next_token();
    return 1;
}

int check_declaration_var(simbolo *s) {
    *s = newId(0,"waiting","var",token.s);
    //adiciona o id da variavel dpois
    next_token();
    if(!token.cat == ID) {
        error_message(ERROR_SINTATICO, get_linha(), get_coluna());
    }
    strcpy(s->nome,token.s);
    return 1;
}

int check_param() {
    if(type()) {
        next_token();
        if(!token.cat == ID) {
            error_message(ERROR_SINTATICO, get_linha(), get_coluna());
        }

        while(next_token() && strcmp(token.s, ",") == 0 && token.cat == OP) {
            next_token();
            if(!token.cat == ID) {
                error_message(ERROR_SINTATICO, get_linha(), get_coluna());
            }
        }
    }
    return 1;
}

int check_function() {
    simbolo s;
    next_token();
    if (!token.cat == ID) {
        error_message(ERROR_SINTATICO, get_linha(), get_coluna());
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
                        if(!token.cat == ID) error_message(ESPERANDO_ID, get_linha(), get_coluna());
                    }
                }
                // cmds
                while(check_cmd());
                if(token.n == ENDFUNC) {
                    next_token();
                    return 1;
                } else {
                    error_message(ESPERANDO_ENDFUNC, get_linha(), get_coluna());
                }
            } else {
                error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
            }
        } else {
            error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
        }
    }
}

int check_func_or_fwd() {
    if(type()) return check_function();
    return check_fwd();
}

int check_fwd() {
    next_token();
    if(type()) {
        next_token();
        if (!token.cat == ID) {
            error_message(ERROR_SINTATICO, get_linha(), get_coluna());
        } else {
            next_token();
            if(token.n == ESPERANDO_ABRE_PAREN) {
                next_token();
                if(type()) {
                    while(next_token() && token.n == VIRGULA) {
                        if(!type()) {
                            error_message(ESPERANDO_TIPO, get_linha(), get_coluna());
                        }
                    }
                }

                if(token.n == FECHAPARENTESE) {
                    next_token();
                } else {
                    error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
                }
            } else {
                error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
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
            error_message(ESPERANDO_TERM, get_linha(), get_coluna());
    }else if(!check_term())
                return 0;
    next_token();
    while((token.cat == OP && (token.n == MAIS || token.n==MENOS)) || (token.cat == LOG && token.n == OR)){
        next_token();
        if(!check_term())
            error_message(ESPERANDO_TERM, get_linha(), get_coluna());
    }
    return 1;
}

int check_id(){
    simbolo tmp;
    strcpy(tmp.nome,token.s);
    int pos = findSymbol(tmp.nome);
    if (pos>-1) return 1;
    return 0;
}

int check_exp(){
    next_token();
    if(!check_simp_exp())
        error_message(ESPERANDO_EXP_SIMPLES, get_linha(),get_coluna());
    next_token();
    if(check_op_rel()){
        next_token();
        if(!check_simp_exp())
            error_message(ESPERANDO_EXP_SIMPLES, get_linha(),get_coluna());
    }
    return 1;
}

int check_op_rel() {
    if (token.cat == LOG && token.n < 5) return 1;
    return 0;
}

int check_term() {
    next_token();
    if(token.cat == OP && (strcmp(token.s, "*") == 0 || strcmp(token.s, "/") == 0) && (token.cat == LOG || strcmp(token.s, "not") == 0)) {
        next_token();
    }
    return 1;
}

void start_Token(){
    token = verifyToken();
    tokenAhead = verifyToken();
}

int check_atrib() {
    if(token.cat == ID) {
        next_token();
        if(token.n == IGUAL) {
            if(!check_exp()) error_message(ESPERANDO_IGUAL_SNG, get_linha(), get_coluna());
        } else {
            error_message(ESPERANDO_IGUAL_SNG, get_linha(), get_coluna());
        }
    } else {
        return 0;
    }
}

int check_procedure(){
    simbolo tmp;
    if(token.cat == PR && token.n == PROC){
        next_token();
        if(token.cat != ID)
            error_message(ESPERANDO_ID, get_linha(), get_coluna());
        tmp = newId(1, token.s, "proc", "void");
        my_push(tmp);
        next_token();
        if(token.cat != ABREPARENTESE)
            error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
        next_token();
        if(token.cat != FECHAPARENTESE)
            error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
        while(check_declaration_var(&tmp)){
            tmp = newId(1, token.s,"var", tmp.tipo);
            next_token();
            while(token.cat == OP && token.n == VIRGULA){
                next_token();
                if(!check_id())
                    error_message(ESPERANDO_ID, get_linha(), get_coluna());//erro;
                next_token();
            }
        }
        while(check_cmd()){
        }
        if(!(token.cat == PR && token.n == ENDPROC))error_message(ESPERANDO_ENDPROC, get_linha(), get_coluna());
        next_token();
        return 1;
        }
     else if(token.cat == PR && token.n == FWD){
        next_token();
        if(!(token.cat == PR && token.n == PROC))
            error_message(ESPERANDO_PROC, get_linha(), get_coluna());
        next_token();
        if(token.cat != ID)
            error_message(ESPERANDO_ID, get_linha(), get_coluna());
        next_token();
        if(token.cat != ABREPARENTESE)
            error_message( ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
        next_token();
        if(!(token.cat == PR && type()) )
            error_message( ESPERANDO_FECHA_PAREN, get_linha(), get_coluna() );
        while(next_token() && token.cat == OP && token.n == VIRGULA){
            next_token();
            if(!type()) error_message(ESPERANDO_TIPO, get_linha(), get_coluna());
        }
        if(token.cat != FECHAPARENTESE)
            error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
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
            if(token.cat != ID)error_message(ESPERANDO_ID, get_linha(), get_coluna());
            if(findSymbol(token.s) == 0)
                error_message(NAO_INDENTIFICADO,  get_linha(), get_coluna());
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN,  get_linha(), get_coluna());
            next_token();
            if(check_exp()){
                next_token();
                while(token.cat == OP && token.n == VIRGULA){
                    next_token();
                    if(!check_exp())
                        error_message(ESPERANDO_EXP, get_linha(), get_coluna());
                    next_token();
                }
            }
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
            return 1;
        //comando if
        case IF:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP, get_linha(), get_coluna());
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
            while(check_cmd()){
                next_token();
            }
            if(token.cat == PR && token.n == ELSE){
                next_token();
                if(!check_cmd())
                    error_message(ESPERANDO_CMD, get_linha(), get_coluna());
                next_token();
                while(check_cmd())
                    next_token();
            }
            if(!(token.cat == PR && token.n == ENDELSE))
                error_message(ESPERANDO_ENDELSE, get_linha(), get_coluna());
            return 1;
        case FOR:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
            next_token();
            if(!check_atrib())
                error_message(ESPERANDO_ATRIB,  get_linha(), get_coluna());
            next_token();
            if(!(token.cat == OP && token.n == VIRGULA))
                error_message(ESPERANDO_VIRGULA, get_linha(), get_coluna());
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP, get_linha(), get_coluna());
            next_token();
            if(!(token.cat == OP && token.n == VIRGULA))
                error_message(ESPERANDO_VIRGULA, get_linha(), get_coluna());
            next_token();
            if(!check_atrib())
                error_message(ESPERANDO_ATRIB, get_linha(), get_coluna());
            next_token();
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
            next_token();
            if(!check_cmd())
                error_message(ESPERANDO_CMD, get_linha(), get_coluna());
            next_token();
            while(check_cmd()){
                next_token();
            }
            if(!(token.cat == PR && token.n == ENDFOR))
                error_message(ESPERANDO_ENDFOR, get_linha(), get_coluna());
            return 1;
        case WHILE:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP, get_linha(), get_coluna());
            next_token();
            if(!(token.cat == OP && token.n == FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
            next_token();
            if(!check_cmd())
                error_message(ESPERANDO_CMD, get_linha(), get_coluna());
            next_token();
            while(check_cmd())
                next_token();
            if(!(token.cat == PR && token.n ==  ENDWHILE))
                error_message(ESPERANDO_ENDWHILE, get_linha(), get_coluna());
            return 1;
        case KEYBOARD:
            next_token();
            if(!(token.n == ID))
                error_message(ESPERANDO_ID, get_linha(), get_coluna());
            next_token();
            while(token.cat == OP && token.n == VIRGULA){
                next_token();
                if(!(token.n == ID))
                    error_message(ESPERANDO_ID, get_linha(), get_coluna());
                next_token();
            }
            return 1;
        case DISPLAY:
            next_token();
            if(!(check_id() || token.cat == CT_I || token.cat == CT_R || token.cat == CT_S || token.cat == CT_CH))
                error_message(ESPERANDO_ID_OU_CT_NO_CMD_DISPLAY, get_linha(), get_coluna());
            next_token();
            if(token.cat == PR && token.n == DUP){
                next_token();
                if(!(check_id() || token.cat == CT_I))
                    error_message(ESPERANDO_ID_OU_CT_DUP, get_linha(), get_coluna());
            }
            while(tokenAhead.cat == OP && tokenAhead.n == VIRGULA){
                next_token();
                next_token();
                if(!(check_id() || token.cat == CT_I || token.cat == CT_R || token.cat == CT_S || token.cat == CT_CH))
                    error_message(ESPERANDO_ID_OU_CT_NO_CMD_DISPLAY, get_linha(), get_coluna());
                 if(token.cat == PR && token.n == DUP){
                    next_token();
                    next_token();
                    if(!(check_id() || token.cat == CT_I))
                        error_message(ESPERANDO_ID_OU_CT_DUP, get_linha(), get_coluna());
                 }
            }
            return 1;
        case RETURN:
            next_token();
            if(!(token.cat == OP && token.n == ABREPARENTESE))
                error_message(ESPERANDO_ABRE_PAREN, get_linha(), get_coluna());
            next_token();
            if(!check_exp())
                error_message(ESPERANDO_EXP, get_linha(), get_coluna());
            next_token();
            if(!(token.cat == OP && token.n ==  FECHAPARENTESE))
                error_message(ESPERANDO_FECHA_PAREN, get_linha(), get_coluna());
            return 1;
        default:
          return 0;
        }
        if(token.cat ==ID){
            if(tokenAhead.cat == OP && tokenAhead.n == IGUAL)
                return check_atri();
            else return 0;
        }
        if(token.cat == OP && token.n == PONTO_VIRGULA){
            return 1;
        }
        return 0;
    }
}
