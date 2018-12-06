#include "analex.h"
#include "error.h"

#define TAM 20

char palavrasRes[][TAM] = {
    "bool", "call", "char", "display", "else", "endfor", "endif", "endproc",
    "endprog", "endvar", "endwhile", "for", "fwd", "id", "if", "int", "keyboard",
    "noparam", "pl", "proc", "prog", "real", "return", "var", "while"
};

int isPalavraRes(char *s){
    return buscaBinaria(s, palavrasRes, 0, TAM-1);
}

char getCaracter(FILE *p, int col, int linha){
    char c = fgetc(p);
        if (c == '\n') {
        linha++;
        col = 0;
    } else col++;
    if (c == EOF) {
    	error_message(16, linha);
		exit(0);
	}
    return c;
}

void concat(char *string, char c) {
    int size = strlen(string);
    string[size] = c;
    string[size + 1] = '\0';
}

int buscaBinaria(char *palavra, char palavrasRes[][TAM], int ini, int fim){
    int cmp;
    cmp = strcmp(palavra, palavrasRes[(ini+fim)/2]);
    if(cmp == 0) //ACHOU
        return (ini + fim)/2;    
    if(cmp < 0) {
        // palavra menor que a metade
        if(ini >= fim) 
            return -1;
        return buscaBinaria(palavra, palavrasRes,ini , ((ini+fim)/2) -1);
    }
    if (cmp > 0){
        //palavra maior que a metade
        if(ini >= fim)
            return -1;
        return buscaBinaria(palavra,palavrasRes,((ini + fim)/2) +1, fim);
    }
}

char desconcat(char *string){
    int size = strlen(string);
    char c = string[size - 1];
    if(size > 0) {
    	string[size - 1] = '\0';
	}
    return c;
}

Token createToken(categoria type, void *buffer)
{
    Token returnToken;
    returnToken.cat = type;
    if (type == ID) {
        strcpy(returnToken.s, (char *) buffer);
    } else if (type == PR) {
        strcpy(returnToken.s, (char *) buffer);
    } else if (type == CT_I) {
        returnToken.n = atoi(buffer);
    } else if (type == CT_R) {
        returnToken.r = atof(buffer);
    } else if (type == LOG) {
        returnToken.n = atoi(buffer);
    } else if (type == OP) {
        strcpy(returnToken.s, (char *) buffer);
    }
    return returnToken;
}

