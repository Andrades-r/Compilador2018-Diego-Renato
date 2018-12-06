#ifndef ERROR_H
#define ERRO_H

enum code_error
{
  ESPERANDO_ID,
  ESPERANDO_FECHA_PAREN,
  ESPERANDO_PT,
  ESPERANDO_ABRE_PAREN,
  ESPERANDO_CLOSE_KEY,
  ESPERANDO_ABRE_KEY,
  ESPERANDO_CMD,
  ESPERANDO_IGUAL_SNG,
  NAO_INDENTIFICADO,
  ESPERANDO_TIPO,
  RETORNO_ESPERADO,
  ATRIBUICAO_INVALIDA,
  ESPERANDO_VIRGULA,
  ERRO_PASSAGEM_ARQUIVO,
  ERRO_ARQUIVO_NAO_ENCONTRADO,
  ERRO_NUMERO_FLOAT_FORMAT,
  FINAL_DO_ARQUIVO
};

void error_message(int flag, int line_number);
#endif // ERRO
