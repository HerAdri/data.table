#include "data.table.h"

static void substitute_call_arg_names(SEXP expr, SEXP env) {
  R_len_t len = length(expr);
  if (len && isLanguage(expr)) { // isLanguage is R's is.call
    SEXP arg_names = getAttrib(expr, R_NamesSymbol);
    if (!isNull(arg_names)) {
      SEXP env_names = getAttrib(env, R_NamesSymbol);
      int *imatches = INTEGER(PROTECT(chmatch(arg_names, env_names, 0)));
      //const SEXP *expr_arg_names = SEXPPTR_RO(arg_names); // debug
      const SEXP *env_sub = SEXPPTR_RO(env);
      int i = 0;
      for (SEXP tmp=expr; tmp!=R_NilValue; tmp=CDR(tmp)) {
        if (imatches[i]) {
          SEXP sym = env_sub[imatches[i]-1];
          if (!isSymbol(sym))
            error("Attempting to substitute '%s' element with object of type '%s' but it has to be symbol type when substituting name of the call argument, functions 'as.name' and 'I' can be used to work out proper substitution, see ?substitute2", CHAR(STRING_ELT(arg_names, i)), type2char(TYPEOF(sym)));
          //Rprintf("substitute names: %s -> %s\n", CHAR(expr_arg_names[i]), CHAR(PRINTNAME(sym))); // debug
          SET_TAG(tmp, sym);
        }
        i++;
        substitute_call_arg_names(CADR(tmp), env); // substitute arg names in child calls
      }
      UNPROTECT(1); // chmatch
    }
  }
}
SEXP substitute_call_arg_namesR(SEXP expr, SEXP env) {
  SEXP ans = PROTECT(MAYBE_REFERENCED(expr) ? duplicate(expr) : expr);
  //Rprintf("entering substitute_call_arg_names\n");
  substitute_call_arg_names(ans, env); // updates in-place
  UNPROTECT(1);
  return ans;
}
