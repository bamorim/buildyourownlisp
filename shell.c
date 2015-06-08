#include "mpc/mpc.h"

#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <histedit.h>


long eval_op(char* op, long x, long y) {
  if(strcmp(op, "%") == 0) { return x % y;    }
  if(strcmp(op, "^") == 0) { return pow(x,y); }
  if(strcmp(op, "+") == 0) { return x + y;    }
  if(strcmp(op, "-") == 0) { return x - y;    }
  if(strcmp(op, "*") == 0) { return x * y;    }
  if(strcmp(op, "/") == 0) { return x / y;    }
  /* Todo: Move functions to a separate evaluation */
  if(strcmp(op, "min") == 0) {
    if(x > y) { return y; }
    return x;
  }
  if(strcmp(op, "max") == 0) {
    if(x > y) { return x; }
    return y;
  }
  return 0;
}

long eval_unary_op(char* op, long x){
  if(strcmp(op, "-") == 0) { return x * -1; }
  if(strcmp(op, "+") == 0) { return x; }
  return 0;
}

long eval(mpc_ast_t* t){
  if(strstr(t->tag,"number")){
    return atoi(t->contents);
  }

  /* Operator is always the second (in expr and in lispy) */
  char* op = t->children[1]->contents;

  /* Evaluate and store the second children */
  long x = eval(t->children[2]);

  int i = 3;

  while(strstr(t->children[i]->tag,"expr")){
    x = eval_op(op, x, eval(t->children[i]));
    i++;
  }

  if( i == 3){
    x = eval_unary_op(op, x);
  }
  
  return x;
};

/* Parsing Stuff */
int main(int argc, char** argv){

  /* Define Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Function = mpc_new("function");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Lispy    = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                            \
      number   : /-?[0-9]+/ ;                                    \
      function : \"min\" | \"max\" ;                             \
      operator : <function> |'+' | '-' | '*' | '/' | '%' | '^' ; \
      expr     : <number> | '(' <operator> <expr>+ ')' ;         \
      lispy    : /^/ <operator> <expr>+ /$/ ;                    \
    ",
    Number, Function, Operator, Expr, Lispy
  );

  puts("Lispy Version 0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while(1) {
    /* Prompt and readline */
    char* input = readline("lispy> ");

    if(!input) {
      printf("\n");
      continue;
    }
    
    if(input[0]){
      add_history(input);
    }

    /* Try to parse stuff and print out the AST. Otherwise print the error */
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Lispy, &r)){
      printf("%ld\n",eval(r.output));
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  return 0;
}
