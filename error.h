#ifndef ERROR_H
#define ERRO_H

enum code_error
{
  ESPERANDO_ID,
  ESPERANDO_FECHA_APOSTROFO,
  ESPERANDO_FECHA_PAREN,
  ESPERANDO_PT,
  ESPERANDO_ABRE_PAREN,
  ESPERANDO_CLOSE_KEY,
  ESPERANDO_ABRE_KEY,
  ESPERANDO_CMD,
  ESPERANDO_IGUAL_SNG,
  ESPERANDO_ENDVAR,
  NAO_INDENTIFICADO,
  ESPERANDO_TIPO,
  RETORNO_ESPERADO,
  ATRIBUICAO_INVALIDA,
  ESPERANDO_VIRGULA,
  ERRO_PASSAGEM_ARQUIVO,
  ERRO_ARQUIVO_NAO_ENCONTRADO,
  ERRO_NUMERO_FLOAT_FORMAT,
  FINAL_DO_ARQUIVO,
  ERROR_SINTATICO,
  ERROR_STACK_FULL,
  ERROR_STACK_EMPTY,
  ESPERANDO_PROC,
  ESPERANDO_ENDPROC,
  ESPERANDO_EXP,
  ESPERANDO_ELSE,
  ESPERANDO_ENDELSE,
  ESPERANDO_ATRIB,
  ESPERANDO_ENDFOR,
  ESPERANDO_ENDWHILE,
  ESPERANDO_ID_OU_CT_NO_CMD_DISPLAY,
  ESPERANDO_ID_OU_CT_DUP,
  ESPERANDO_ENDFUNC,
  ESPERANDO_TERM,
  ESPERANDO_EXP_SIMPLES,
  ERROR_NO_RESERVED_WORD,
  ERROR_ENDPROG,
  ERROR_ENDFUNC,
  ESPERANDO_ENDIF,
  ESPERANDO_FATOR
};

void error_message(int flag);
#endif // ERRO
