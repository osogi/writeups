#include <stdio.h>

typedef unsigned char uchar;

int main() {
    uchar pre_a = 0x89;
    uchar a1[4];

    for (uchar i = pre_a + 1; i != 0; i++) {
        a1[0] = i;
        for (uchar ii = a1[0]; ii != 0; ii++) {
            a1[1] = ii;
            for (uchar iii = a1[1]; iii != 0; iii++) {
                a1[2] = iii;
                for (uchar iiii = a1[2]; iiii != 0; iiii++) {
                    a1[3] = iiii;

                    if (((a1[0] ^ a1[3]) == 0x30) && ((a1[1] ^ a1[3]) == 0x18) && ((a1[2] ^ a1[3]) == 0x12)) {
                        printf("%2x %2x %2x %2x\n", a1[0], a1[1], a1[2], a1[3]);
                        /*
                            8a a2 a8 ba
                            8b a3 a9 bb
                            8c a4 ae bc
                            8d a5 af bd
                            8e a6 ac be
                            8f a7 ad bf
                            c8 e0 ea f8
                            c9 e1 eb f9
                            ca e2 e8 fa
                            cb e3 e9 fb
                            cc e4 ee fc
                            cd e5 ef fd
                            ce e6 ec fe
                            cf e7 ed ff
                        */
                    }
                }
            }
        }
    }
}
