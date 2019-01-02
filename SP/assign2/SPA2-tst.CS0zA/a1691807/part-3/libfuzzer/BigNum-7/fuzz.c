#include "calc.c"

int LLVMFuzzerTestOneInput(const uint8_t *data, uint64_t size) {
    // Copy the data so we can mutate it
    char* d = malloc(size + 1);
    memcpy(d, data, size);
    d[size] = '\0';
//    bn_stack* stack = create_stack();
    int sp = 0;
    char c, testNum;

    bn_t stack[100];

    // If a valid number add it to the stack.
    if((strcmp(d,"+") == 0) || (strcmp(d,"-") == 0) || (strcmp(d,"*") == 0)){
        action(stack, &sp, d[0]);
    } else if(strcmp("dup",d) == 0){
        dup(stack,&sp);
    } else if(strcmp("pop",d) == 0){
        pop(stack,&sp);
    } else if(strcmp("print",d) == 0){
        print(stack,&sp);
    } else if(strcmp("swap",d) == 0){
        swap(stack,&sp);
    } else if(strcmp("dump",d) == 0){
        dump(stack,sp);
    } else if(strcmp("clear",d) == 0){
        clear(stack,&sp);
    } else {
        int testNumCount = 0;
        testNum = d[testNumCount];
        while(testNum){
            if(isdigit(testNum)){
                testNumCount++;
                testNum = d[testNumCount];
            } else {
                printf("Error, Enter a correct command or number, errno = %d\n", errno);
                exit(1);
            }
        }
        decimalNumber(stack,&sp,d);
    }



    free(d);
    return 0;
}
