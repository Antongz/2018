/*
 * =====================================================================================
 *
 *       Filename:  FR-victim.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/22/2018 09:23:27 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yi-Ting, Hsieh (a1691807), a1691807@student.adelaide.edu.au
 *   Organization:  
 *
 * =====================================================================================
 */
#incldue <stdio.h>

int x() {
    int v = 0;
    for (int i=0; i<1000000; i++) {
        v += i;
    }
    return v;
}

int y(){
    int v = 0;
    for (int i=0; i<1000000; i++) {
        v += i;
    }
    return v;
}


int main() {

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
