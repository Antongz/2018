#include "calc.c"

int main() {
    bufferLength = buffer_size;

    char buffer[2048];

    while(fgets(buffer, 2048, stdin) != NULL){
        if (parse(buffer) != 0) {
            status = -1;
            break;
        }
    }

    if (status == -1) { fprintf(stderr, "Error\n"); clear(&root); return -1; }
    return 0;
}