Token verifyToken() {
    FILE *codFonte;
    char c;
    char buffer[20];
    memset(buffer, 0, 19);
    int estado, coluna=0, linha=0;
    Token token;
    
    printf("================ Abrindo arquivo ==============\n");

    codFonte = fopen("teste.txt", "r");
    
    if (codFonte == NULL) {
        printf("erro ao abrir o arquivo\n");
        exit(-1);
    }
    int i = 0;
    estado = 0;
    while(1) {
        switch(estado)
        {
            case 0:
                c = getCaracter(codFonte,coluna,linha);
                concat(buffer, c);                
                if(isalpha(c)){
                    estado = 1;
                }else if(isdigit(c)){
                    estado = 3;
                }else if (c == '<'){
                    estado = 9;
                }else if (c == '>'){
                    estado = 12;
                }else if (c == '.'){
                    estado = 16;
                }else if (c == '='){
                    estado = 28;
                }else if (c == '-'){
                    estado = 15;
                }else if (c == '+'){
                    estado = 8;
                }else if (c == '"'){
                    estado = 31;
                }else if (c == '#'){
                    estado = 38;
                }else if (c == '/'){
                    estado = 33;
                }else if (c == '*'){
                    estado = 37;
                }else if (c == ' '){
                    estado = 0;
                }else if (c == '\''){
                    estado =39;
                }else if (feof(codFonte)){
                    error_message(FINAL_DO_ARQUIVO, -1);
                    break;
                }
                break;
            case 1:
                c = getCaracter(codFonte, coluna, linha);
                if(!(isalnum(c))){
                    estado = 2;
                }
                i++;
                concat(buffer, c);
                break;
            case 2:
                // FINAL Lexema
                desconcat(buffer);
                int tmp = isPalavraRes(buffer);
                if(tmp){
                    // case seja palavra reservadoa identificar qual a palavra reservada
                     return createToken(tmp, buffer);
                }else {
                     return createToken(0, buffer);
                }
                return token;
                break;
            case 3:
                c = getCaracter(codFonte, coluna, linha);
                if(isdigit(c)){
                    i++;
                    concat(buffer, c);
                }else if (c ==  '.'){
                    i++;
                    concat(buffer, c);
                    estado = 4;
                }else 
                    estado = 5;
                break;
            case 4:
                c = getCaracter(codFonte, coluna, linha);
                if(isdigit(c)){
                    i++;
                    concat(buffer, c);
                }else {
                    //volta um caracter do arquivo e vai para o estado 5
                    desconcat(buffer);
                    estado = 5;
                }
                break;
            case 5:
                //FINAL
                desconcat(buffer);
                return createToken(3, buffer);
                break;
            case 6:
                c = getCaracter(codFonte, coluna, linha);
                if( isdigit(c) ){
                    concat(buffer, c);
                    i++;
                }else {
                    estado = 7;
                    concat(buffer, c);
                    i++;
                }
                break;
            case 7:
                //FINAL NUMERO REAL
                 return createToken(CT_R, buffer);
                break;
            case 8:
                //FINAL MAIS
                return createToken(OP, buffer);
                break;
            case 9:
                c = getCaracter(codFonte, coluna, linha);
                if(c == '='){
                    concat(buffer, c);
                    i++;
                    estado = 10;
                } else {
                    desconcat(buffer);
                    estado = 11;
                }
                break;
            case 10:
                //FINAL MENOR IGUAL
                 return createToken(OP,buffer);
                break;
            case 11:
                //FINAL MENOR 
                desconcat(buffer);
                break;
            case 12:
                c = getCaracter(codFonte, coluna, linha);
                if(c == '='){
                    concat(buffer, c);
                    i++;
                    estado = 13;
                }else {
                    desconcat(buffer);
                    estado = 14;
                }
                break;
            case 13:
                //FINAL MAIOROUIGUAL
                 return createToken(OP,buffer);
                break;
            case 14:
                //FINAL MAIOR
                 return createToken(OP, buffer);
                break;
            case 15:
                //FINAL MENOS
                 return createToken(OP,buffer);
                break;
            case 16:
                c = getCaracter(codFonte,coluna,linha);
                if( c == 'a'){
                    concat(buffer, c);
                    i++;
                    estado = 17;
                }else if (c == 'o'){
                    concat(buffer, c);
                    i++;
                    estado = 21;
                }else if (c == 'n'){
                    concat(buffer, c);
                    i++;
                    estado = 25;
                } 
//				else error_message();
                break;
            case 17:
                c = getCaracter(codFonte,coluna,linha);
                if(c == 'n'){
                    concat(buffer, c);
                    i++;
                    estado = 18;
                }
//				else error_message();
                break;
            case 18:
                c = getCaracter(codFonte,coluna,linha);
                if(c == 'd'){
                    concat(buffer, c);
                    i++;
                    estado = 19;
                }
//				else error_message();
                break;
            case 19:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '.'){
                    concat(buffer, c);
                    i++;
                    estado = 20;
                }
//				else error_message();
                break;
            case 20:
                //FINAL .and.
                 return createToken(LOG, buffer);
                break;
            case 21:
                c = getCaracter(codFonte,coluna,linha);
                if(c == 'r'){
                    concat(buffer, c);
                    i++;
                    estado = 22;
                }
//				else error_message();
                break;
            case 22:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '.'){
                    concat(buffer, c);
                    i++;
                    estado = 26;
                }
                break;
            case 23:
                c = getCaracter(codFonte,coluna,linha);
                if(c == 'o'){
                    concat(buffer, c);
                    i++;
                    estado = 24;
                }
//				else error_message();
                break;
            case 24:
                c = getCaracter(codFonte,coluna,linha);
                if(c == 't'){
                    concat(buffer, c);
                    i++;
                    estado = 25;
                }
//				else error_message();
                break;
            case 25:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '.'){
                    concat(buffer, c);
                    i++;
                    estado = 20;
                }
//				else error_message();
                break;
            case 26:
                //FINAL .OR.
                 return createToken(LOG, buffer);
                break;
            case 27:
                //FINAL  .NOT.
                token = createToken(OP, buffer);
                break;
            case 28:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '='){
                    i++;
                    concat(buffer, c);
                    estado = 29;
                }else estado = 30;
                break;
            case 29:
                //FINAL ==
                token = createToken(OP, buffer);
                break;
            case 30:
                //FINAL DIVISAO
                desconcat(buffer);
                token = createToken(OP,buffer);
                break;
            case 31:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '"')
                    estado = 32;
                i++;
                concat(buffer, c);
            case 32:
                token = createToken(CT_S, buffer);
                break;
            case 33:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '/'){
                }else estado = 36;
                break;
            case 34:
                c = getCaracter(codFonte,coluna,linha);
                if(c == '\n'){
                    estado = 35;
                }
                break;
            case 35:
                estado = 0;
                break;
            case 36:
                 return createToken(OP, buffer);
                break;
            case 37:
                break;
            case 38:
                token = createToken(OP, buffer);
                break;
            case 39:
                c = getCaracter(codFonte,coluna,linha);
            case 40:
                 c = getCaracter(codFonte,coluna,linha);
                estado = 41;
            case 41:
                token = createToken(CT_CH, buffer);
            default:
                error_message(FINAL_DO_ARQUIVO, linha);
        }//fim switch
    } //fim while

    fclose(codFonte);
    return token;
}


