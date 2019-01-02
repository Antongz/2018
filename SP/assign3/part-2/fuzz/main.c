#include "calc.c"

int main() {
    char buffer[2048];

    while(fgets(buffer, 2048, stdin) != NULL){
        if (parse(buffer) != 0) {
            status = -1;
            break;
        }
    }

    clear(&root);
    clearYBN(&ybnRoot);

    if (status == -1) { fprintf(stderr, "Error\n");  return -1; }
    return 0;
}
