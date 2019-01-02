void kernel even_step(global int* A, global const int* len) {
    int id = get_global_id(0);
    // First, we check whether this device id is in the array boundary,
    // if it is, then we perform even-step bubble sort.
    if(id<=(*len/2-1)){
        // Swap the elements if they are not in right order.
        if(A[2*id] > A[2*id+1]){
            int temp = A[2*id];
            A[2*id] = A[2*id+1];
            A[2*id+1] = temp;
        }
    }
}


void kernel odd_step(global int* A, global const int* len) {
    int id = get_global_id(0);
    // First, we check whether this device id is in the array boundary,
    // if it is, then we perform odd-step bubble sort.
    if(id<=(*len/2-2)){
        // Swap the elements if they are not in right order.
        if(A[2*id+1] > A[2*id+2]){
            int temp = A[2*id+1];
            A[2*id+1] = A[2*id+2];
            A[2*id+2] = temp;
        }
    }
}
