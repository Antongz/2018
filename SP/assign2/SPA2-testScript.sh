#!/bin/bash

DEFTIMEOUT=30s

usage() {
  echo "Usage: $1 <archive> <tests>"
  exit 1
}

case $# in 
  1);;
  *) usage;;
esac

FILE="$1"

shopt -q nullglob

setdisp() {
  for f; do
    case "$f" in 
      "reset")                  param=0;;
      "bold")                   param="$param;1";;
      "nobold")                 param="$param;21";;
      "underscore")             param="$param;4";;
      "blink")                  param="$param;5";;
      "fg-black"|"black")       param="$param;30";;
      "fg-red"|"red")           param="$param;31";;
      "fg-green"|"green")       param="$param;32";;
      "fg-yellow"|"yellow")     param="$param;33";;
      "fg-blue"|"blue")         param="$param;34";;
      "fg-magenta"|"magenta")   param="$param;35";;
      "fg-cyan"|"cyan")         param="$param;36";;
      "fg-gray"|"gray")         param="$param;37";;
      "fg-dark"|"dark")         param="$param;90";;
      "fg-lred"|"lred")         param="$param;91";;
      "fg-lgreen"|"lgreen")     param="$param;92";;
      "fg-lyellow"|"lyellow")   param="$param;93";;
      "fg-lblue"|"lblue")       param="$param;94";;
      "fg-lmagenta"|"lmagenta") param="$param;95";;
      "fg-lcyan"|"lcyan")       param="$param;96";;
      "fg-white"|"white")       param="$param;97";;
      "fg-default")             param="$param;39";;
      "bg-black")               param="$param;40";;
      "bg-red")                 param="$param;41";;
      "bg-green")               param="$param;42";;
      "bg-yellow")              param="$param;43";;
      "bg-blue")                param="$param;44";;
      "bg-magenta")             param="$param;45";;
      "bg-cyan")                param="$param;46";;
      "bg-gray")                param="$param;47";;
      "bg-default")             param="$param;49";;
      *) return;;
    esac
  done
  echo -ne "\e[${param:1}m"
}

message() {
  color="$1"
  message="$2"
  setdisp $color
  echo "$message"
  setdisp reset
}

error() {
  if [ -d "$RUNDIR" ]; then 
    echo "* $*" >> $RUNDIR/Results
  fi
  message lred "***ERROR*** $*"
  exit 2
}

runerror() {
  message lred "***RUN ERROR*** $*"
  exit 3
}

warning() {
  message red "***WARNING*** $*"
}

hasFile() {
  match=`echo $1`
  test -e "$match"
}


trap "setdisp reset" 0 

# Test file name
test -f "$FILE" || runerror "$FILE: does not exist"
test -r "$FILE" || runerror "$FILE: is not readable"

case "$FILE" in
  *.tar|*.tgz);;
  *) error "File $FILE is neither .tar nor .tgz";;
esac

if which unzip >& /dev/null; then
  unzip -tq "$FILE" >& /dev/null && error "File $FILE seems to be a zip file"
else
  warning "Cannot test that $FILE is not a zip file."
fi



RUNDIR=`mktemp -d  SPA2-tst.XXXXX` || runerror "Cannot create test run directory"
echo "Created test directory `setdisp blue`$RUNDIR`setdisp reset`"
RUNDIR=`realpath "$RUNDIR"` || runerror "Cannot get real path of test run directory"
echo "> Starting"  >> $RUNDIR/Results

tar xf "$FILE" -C "$RUNDIR" || error "$FILE: Cannot extract"

cd "$RUNDIR"
rm Results
USER=`echo *`
echo "> Starting"  >> $RUNDIR/Results
case "$USER" in
  a[0-9][0-9][0-9][0-9][0-9][0-9][0-9]);;
  *) error "$FILE: Contents does not seem to be a student number"
esac



test -h "$USER" && error "$USER: is a symlink"
test -d "$USER" || error "$USER: is not a directory"
chmod -R u+rw "$USER" || error "$USER: could not chmod???"

echo "Found user `setdisp blue`$USER`setdisp reset`"

cd $USER
hasFile "answers.pdf" || error "Cannot find answers.pdf"

test -d "part-4" || error "No directory for part-4"

cd part-4
hasFile "*.o" && warning "$FILE: Includes .o file"
hasFile "calc" && error "$FILE: Includes calc binary"
hasFile "libbn.a" && error "$FILE: Includes libbn.a binary"


hasFile "[mM]akefile" || warning "Could not find a Makefile"
grep -q "[-]Wall" [mM]akefile || warning "Makefile does not seem to use -Wall"

make || error "Make failed"

hasFile "libbn.a" || error "Make did not build libbn.a"

cd "$RUNDIR"


FAIL=0
TESTS=0

sed 's/^X//' > test.c <<EOFTEST
X#include <stdio.h>
X#include <stdlib.h>
X#include <bn.h>
X
X// Sadly, I forgot to ask to include this in bn.h.
Xint bn_modexp(bn_t result, bn_t base, bn_t exp, bn_t modulus);
X
Xchar *toString(bn_t bn) {
X  static char *printbuf = NULL;
X  static int len = 0;
X
X  int reqlen  = bn_toString(bn, printbuf, len);
X  if (reqlen > 0) {
X    printbuf = realloc(printbuf, reqlen * 2);
X    if (printbuf == NULL) {
X      fprintf(stderr, "Malloc error\n");
X      exit(1);
X    }
X    len = reqlen * 2;
X    reqlen = bn_toString(bn, printbuf, len);
X    if (reqlen != 0) {
X      fprintf(stderr, "bn_toString failed\n");
X      exit(1);
X    }
X  }
X  return printbuf;
X}
X
Xvoid calc(char *b, char *e, char *m) {
X  bn_t base = bn_alloc();
X  bn_t exp = bn_alloc();
X  bn_t mod = bn_alloc();
X  bn_t res = bn_alloc();
X
X  if (base == NULL || exp == NULL || mod == NULL || res == NULL) {
X    fprintf(stderr, "Allocation failed\n");
X    exit(1);
X  }
X
X  bn_fromString(base, b);
X  bn_fromString(exp, e);
X  bn_fromString(mod, m);
X  if (bn_modexp(res, base, exp, mod) != 0) {
X    fprintf(stderr, "Exponentiation error\n");
X    exit(1);
X  }
X  printf("%s\n", toString(res));
X
X  bn_free(base);
X  bn_free(mod);
X  bn_free(exp);
X  bn_free(res);
X}
X
Xint main(int ac, char **av) {
X  calc("1", "1", "10");
X  calc("2", "2", "10");
X  calc("12", "16", "17");
X  exit(0);
X}
EOFTEST

gcc --std=gnu99 -I"$USER/part-4" -L"$USER/part-4" "test.c" -lbn -lm -o "test" || error "Cannot build test program"

./test > test.out 
diff test.out - <<EOF
1
4
1
EOF
test "$?" = "0" || error "Results do not match"

message green "Test Completed"


