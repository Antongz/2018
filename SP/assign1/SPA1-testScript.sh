#!/bin/bash

usage() {
  echo "Usage: $1 <archive> <tests>"
  exit 1
}

case $# in 
  2);;
  *) usage;;
esac

FILE="$1"
TESTDIR="$2"

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


test -r "$TESTDIR" || runerror "$TESTDIR: not readable"
TESTDIR=`realpath "$TESTDIR"` || runerror "$TESTDIR: Realpath failed"


RUNDIR=`mktemp -d  SPA1-tst.XXXXX` || runerror "Cannot create test run directory"
echo "Created test directory `setdisp blue`$RUNDIR`setdisp reset`"
RUNDIR=`realpath "$RUNDIR"` || runerror "Cannot get real path of test run directory"



tar xf "$FILE" -C "$RUNDIR" || error "$FILE: Cannot extract"

cd "$RUNDIR"
USER=`echo *`
case "$USER" in
  a[0-9][0-9][0-9][0-9][0-9][0-9][0-9]);;
  *) error "$FILE: Contents does not seem to be a student number"
esac

if [ -f "$TESTDIR" ]; then
  tar xf "$TESTDIR" -C "$RUNDIR" || runerror "$TESTDIR: Cannot extract"
  test -d "$RUNDIR/tests" || runerror "$TESTDIR: Bad file"
  TESTDIR="$RUNDIR/tests"
fi

test -d "$TESTDIR" || runerror "$TESTDIR: Not a directory"
test -d "$TESTDIR/calc" || runerror "$TESTDIR: Bad structure"
test -d "$TESTDIR/libbn" || runerror "$TESTDIR: Bad structure"


test -h "$USER" && error "$USER: is a symlink"
test -d "$USER" || error "$USER: is not a directory"
chmod -R u+rw "$USER" || error "$USER: could not chmod???"

echo "Found user `setdisp blue`$USER`setdisp reset`"

cd $USER
hasFile "*.o" && warning "$FILE: Includes .o file"
hasFile "calc" && error "$FILE: Includes calc binary"
hasFile "libbn.a" && error "$FILE: Includes calc binary"


hasFile "[mM]akefile" || warning "Could not find a Makefile"
grep -q "[-]Wall" [mM]akefile || warning "Makefile does not seem to use -Wall"

make || error "Make failed"

hasFile "calc" || error "Make did not build calc"
hasFile "libbn.a" || error "Make did not build libbn.a"

cd "$RUNDIR"


FAIL=0
TESTS=0

calctest() {
  TESTS=$((TESTS + 1))
  testname=`basename "$1" .tst`
  expected=`dirname "$1"`/"$testname".out
  "$USER/calc" < "$1" > "$testname.out" 2>"$testname.err"
  if diff -q "$testname.out" "$expected"; then
    echo "Test $testname `setdisp green`Passed`setdisp reset`"
  else
    echo "Test $testname `setdisp lred`Failed`setdisp reset`"
    FAIL=$((FAIL + 1))
  fi
}

libbntest() {
  TESTS=$((TESTS + 1))
  testname=`basename "$1" .c`
  expected=`dirname "$1"`/"$testname".out
  gcc --std=gnu99 -I"$USER" -L"$USER" "$1" -lbn -o "$testname"
  ./"$testname" > "$testname.out" 2>"$testname.err"
  if diff -q "$testname.out" "$expected" ; then
    echo "Test $testname `setdisp green`Passed`setdisp reset`"
  else
    echo "Test $testname `setdisp lred`Failed`setdisp reset`"
    FAIL=$((FAIL + 1))
  fi
}

for f in $TESTDIR/calc/*.tst; do
  calctest "$f"
done

for f in $TESTDIR/libbn/*.c; do
  libbntest "$f"
done

echo "Passed $((TESTS - FAIL)) out of $TESTS tests"
