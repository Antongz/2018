int x() {
    int v = 0;
    for (int i=0; i<100000; i++) {
        v += i;
    }
    return v;
}

int y(){
    int v = 0;
    for (int i=0; i<100000; i++) {
        v += i;
    }
    return v;
}


int main(int ac, char **argv) {

    while(1) {
        x();
        y();
        
        x();
        x();
        x();

        y();
        y();
        y();
    }

    return 0;
}
