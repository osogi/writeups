#include <stdio.h>
unsigned long murmur_hash_64_inverse(unsigned long h, unsigned long seed)
{
	const unsigned long m = 0xc6a4a7935bd1e995ULL;
	const unsigned long minv = 0x5f7a0ea7e59b19bdULL; 
	const int r = 47;

	h ^= h >> r;
	h *= minv;
	h ^= h >> r;
	h *= minv;

	unsigned long hforward = seed ^ (8 * m);
	unsigned long k = h ^ hforward;

	k *= minv;
	k ^= k >> r;
	k *= minv;

		char *p = (char *)&k;
		char c;
		c = p[0]; p[0] = p[7]; p[7] = c;
		c = p[1]; p[1] = p[6]; p[6] = c;
		c = p[2]; p[2] = p[5]; p[5] = c;
		c = p[3]; p[3] = p[4]; p[4] = c;

	
	return k;
}

int main(int argc, char *argv[])
{
    char* buf=(char*)malloc(8);
    memcpy(buf, argv[1], 8);
    printf("Input: %s\n", buf);
    unsigned long s=*((unsigned long*)buf);
    //printf("%lx\n", s);
    unsigned long seed=64400;
    unsigned long res=murmur_hash_64_inverse(s, seed);
    printf("%lx\n", res);

    return 0;
}