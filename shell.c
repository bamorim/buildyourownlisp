#include "mpc/mpc.h"

#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <histedit.h>

/* Lispy Value */

/* Value types */
enum { LVAL_NUM, LVAL_ERR };

/* Error types */
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_UOP, LERR_BAD_NUM };
typedef struct {
  int type;
  long num;
  int err;
} lval;

/* Helper functions */

lval lval_num(long x){
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int err){
  lval v;
  v.type = LVAL_ERR;
  v.err = err;
  return v;
}

/* Print Lisp Val */
void lval_print(lval v){
  switch(v.type) {
    case LVAL_NUM: printf("%ld", v.num); break;
    case LVAL_ERR:
      /* Check error type and print the message */
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division by zero!");
      }
      if (v.err == LERR_BAD_OP) {
        printf("Error: Invalid operator!");
      }
      if (v.err == LERR_BAD_UOP) {
        printf("Error: Invalid unary operator!");
      }
      if (v.err == LERR_BAD_NUM) {
        printf("Error: Invalid Number!");
      }
    break;
  }
}

void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}

/* Evaluation */

lval eval_op(char* op, lval x, lval y) {
  /* Propagate Errors */
  if(x.type == LVAL_ERR) return x;
  if(y.type == LVAL_ERR) return y;

  if(strcmp(op, "%") == 0) { return lval_num(x.num % y.num);    }
  if(strcmp(op, "^") == 0) { return lval_num(pow(x.num,y.num)); }
  if(strcmp(op, "+") == 0) { return lval_num(x.num + y.num);    }
  if(strcmp(op, "-") == 0) { return lval_num(x.num - y.num);    }
  if(strcmp(op, "*") == 0) { return lval_num(x.num * y.num);    }
  if(strcmp(op, "/") == 0) {
    if(y.num == 0) return lval_err(LERR_DIV_ZERO);
    return lval_num(x.num / y.num);
  }

  /* Todo: Move functions to a separate evaluation */
  if(strcmp(op, "min") == 0) {
    if(x.num > y.num) { return y; }
    return x;
  }
  if(strcmp(op, "max") == 0) {
    if(x.num > y.num) { return x; }
    return y;
  }
  return lval_err(LERR_BAD_OP);
}

lval eval_unary_op(char* op, lval x){
  /* Propagate Errors */
  if(x.type == LVAL_ERR) return x;

  if(strcmp(op, "-") == 0) { return lval_num(x.num * -1); }
  if(strcmp(op, "+") == 0) { return x; }
  return lval_err(LERR_BAD_UOP);
}

lval eval(mpc_ast_t* t){
  if(strstr(t->tag,"number")){
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    if(errno == ERANGE) return lval_err(LERR_BAD_NUM);
    return lval_num(x);
  }

  /* Operator is always the second (in expr and in lispy) */
  char* op = t->children[1]->contents;

  /* Evaluate and store the second children */
  lval x = eval(t->children[2]);

  int i = 3;

  while(strstr(t->children[i]->tag,"expr")){
    lval y = eval(t->children[i]);
    x = eval_op(op, x, y);
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
      lval_println(eval(r.output));
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  return 0;
}
