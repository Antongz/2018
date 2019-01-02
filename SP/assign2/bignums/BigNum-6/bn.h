
#ifndef __BN_H__
#define __BN_H__ 1
 
typedef struct bn* bn_t;


// Allocates a new bignum and initialises it to (positive) 0. 
// Returns NULL if not enough resources are available.
bn_t bn_alloc();

// Frees all the resources associated with the bignum bn.
void bn_free(bn_t bn);

// Adds the bignums a and b and places the sum in result. 
// Returns 0 if completed successfully and −1 otherwise.
int bn_add(bn_t result, bn_t a, bn_t b);

// Subtracts the bignum b from a and places the difference in result. 
// Returns 0 if completed successfully and −1 otherwise.
// Undergraduate students: if a is smaller than b, the result is set to 0.
int bn_sub(bn_t result, bn_t a, bn_t b);

// Multiplies the bignums a and b and places the product in result. 
// Returns 0 if completed successfully and −1 otherwise.
int bn_mul(bn_t result, bn_t a, bn_t b);

// Read the decimal number in the string str and stores its value in bn. 
// Returns 0 if completed successfully and −1 otherwise.
int bn_fromString(bn_t bn, const char *s);

// Converts a bignum bn to a string. including the terminating NUL character. 
// If buflen is large enough to store the converted string, the function stores the string in buf and returns 0. 
// Otherwise, buf is not changed and the return value is the number of characters required to store the string representation.
// The function returns a negative number in case of error.
int bn_toString(bn_t bn, char *buf, int buflen);

int bn_sqr(bn_t result, bn_t a);
int bn_div(bn_t quot, bn_t rem, bn_t a, bn_t b);

// Returns 1 if you’re an undergraduate student
int bn_IAmAnUndergrad();

#endif // __BN_H__
