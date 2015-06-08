#include "lang.h"
/* Lispy Value */

/** Value types **/
enum { LVAL_NUM, LVAL_DBL, LVAL_ERR };

/** Error types **/
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_UOP, LERR_BAD_NUM, LERR_BAD_COER };

typedef struct {
  int type;
  union {
    double dbl;
    long num;
    int err;
  } val;
} lval;

/* Helper functions */
lval lval_num(long x);
lval lval_err(int err);

/* Print Lisp Val */
void lval_print(lval v);
void lval_println(lval v);

/* Evaluation */

lval eval_op(char* op, lval x, lval y);
lval eval_unary_op(char* op, lval x);
lval eval(mpc_ast_t* t);
