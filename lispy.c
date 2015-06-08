#include "lispy.h"
/* Helper functions */

lval lval_num(long x){
  lval v;
  v.type = LVAL_NUM;
  v.val.num = x;
  return v;
}

lval lval_err(int err){
  lval v;
  v.type = LVAL_ERR;
  v.val.err = err;
  return v;
}

lval lval_dbl(double x){
  lval v;
  v.type = LVAL_DBL;
  v.val.dbl = x;
  return v;
}

/* Print Lisp Val */
void lval_print(lval v){
  switch(v.type) {
    case LVAL_NUM: printf("%ld", v.val.num); break;
    case LVAL_DBL: printf("%f", v.val.dbl); break;
    case LVAL_ERR:
      /* Check error type and print the message */
      if (v.val.err == LERR_DIV_ZERO) {
        printf("Error: Division by zero!");
      }
      if (v.val.err == LERR_BAD_OP) {
        printf("Error: Invalid operator!");
      }
      if (v.val.err == LERR_BAD_UOP) {
        printf("Error: Invalid unary operator!");
      }
      if (v.val.err == LERR_BAD_NUM) {
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

/* I should probably add a coerce method to convert from primitive types */
/* Primitive types will probably grow into: int, double, string, atom and lists */
/* Available coercions should be:
 * int -> double
 * string -> list (list of chars)
 */

lval to_dbl(lval x){
  if(x.type == LVAL_NUM){
    return lval_dbl((double)x.val.num);
  } if (x.type == LVAL_DBL) {
    return x;
  }
  return lval_err(LERR_BAD_COER);
}

/* Those macros define an operator and a function that allows
 * int and double. Coercing ints into doubles when one of 
 * each argument is double */
#define BIN_OP(op,x,y)                                         \
  do {                                                         \
    if(x.type == LVAL_DBL || y.type == LVAL_DBL){              \
      return lval_dbl(to_dbl(x).val.dbl op to_dbl(y).val.dbl); \
    } else {                                                   \
      return lval_num(x.val.num op y.val.num);                 \
    }                                                          \
  } while(0)

#define BIN_FN(fn,x,y)                                           \
  do {                                                           \
    if(x.type == LVAL_DBL || y.type == LVAL_DBL){              \
      return lval_dbl(fn(to_dbl(x).val.dbl, to_dbl(y).val.dbl)); \
    } else {                                                     \
      return lval_num(fn(x.val.num, y.val.num));                 \
    }                                                            \
  } while(0)

lval eval_op(char* op, lval x, lval y) {
  /* Propagate Errors */
  if(x.type == LVAL_ERR) return x;
  if(y.type == LVAL_ERR) return y;

  if(strcmp(op, "^") == 0) BIN_FN(pow,x,y);
  if(strcmp(op, "+") == 0) BIN_OP(+, x, y);
  if(strcmp(op, "-") == 0) BIN_OP(-, x, y);
  if(strcmp(op, "*") == 0) BIN_OP(*, x, y);
  if(strcmp(op, "%") == 0) {
    if(x.type == LVAL_DBL || y.type == LVAL_DBL) {
      return lval_err(LERR_BAD_OP);
    }
    return lval_num(x.val.num % y.val.num);
  }
  if(strcmp(op, "/") == 0) {
    if(y.val.num == 0) return lval_err(LERR_DIV_ZERO);
    BIN_OP(/, x, y);
  }

  /* Todo: Move functions to a separate evaluation */
  if(strcmp(op, "min") == 0) BIN_FN(fmin,x,y);
  if(strcmp(op, "max") == 0) BIN_FN(fmax,x,y);

  return lval_err(LERR_BAD_OP);
}

lval eval_unary_op(char* op, lval x){
  /* Propagate Errors */
  if(x.type == LVAL_ERR) return x;

  if(strcmp(op, "-") == 0) {
    if(x.type == LVAL_NUM) return lval_num(x.val.num * -1);
    return lval_dbl(x.val.dbl * -1);
  }
  if(strcmp(op, "+") == 0) { return x; }
  
  /* This is just to show how to interface with C funcs */
  /* I'll turn to it later when adding functions to this language */
  if(strcmp(op, "sin") == 0) {
    return lval_dbl( sin( to_dbl(x).val.dbl ) );
  }
  return lval_err(LERR_BAD_UOP);
}

lval eval(mpc_ast_t* t){
  if(strstr(t->tag,"number|int")){
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    if(errno == ERANGE) return lval_err(LERR_BAD_NUM);
    return lval_num(x);
  }

  if(strstr(t->tag,"number|decimal")){
    errno = 0;
    double x = strtod(t->contents, NULL);
    if(errno == ERANGE) return lval_err(LERR_BAD_NUM);
    return lval_dbl(x);
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
