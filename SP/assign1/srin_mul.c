int bn_mul(bn_t result, bn_t a, bn_t b){
  //allocating place
  char buf1[1000];
  char buf2[1000];
  //two bn nums used during the multiplication
  bn_t temp_res = bn_alloc();
  bn_t res = bn_alloc();
  uint16_t len = 0;
  //bn resizing 
  if (bn_resize(temp_res,a->bn_len+1)==-1){
    return -1;
  }
  temp_res->bn_len = temp_res->bn_size;
  //bn resizing 
  if (bn_resize(temp_res,1000)==-1){
    return -1;
  }
  temp_res->bn_len = temp_res->bn_size;
  //current_carry
  uint32_t temp_mul = 0;
  uint16_t current_carry = 0;
  //iterators for loops
  int i, k, f, g;
  //tracking how much shifting 
  int shifting_amount = 0;
  //double for loop to do multiplication
  //loop through b
  for(i=0; i<b->bn_len; i++){
    printf("i: %d\n", i);
    //looping through a 
    for(k=0; k<a->bn_len; k++){
      printf("k: %d\n", k);
      printf("a->bn_data[%d]: %d\n",k, a->bn_data[k]);
      printf("b->bn_data[%d]: %d\n",i, b->bn_data[i]);
      temp_mul = ((a->bn_data[k]*b->bn_data[i])+current_carry);
      temp_res->bn_data[k] = temp_mul%65536;
      temp_mul = ((a->bn_data[k]*b->bn_data[i])+current_carry);
      current_carry = temp_mul/65536;  
    }
    //removing leading zero's by updating bn_len and bn_size
    temp_res->bn_len = bn_reallen(temp_res);
    temp_res->bn_size = temp_res->bn_len;
    //shifting to the right only if i>0
    if(i>0){
      printf("SHIFTING\n");
      //len
      len = temp_res->bn_len+i;
      //create another new uint16_t array
      uint16_t *new_array;
      new_array = (uint16_t*)calloc(len, sizeof(uint16_t));
      //checking if an error occurred during calloc
      if(res==NULL){
        return -1;
      }
      //adding the 0's
      for(g=0; g<i; g++){
        shifting_amount++;
        printf("ADDING a 0\n");
        new_array[g] = 0;
      }
      //adding the content located in temp_res
      for(f=i; f<len; f++){
        new_array[f] = temp_res->bn_data[f-i];
      }
      //printing out new_array contents
      for(f=0; f<len; f++){
        printf("new_array[%d] = %d\n",f,new_array[f]);
      }
      // //bn resizing 
      if (bn_resize(temp_res, len)==-1){
        return -1;
      }
      temp_res->bn_len = temp_res->bn_size;
      //swapping the new_array with the temp_res->bn_data array
      memcpy(temp_res->bn_data, new_array, (len)*sizeof(uint16_t));
    }
    //checking if current_carry > 0 if so add the current_carry to temp_res
    if(current_carry>0){
      if (bn_resize(temp_res, k+1+shifting_amount)==-1){
        return -1;
      }
      temp_res->bn_len = temp_res->bn_size;
      temp_res->bn_data[k+shifting_amount] = current_carry;
      current_carry = 0;
    }
    //add this temp
    bn_add(res, res, temp_res);
    // len = temp_res->bn_len;
    // temp_res->bn_data = (uint16_t *)calloc(1, (len)*sizeof(uint16_t));
    memset(temp_res->bn_data, 0, (temp_res->bn_len)*sizeof(uint16_t));
    //reset shifting_amount
    shifting_amount = 0;
  }
  //place res into result->bn_data
  //result resizing data
  if (bn_resize(result, res->bn_len)==-1){
    return -1;
  }
  result->bn_len = result->bn_size;
  memcpy(result->bn_data, res->bn_data, res->bn_len*sizeof(uint16_t));
  //update result->bn_len (the number digits)
  result->bn_len = bn_reallen(result);
  result->bn_size = result->bn_len;
  //memset of res
  memset(res, 0, sizeof(uint16_t));
  return 0;
}