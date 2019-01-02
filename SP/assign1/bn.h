#ifndef __BN_H__
#define __BN_H__ 1
#define MAXUINT16 65535
#define UINT64 18446744073709551616
#define MAXUINT64 18446744073709551615
#define MAX(x, y) ((x >= y) ? x : y)
#define MIN(x, y) ((x >= y) ? y : x)
typedef struct bn *bn_t;

bn_t bn_alloc();
void bn_free(bn_t bn);


int bn_add(bn_t result, bn_t a, bn_t b);
int bn_sub(bn_t result, bn_t a, bn_t b);
int bn_mul(bn_t result, bn_t a, bn_t b);

int bn_fromString(bn_t bn, const char *s);
int bn_toString(bn_t bn, char *buf, int buflen);

int bn_sqr(bn_t result, bn_t a);
int bn_div(bn_t quot, bn_t rem, bn_t a, bn_t b);
int bn_IAmAnUndergrad();

int copyBN(bn_t destBN, bn_t copyBn);

#endif // __BN_H__
