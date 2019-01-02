#include "src.c"

int main(){
    char buffer[2048];
    calc* c = calc_alloc();
    while(fgets(buffer, 2048, stdin) != NULL){
        parse(c, buffer);
    }
}
