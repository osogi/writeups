#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>



char  rechange(char *a1, char *a2)
{
  char result; // al
  char v3; // [esp+0h] [ebp-4h]

  v3 = *a1;
  *a1 = *a2;
  result = v3;
  *a2 = v3;
  return result;
}

int  sub_4010E0(char* a1, char *Str, char* a3)
{
  size_t v4; // [esp+4h] [ebp-10h]
  unsigned char v5; // [esp+8h] [ebp-Ch]
  size_t v6; // [esp+Ch] [ebp-8h]
  unsigned char v7; // [esp+10h] [ebp-4h]
  v7 = 0;
  v5 = 0;
  v6 = 0;
  v4 = strlen(Str);
  while ( v6 < v4 )
  {

    v7 = (v7 + 1) % 0x100;
    v5 = (v5 + *(unsigned char *)(v7 + a1)) % 0x100;
    rechange((char *)(v7 + a1), (char *)(v5 + a1));

    *(char *)(v6 + a3) = *(char *)(a1
                                   + (*(unsigned char *)(v5 + a1) + (unsigned int)*(unsigned char *)(v7 + a1))
                                   % 0x100) ^ Str[v6];
    ++v6;
  }
  return 0;
}


int  sub_401030(char *Str, char *a2)
{
  signed int v3; // [esp+0h] [ebp-10h]
  unsigned char v4; // [esp+4h] [ebp-Ch]
  unsigned int i; // [esp+8h] [ebp-8h]
  unsigned int j; // [esp+Ch] [ebp-4h]
  v3 = strlen(Str);
  v4 = 0;

  for ( j = 0; j < 0x100; ++j )
  {
    v4 = ((unsigned char)Str[(int)j % v3] + v4 + *(unsigned char *)(j + a2)) % 0x100;
    rechange(j + a2, v4 + a2);
  }
  return 0;
}


int  some_crypt(char *Str, char *a2, char* a3)
{
  char v4[256]; // [esp+0h] [ebp-104h] BYREF
    for ( int i = 0; i < 0x100; ++i ){
    *(char *)(i + v4) = i;
    
  }
  sub_401030(Str, v4);
  sub_4010E0(v4, a2, a3);
  return 0;
}

int main(){
   char *v4; // [esp+0h] [ebp-20h]
  char *v5; // [esp+4h] [ebp-1Ch]
  char *new_block2; // [esp+8h] [ebp-18h]
  char *block3; // [esp+Ch] [ebp-14h]
  char *block2; // [esp+10h] [ebp-10h]
  char *new_block1; // [esp+14h] [ebp-Ch]
  char *block1; // [esp+18h] [ebp-8h]
  int s; // [esp+1Ch] [ebp-4h]
  char peer1_0[] = { /* Packet 4 */
0x22, 0x4e, 0x53, 0x29, 0x50, 0x27, 0xb1, 0x0d, 
0x8a, 0x77, 0x37, 0xc8, 0x02, 0x57, 0x22, 0x3e, 
0xa9, 0xef, 0xc1, 0x23, 0x73, 0x6d, 0xa6, 0x7a, 
0x58, 0xa7, 0xf4, 0x77, 0xb4, 0x76, 0xf1, 0x96, 
0xd3, 0xdb, 0x27, 0xe7, 0x9a, 0xc7, 0xcc, 0x14,
0x66, 0x2f, 0x4a, 0x40, 0x61, 0x71, 0xe5, 0x65, 
0xac, 0xb9, 0x96, 0xb1, 0x61, 0x97, 0x60, 0x9a, 
0x5c, 0x1d, 0x54, 0x15, 0xc9, 0xfb, 0xa7, 0xde, 
0x12, 0x6d, 0xe0, 0xb3, 0x2d, 0x64, 0xf2, 0xb5, 
0x81, 0x55, 0xb5, 0x42, 0xb9, 0x5a, 0x16, 0x0b, 
0x21, 0x2f, 0xca, 0x84, 0x30, 0xec, 0xfd, 0xad, 
0x8e, 0x9b, 0xf1, 0x7b, 0xf6, 0xe9, 0xb0, 0x9b };

  block1 = (char *)calloc(1u, 0x100u);
  memcpy(block1, &peer1_0[0], 32);
  block2 = (char *)calloc(1u, 0x100u);
  memcpy(block2, &peer1_0[32], 32);
  block3 = (char *)calloc(1u, 0x100u);
  memcpy(block3, &peer1_0[64], 32);
  new_block1 = (char *)calloc(1u, 0x100u);
  some_crypt(block1, block2, new_block1);
  new_block2 = (char *)calloc(1u, 0x100u);
  some_crypt(block1, block3, new_block2);
  v5 = calloc(1u, 0x100u);
  some_crypt(new_block1, new_block2, v5);
  for(int i=0; i<64; i++) printf("%p ", *(new_block1+i));
  printf("\n");
  for(int i=0; i<64; i++) printf("%p ", *(v5+i));
  printf("\n");
}
