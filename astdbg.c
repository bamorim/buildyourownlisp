#include "lang.h"
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <histedit.h>

int main(int argc, char** argv){
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
    if(lispy_parse("<stdin>", input, &r)){
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  return 0;
}
