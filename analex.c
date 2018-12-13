#include "analex.h"
#include "error.h"
#define TAM 20
#define TAM_RES 25

int coluna = 1, linha = 1;
char fileName[TAM];
FILE *codFonte;
Token token;

int get_linha() {
	return linha;
}

int get_coluna() {
	return coluna;
}

char opString[][TAM]={
    "MAIS", "MENOS", "ABREPARENTESE", "FECHAPARENTESE",
    "ABRECOLCHETE", "FECHACOLCHETE", "PONTO_VIRGULA","VIRGULA",
     "DIVISAO", "ATRIBUICAO", "MULTIPLICACAO"
};
char LOGstring[][TAM]={
 "IGUAL", "HASTAG", "MAIOR", "MENOR", "MENOROUIGUAL", "MAIROUIGUAL",
  "AND", "OR", "NOT"
};

char palavrasRes[][TAM] = {
    "bool", "call", "char", "display", "dup",
    "else", "endfor", "endfunc", "endif", "endproc", "endprog",
    "endvar", "endwhile", "for", "fwd",
    "if", "int", "keyboard", "noparam", "pl",
    "proc", "prog", "real", "return", "var", "while"
};

int isPalavraRes(char *s) {
    int a = buscaBinaria(s, palavrasRes, 0, TAM_RES);
    // printf("[%d]\n", a);
    return a;
}

char getCaracter(){
    char c = fgetc(codFonte);
    return c;
}

void concat(char *string, char c) {
    int size = strlen(string);
    string[size] = c;
    string[size + 1] = '\0';
    coluna++;
}

int buscaBinaria(char *palavra, char palavrasRes[][TAM], int ini, int fim){
    int cmp;
    cmp = strcmp(palavra, palavrasRes[(ini+fim)/2]);
    // printf("Search Binary: [%d] [%s]\n", cmp, palavra);
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
        return buscaBinaria(palavra, palavrasRes,((ini + fim)/2) +1, fim);
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
        returnToken.n = atoi(buffer);
        // strcpy(returnToken.s, (char *) buffer);
    } else if (type == CT_I) {
        returnToken.n = atoi(buffer);
    } else if (type == CT_R) {
        returnToken.r = atof(buffer);
    } else if (type == LOG) {
        returnToken.n = atoi(buffer);
    } else if (type == OP) {
        returnToken.n = atoi(buffer);
    }else if (type == -1){
            returnToken.n = atoi(buffer);
    }    //printToken();
    return returnToken;
}

void clearBuffer(char *buffer) {
	memset(buffer, 0, 199);
}

FILE* openFile() {
    //coloque nome do arquivo sem a extensao .txt o arquivo deve estar na pasta do projeto
	printf("Digite o nome do arquivo desejado: ");
	scanf("%s",fileName);
	printf("****************************************************************************\n");
    printf("*   ============================ OPEN FILE =============================   *\n");
    printf("****************************************************************************\n\n");
    strcat(fileName,".txt");
    codFonte = fopen(fileName, "r");
    if (codFonte == NULL) {
        printf("erro ao abrir o arquivo\n");
        exit(-1);
    }
}

