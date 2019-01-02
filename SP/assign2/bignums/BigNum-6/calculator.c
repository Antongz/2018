#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>

#include "bn.h"
#include "stack.h"

int checkIfDup(char* ptr)
{
    return !strncmp(ptr,"dup",3);
}

int checkIfPop(char* ptr)
{
    return !strncmp(ptr,"pop",3);
}

int checkIfPrint(char* ptr)
{
    return !strncmp(ptr,"print",5);
}

int checkIfSwap(char* ptr)
{
    return !strncmp(ptr,"swap",4);
}

int checkIfClear(char* ptr)
{
    return !strncmp(ptr,"clear",5);
}

int checkIfDump(char* ptr)
{
    return !strncmp(ptr,"dump",4);
}

int checkIfOperator(char c)
{
    return (c=='+' || c=='-' || c=='*');
}

int checkIfNum(char* ptr, int size)
{
    for(int i = 0; i<size; i++)
    {
        if(!isdigit(ptr[i]))
        {
            return 1;
        }
    }
    return 0;
}



int parseInput()
{
    // read 1000 bytes at a time
    int lenMax = 1000;
    int currSize = lenMax;
    
    char* strInput = malloc(lenMax);

    st inStack = initStack(0);
    st poppedStack = initStack(1);

    if(strInput != NULL)
    {
        int c = EOF;
        int i = 0;

        //get user input one character at a time till EOF
        while (( c = getchar() ) != EOF)
        {
            //parser
            if(isspace(c) || c=='\n')
            {
                if(i>0)
                {
                    strInput[i] = '\0';
                    //printf("%s\n",strInput);

                    if(checkIfNum(strInput,i)==0)
                    {
                        bn_t num = bn_alloc();
                        if(num == NULL)
                        {
                            fprintf(stderr, "Error: out ofmemory\n");
                            return 1;
                        }
                        if(bn_fromString(num,strInput)!=0)
                        {
                            fprintf(stderr, "Error: could not convert string: %s to number\n",strInput);
                            return 1;
                        }
                        if(pushStack(inStack,num)!=0)
                        {
                            fprintf(stderr, "Error: could not push to stack\n");
                            return 1;
                        }

                    }
                    else if(i==1)
                    {
                        if(checkIfOperator(strInput[0])!=0)
                        {
                            bn_t a = topStack(inStack);
                            if(a == NULL)
                            {
                                fprintf(stderr, "Error: need two operands on stack\n");
                                return 1;
                            }
                            if(popStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: need two operands on stack\n");
                                return 1;
                            }
                            if(pushStack(poppedStack,a)!=0)
                            {
                                fprintf(stderr, "Error: out of memory popped stack\n");
                                return 1;
                            }
                            bn_t b = topStack(inStack);
                            if(b == NULL)
                            {
                                fprintf(stderr, "Error: need two operands on stack\n");
                                return 1;
                            }
                            if(popStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: need two operands on stack\n");
                                return 1;
                            }
                            if(pushStack(poppedStack,b)!=0)
                            {
                                fprintf(stderr, "Error: out of memory popped stack\n");
                                return 1;
                            }
                            bn_t result = bn_alloc();
                            if(result == NULL)
                            {
                                fprintf(stderr, "Error: out of memory \n");
                                return 1;
                            }
                            if(strInput[0]=='+')
                            {
                                if(bn_add(result,b,a)!=0)
                                {
                                    fprintf(stderr, "Error: could not perform add operation\n");
                                    return 1;
                                }
                            }
                            else if(strInput[0]=='-')
                            {
                                if(bn_sub(result,b,a)!=0)
                                {
                                    fprintf(stderr, "Error: could not perform subtract operation\n");
                                    return 1;
                                }
                            }
                            else if(strInput[0]=='*')
                            {
                                if(bn_mul(result,b,a)!=0)
                                {
                                    fprintf(stderr, "Error: could not perform multiply operation\n");
                                    return 1;
                                }
                            }
                            if(pushStack(inStack,result)!=0)
                            {
                                fprintf(stderr, "Error: could not push result onto stack\n");
                                return 1;
                            }
                        }
                    }
                    else if(i==3)
                    {
                        if(checkIfDup(strInput))
                        {
                            if(dupStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: could not perform duplicate operation\n");
                                return 1;
                            }
                        }
                        else if(checkIfPop(strInput))
                        {
                            bn_t temp = topStack(inStack);
                            if(temp == NULL)
                            {
                                fprintf(stderr, "Error: nothing to pop\n");
                                return 1;
                            }
                            if(popStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: nothing to pop\n");
                                return 1;
                            }
                            if(pushStack(poppedStack,temp)!=0)
                            {
                                fprintf(stderr, "Error: could not push onto popped stack\n");
                                return 1;
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Error: Invalid input\n");
                            return 1;
                        }
                    }
                    else if(i==4)
                    {
                        if(checkIfSwap(strInput))
                        {
                            if(swapStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: could not perform swap operation\n");
                                return 1;
                            }
                        }
                        else if(checkIfDump(strInput))
                        {
                            if(dumpStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: could not perform dump operation\n");
                                return 1;
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Error: Invalid input\n");
                            return 1;
                        }
                    }
                    else if(i==5)
                    {
                        if(checkIfClear(strInput))
                        {
                            if(clearStack(inStack)!=0 && clearStack(poppedStack)!=0)
                            {
                                fprintf(stderr, "Error: could not perform clear operation\n");
                                return 1;
                            }
                        }
                        else if(checkIfPrint(strInput))
                        {
                            if(printStack(inStack)!=0)
                            {
                                fprintf(stderr, "Error: could not perform print operation\n");
                                return 1;
                            } 
                        }
                        else 
                        {
                            fprintf(stderr, "Error: Invalid input\n");
                            return 1;
                        }
                    }
                    else{
                        fprintf(stderr, "%s%s\n",strInput,"Error: Invalid input");
                        return 1;
                    }

                }
                i = 0;
                continue;
            }

            strInput[i] = (char)c;

            i++;

            //if i reached maximize size then realloc size
            if(i == currSize)
            {
                currSize = i+lenMax;
                if(currSize>=INT_MAX-2000){
                    fprintf(stderr, "%s%s\n",strInput,"Error: Maximum input string length reached");
                    return 1;
                }
                strInput = (char* )realloc(strInput, currSize);
                if(strInput == NULL)
                {
                    fprintf(stderr, "%s%s\n",strInput,"Error: Out of memory for input string storage");
                    return 1;
                }
            }

        }
        if(i>0)
        {
            strInput[i] = '\0';
            //printf("%s\n",strInput);
        }

        //printf("%s\n", strInput);
        free(strInput);
        strInput = NULL;


    }
    else
    {
        return 1;
    }
    if(clearStack(inStack)!=0)
    {
        // could not clear active stack
        return 1;
    }
    if(clearStack(poppedStack)!=0)
    {
        // could not clear popped stack
        return 1;
    }
    freeStack(inStack);
    freeStack(poppedStack);
    return 0;

}

int main()
{   
    parseInput();
    
    return 0;
}
