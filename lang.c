#include "lang.h"

int lispy_parse(char* filename, char* input, mpc_result_t* r){
  /* Define Parsers */
  mpc_parser_t* Int = mpc_new("int");
  mpc_parser_t* Decimal = mpc_new("decimal");
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Function = mpc_new("function");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                            \
      decimal  : /-?[0-9]+\\.[0-9]+/ ;                           \
      int      : /-?[0-9]+/ ;                                    \
      number   : <decimal> | <int>;                              \
      function : \"min\" | \"max\" | \"sin\" ;                   \
      operator : <function> |'+' | '-' | '*' | '/' | '%' | '^' ; \
      expr     : <number> | '(' <operator> <expr>+ ')' ;         \
      lispy    : /^/ <operator> <expr>+ /$/ ;                    \
    ",
    Decimal, Int, Number, Function, Operator, Expr, Lispy
  );

  return mpc_parse(filename, input, Lispy, r);
}