Token verifyToken() {
    int estado = 0;
    char buffer[200], c;
    memset(buffer, 0, 199);

    while(1) {
        switch(estado)
        {
            case 0:
                c = getCaracter();
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
                    coluna--;
                    estado = 0;
                    clearBuffer(buffer);
                    break;
                }else if (c == '\''){
                    estado = 39;
                } else if(c == '('){
                    estado = 42;
                }else if(c == ')'){
                    estado = 43;
                }else if(c == '['){
                    estado = 44;
                }else if(c == ']'){
                    estado = 45;
                }else if(c == ','){
                    estado = 46;
                }else if(c == ';'){
                    estado = 47;
                }else if(c == '\n') {
                    linha++;
                    coluna = 1;
                    estado = 0;
                    break;
                }else if (feof(codFonte)) {
                    error_message(FINAL_DO_ARQUIVO);
                    token.cat = -1;
                    printf("eof reached\n");
                }
                concat(buffer, c);
                break;
            case 1:
            	c = getCaracter();
            	if(!(isalpha(c) || isdigit(c))) estado = 2;
                concat(buffer, c);
                break;
            case 2:
                // FINAL Lexema
                ungetc(desconcat(buffer), codFonte);
                int tmp = isPalavraRes(buffer);
                if(tmp == -1){
                	return createToken(ID, buffer);
                } else {
                    itoa(tmp, buffer, 10);
                    return createToken(PR, buffer);
                }
            case 3:
                c = getCaracter();
                if(isdigit(c)) {
                	concat(buffer, c);
                }else if (c ==  '.') {
                    concat(buffer, c);
                    estado = 4;
                }else estado = 5;
                break;
            case 4:
                c = getCaracter();
                if(isdigit(c)) {
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
            case 6:
                c = getCaracter();
                if( isdigit(c) ){
                    concat(buffer, c);
                }else {
                    estado = 7;
                    concat(buffer, c);
                }
                break;
            case 7:
                //FINAL NUMERO REAL
                return createToken(CT_R, buffer);
                break;
            case 8:
                //FINAL MAIS
                itoa(MAIS, buffer, 10);
                return createToken(OP, buffer);
                break;
            case 9:
                c = getCaracter();
                if(c == '=') {
                    concat(buffer, c);
                    estado = 10;
                } else {
                    desconcat(buffer);
                    estado = 11;
                }
                break;
            case 10:
                //FINAL MENOR IGUAL
                itoa(MENOROUIGUAL, buffer, 10);
                return createToken(LOG,buffer);
            case 11:
                //FINAL MENOR
                desconcat(buffer);
                itoa(MENOR, buffer, 10);
                return createToken(LOG, buffer);
                break;
            case 12:
                c = getCaracter();
                if(c == '=') {
                    concat(buffer, c);
                    estado = 13;
                }else {
                    desconcat(buffer);
                    estado = 14;
                }
                break;
            case 13:
                //FINAL MAIOROUIGUAL
                itoa(MAIROUIGUAL, buffer, 10);
                return createToken(LOG,buffer);
            case 14:
                //FINAL MAIOR
                itoa(MAIOR, buffer, 10);
                return createToken(LOG, buffer);
            case 15:
                //FINAL MENOS
                itoa(MENOS, buffer, 10);
                return createToken(OP,buffer);
            case 16:
                c = getCaracter();
                if( c == 'a') {
                    concat(buffer, c);
                    estado = 17;
                }else if (c == 'o') {
                    concat(buffer, c);
                    estado = 21;
                }else if (c == 'n'){
                    concat(buffer, c);
                    estado = 25;
                }
				else error_message(ESPERANDO_ID);
                break;
            case 17:
                c = getCaracter();
                if(c == 'n'){
                    concat(buffer, c);
                    estado = 18;
                }
				else error_message(ESPERANDO_ID);
                break;
            case 18:
                c = getCaracter();
                if(c == 'd') {
                    concat(buffer, c);
                    estado = 19;
                }
                else error_message(ESPERANDO_ID);
                break;
            case 19:
                c = getCaracter();
                if(c == '.') {
                    concat(buffer, c);
                    estado = 20;
                }
                else error_message(ESPERANDO_ID);
                break;
            case 20:
                //FINAL .and.
                itoa(AND, buffer, 10);
                return createToken(LOG, buffer);
            case 21:
                c = getCaracter();
                if(c == 'r') {
                    concat(buffer, c);
                    estado = 22;
                }
				else error_message(ESPERANDO_ID);
                break;
            case 22:
                c = getCaracter();
                if(c == '.') {
                    concat(buffer, c);
                    estado = 26;
                }
                break;
            case 23:
                c = getCaracter();
                if(c == 'o') {
                    concat(buffer, c);
                    estado = 24;
                }
                else error_message(ESPERANDO_ID);
                break;
            case 24:
                c = getCaracter();
                if(c == 't') {
                    concat(buffer, c);
                    estado = 25;
                }
                else error_message(ESPERANDO_ID);
                break;
            case 25:
                c = getCaracter();
                if(c == '.') {
                    concat(buffer, c);
                    estado = 20;
                }
				else error_message(ESPERANDO_ID);
                break;
            case 26:
                //FINAL .OR.
                itoa(OR, buffer, 10);
                return createToken(LOG, buffer);
            case 27:
                //FINAL  .NOT.
                itoa(NOT, buffer, 10);
                return createToken(LOG, buffer);
            case 28:
                c = getCaracter();
                if(c == '=') {
                    concat(buffer, c);
                    estado = 29;
                }else estado = 30;
                break;
            case 29:
                //FINAL ==
                itoa(IGUAL, buffer, 10);
                return createToken(LOG, buffer);
            case 30:
                //FINAL ATRIBUICAO
                desconcat(buffer);
                ungetc(c,codFonte);
                itoa(ATRIBUICAO, buffer, 10);
                return createToken(OP, buffer);
            case 31:
                c = getCaracter();
                if(c == '"') estado = 32;
                concat(buffer, c);
                break;
            case 32:
                return createToken(CT_S, buffer);
            case 33:
                c = getCaracter();
                if(c == '/') {
                }else estado = 36;
                break;
            case 34:
                c = getCaracter();
                if(c == '\n') {
                    estado = 35;
                }
                break;
            case 35:
                estado = 0;
                break;
            case 36:
                //FINAL DIVISAO
                desconcat(buffer);
                itoa(DIVISAO, buffer, 10);
                return createToken(OP, buffer);
            case 37:
                //FINAL MULTIPLICACAO
                itoa(MULTIPLICACAO, buffer, 10);
                return createToken(OP,buffer);
            case 38:
                //FINAL HASTAG
                itoa(HASTAG, buffer, 10);
                return createToken(LOG, buffer);
            case 39:
                c = getCaracter();
                concat(buffer, c);
				estado = 40;
				break;
            case 40:
                c = getCaracter();
                if(c == '\'') {
	                concat(buffer, c);
	                estado = 41;
				}else {
					error_message(ESPERANDO_FECHA_APOSTROFO);
				}
				break;
            case 41:
                return createToken(CT_CH, buffer);
	     	case 42:
                itoa(ABREPARENTESE, buffer, 10);
                return createToken(OP, buffer);
	    	case 43:
                itoa(FECHAPARENTESE, buffer, 10);
                return createToken(OP, buffer);
	   		case 44:
                itoa(ABRECOLCHETE, buffer, 10);
                return createToken(OP, buffer);
	   		case 45:
                itoa(FECHACOLCHETE, buffer, 10);
                return createToken(OP, buffer);
	      	case 46:
                itoa(VIRGULA, buffer, 10);
                return createToken(OP, buffer);
	      	case 47:
                itoa(PONTO_VIRGULA, buffer, 10);
                return createToken(OP, buffer);
            default:
                itoa(-1,buffer,10);
                return createToken(-1,buffer);
        }//fim switch
    } //fim while
}

void close_file() {
    fclose(codFonte);
}

void printToken(){
    if(token.cat ==PR){
            printf("< PR , %s >\n",palavrasRes[token.n]);
    }
    else if(token.cat == ID)
        printf("< ID , %s >\n",token.s);
    else if(token.cat == OP)
        printf("< OP , %s >\n",opString[token.n]);
    else if(token.cat == CT_I)
        printf("< INTEIRO, %d>\n",token.n);
    else if(token.cat == CT_R)
        printf("< REAL, %f>\n",token.r);
    else if(token.cat == LOG)
        printf("< LOG, %s>\n", LOGstring[token.n]);

    else{
        printf("< %d, %s>\n",token.cat,token.s);
    }

}
