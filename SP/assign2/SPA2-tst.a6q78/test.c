#include <stdio.h>
#include <stdlib.h>
#include <bn.h>

// Sadly, I forgot to ask to include this in bn.h.
int bn_modexp(bn_t result, bn_t base, bn_t exp, bn_t modulus);

char *toString(bn_t bn) {
  static char *printbuf = NULL;
  static int len = 0;

  int reqlen  = bn_toString(bn, printbuf, len);
  if (reqlen > 0) {
    printbuf = realloc(printbuf, reqlen * 2);
    if (printbuf == NULL) {
      fprintf(stderr, "Malloc error\n");
      exit(1);
    }
    len = reqlen * 2;
    reqlen = bn_toString(bn, printbuf, len);
    if (reqlen != 0) {
      fprintf(stderr, "bn_toString failed\n");
      exit(1);
    }
  }
  return printbuf;
}

void calc(char *b, char *e, char *m) {
  bn_t base = bn_alloc();
  bn_t exp = bn_alloc();
  bn_t mod = bn_alloc();
  bn_t res = bn_alloc();

  if (base == NULL || exp == NULL || mod == NULL || res == NULL) {
    fprintf(stderr, "Allocation failed\n");
    exit(1);
  }

  bn_fromString(base, b);
  bn_fromString(exp, e);
  bn_fromString(mod, m);
  if (bn_modexp(res, base, exp, mod) != 0) {
    fprintf(stderr, "Exponentiation error\n");
    exit(1);
  }
  printf("%s\n", toString(res));

  bn_free(base);
  bn_free(mod);
  bn_free(exp);
  bn_free(res);
}

int main(int ac, char **av) {
  calc("1", "1", "10");
  calc("2", "2", "10");
  calc("12", "16", "17");
  exit(0);
}
