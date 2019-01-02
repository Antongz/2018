#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bn.h"
#include "stack.h"


struct stackElement
{
    bn_t data;
    stEl next;
};

struct stackStore
{
    stEl head;
    int mode;
};

st initStack(int mode)
{
    st newStack = (st)malloc(sizeof newStack);
    if(newStack == NULL)
    {
        return NULL;
    }
    newStack->head = NULL;
    return newStack;
}

int pushStack(st myStack, bn_t i)
{
    if(myStack == NULL)
    {
        return 1;
    }
    if(i == NULL)
    {
        return 1;
    }
    stEl element = (stEl)malloc(sizeof element);
    if(element == NULL)
    {
        return 1;
    }
    else
    {
        element->data = i;
        element->next = myStack->head;
        myStack->head = element;

    }
    return 0;
}

bn_t topStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return NULL;
    }
    if(myStack->head!=NULL)
    {
        return myStack->head->data;
    }
    else
    {
        return NULL;
    }
}

int popStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return 1;
    }
    if(myStack->head != NULL)
    {
        stEl temp = myStack->head;
        myStack->head = myStack->head->next;
        if(temp->data != NULL && myStack->mode==1)
        {
            bn_free(temp->data);
        }
        free(temp);
    }
    else
    {
        fprintf(stderr, "Error: nothing to pop\n");
        return 1;
    }
    return 0;
}

int dupStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return 1;
    }
    if(pushStack(myStack,topStack(myStack))!=0)
    {
        fprintf(stderr, "Error: nothing on stack to duplicate\n");
        return 1;  
    }
    return 0;
}

int printStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return 1;
    }
    if(myStack->head != NULL)
    {
        if(myStack->head->data != NULL)
        {
            char* buf = (char*)malloc(100);

            if(buf == NULL)
            {
                fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                return 1;
            }
            int i = bn_toString(myStack->head->data,buf,1000);
            if(i>0)
            {
                buf = (char*)realloc(buf,i);

                if(buf == NULL)
                {
                    fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                    return 1;
                }
                if(bn_toString(myStack->head->data,buf,i)!=0)
                {
                    fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                    return 1;
                }
            }
            if(i==-1)
            {
                fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                return 1;
            }
            printf("%s\n",buf);

            free(buf);
        }
    }
    return 0;
}

int swapStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return 1;
    }

    stEl temp = myStack->head;
    if(temp == NULL)
    {
        fprintf(stderr, "Error: nothing to swap\n");
        return 1;
    }
    bn_t tempData = temp->data;

    if(temp->next == NULL)
    {
        fprintf(stderr, "Error: need two elements to swap\n");
        return 1;
    }

    stEl temp2 = temp->next;

    temp->data = temp2->data;

    temp2->data = tempData;

    return 0;
}

int dumpStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return 1;
    }
    if(myStack->head != NULL)
    {
        stEl temp = myStack->head;
        do
        {
            char* buf = (char*)malloc(100);

            if(buf == NULL)
            {
                fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                return 1;
            }
            int i = bn_toString(temp->data,buf,1000);
            if(i>0)
            {
                buf = (char*)realloc(buf,i);

                if(buf == NULL)
                {
                    fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                    return 1;
                }
                if(bn_toString(temp->data,buf,i)!=0)
                {
                    fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                    return 1;
                }
            }
            if(i==-1)
            {
                fprintf(stderr, "Error: could not allocate memory for print buffer\n");
                return 1;
            }
            printf("%s\n",buf);
            free(buf);
            temp = temp->next;
        } while (temp != NULL);
    }
    return 0;
}

int clearStack(st myStack)
{
    if(myStack == NULL)
    {
        fprintf(stderr, "Error: stack not initialized\n");
        return 1;
    }
    while(myStack->head != NULL)
    {
        popStack(myStack);
    }
    return 0;
}

void freeStack(st myStack)
{
    free(myStack);
}
