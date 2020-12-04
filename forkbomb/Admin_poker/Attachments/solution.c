#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

int comp (const int *i, const int *j)
{
return *i - *j;
}
unsigned long winner (long table[5], long hand[2] )

{
  unsigned long result;
char masti[4]={'c','d','h','s'};
int* zt=malloc(5*sizeof(int));
char* mt=malloc(5*sizeof(char));
for (int i=0; i<7; i++)
{
  if(i<5){
zt[i]=*(int*)table[i];
mt[i]=*(char*)(table[i]+4);
  }
  else
  {
zt[i]=*(int*)hand[i-5];
mt[i]=*(char*)(hand[i-5]+4);
  }
}
qsort(zt, 7, sizeof(int), (int(*) (const void *, const void *)) comp);
int kol_z[15]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
for(int i=0; i<7; i++)
{
kol_z[zt[i]]++;
}
//starshaya
 for(int i=0; i<15; i++) 
 {
  if(kol_z[i]==1)   result=i;
 }
//para
 for(int i=0; i<15; i++) 
 {
  if(kol_z[i]==2)
  {   
  result=0x10*i;
    for(int b=15; b>0; b--) 
    {
      if(kol_z[b]==1)   {result+=b; b=-3;}
    }
 }

 }
//dve pary
 for(int i=0; i<15; i++) 
 {
  if(kol_z[i]==2) 
  {
    for(int b=i+1; b<15; b++)
    {
      if(kol_z[b]==2)  result=0x100*b+i;
    }
  }
 }
//troika
 for(int i=0; i<15; i++) 
 {
  if(kol_z[i]==3) {
    result=0x1000*i;
  for(int b=15; b>0; b--) 
    {
      if(kol_z[b]==1)   {result+=b; b=-3;}
    }
  }
 }
//street
for(int i=0; i<15-5; i++){
  int buf=1;
  for(int b=0; b<5; b++)
  {
    if(kol_z[b+i]<1) buf=0;
  }
  if(buf) result=0x10000*i;
}
//flash
for(int i=0; i<4; i++)
{
  int buf=0;
  char cel = masti[i];
  for(int b=0; b<7; b++)
  {
    if(mt[b]==cel) buf++;
  }
  if(buf>4) result=0x100000;
}
//xaoc
 for(int i=0; i<15; i++) 
 {
  if(kol_z[i]==3) 
  {
    for(int b=0; b<15; b++)
    {
      if(kol_z[b]==2)  result=0x1000000*i;
    }
  }
 }
 //kare
 for(int i=0; i<15; i++) 
 {
if(kol_z[i]>=4) result=0x10000000*i;
 }
return result;
}


long *sub_202880()
{
  long *result; // rax

  result = malloc(8uLL);
  *result = 0LL;
  return result;
}
char * sub_202DB0(char a1, int a2)
{
  char *result; // rax

  result = malloc(8uLL);
  result[4] = a1;
  *(int *)result = a2;
  return result;
}
long  sub_2030C0(long a1)
{
  long result; // rax
  unsigned int i; // [rsp+4h] [rbp-Ch]

  *(int *)(a1 + 16) = 0;
  if ( *(long *)(a1 + 64) )
    free(*(void **)(a1 + 64));
  for ( i = 0; ; ++i )
  {
    result = i;
    if ( (int)i >= *(int *)(a1 + 8) )
      break;
    if ( *(long *)(*(long *)(*(long *)a1 + 8LL * (int)i) + 24LL) )
    {
      free(*(void **)(*(long *)(*(long *)a1 + 8LL * (int)i) + 24LL));
      free(*(void **)(*(long *)(*(long *)a1 + 8LL * (int)i) + 32LL));
    }
  }
  return result;
}

long  sub_202900(long **a1)
{
  long v2; // [rsp+0h] [rbp-20h]
  long *ptr; // [rsp+8h] [rbp-18h]

  if ( !*a1 )
    return 0LL;
  ptr = *a1;
  v2 = **a1;
  *a1 = (long *)(*a1)[1];
  free(ptr);
  return v2;
}
long * sub_2028B0(long *a1, long a2)
{
  long *result; // rax

  result = malloc(0x10uLL);
  *result = a2;
  result[1] = *a1;
  *a1 = (long)result;
  return result;
}
long *sub_202C00()
{
  char *v0; // rax
  int v1; // esi
  char *v2; // rax
  int v3; // esi
  char *v4; // rax
  int v5; // esi
  char *v6; // rax
  int v7; // esi
  unsigned int v8; // eax
  int v9; // eax
  long v11; // [rsp+8h] [rbp-1C8h]
  int v12; // [rsp+10h] [rbp-1C0h]
  long *v13; // [rsp+18h] [rbp-1B8h]
  long v14[53]; // [rsp+20h] [rbp-1B0h]
  int v15; // [rsp+1C8h] [rbp-8h]
  int i; // [rsp+1CCh] [rbp-4h]

  v13 = sub_202880();
  i = 2;
  v15 = 0;
  while ( i < 15 )
  {
    v0 = sub_202DB0(100, i);
    v1 = v15++;
    v14[v1] = (long)v0;
    v2 = sub_202DB0(99, i);
    v3 = v15++;
    v14[v3] = (long)v2;
    v4 = sub_202DB0(104, i);
    v5 = v15++;
    v14[v5] = (long)v4;
    v6 = sub_202DB0(115, i);
    v7 = v15++;
    v14[v7] = (long)v6;
    ++i;
  }
  v8 = time(0LL);
  srand(v8);
  v12 = 52;
  while ( v12 > 0 )
  {
    v9 = rand();
    v11 = v14[--v12];
    v14[v12] = v14[v9 % 52];
    v14[v9 % 52] = v11;
  }
  for ( i = 0; i < 52; ++i )
    sub_2028B0(v13, v14[i]);
  return v13;
}
long  sub_202F90(long a1)
{
  long result; // rax
  int i; // [rsp+24h] [rbp-Ch]
  int j; // [rsp+24h] [rbp-Ch]

  sub_2030C0(a1);
  *(long *)(a1 + 64) = (long)sub_202C00();
  sub_202900(*(long ***)(a1 + 64));
  for ( i = 0; i < *(int *)(a1 + 8); ++i )
    *(long *)(*(long *)(*(long *)a1 + 8LL * i) + 24LL) = sub_202900(*(long ***)(a1 + 64));
  for ( j = 0; j < *(int *)(a1 + 8); ++j )
    *(long *)(*(long *)(*(long *)a1 + 8LL * j) + 32LL) = sub_202900(*(long ***)(a1 + 64));
  *(long *)(a1 + 24) = sub_202900(*(long ***)(a1 + 64));
  *(long *)(a1 + 32) = sub_202900(*(long ***)(a1 + 64));
  *(long *)(a1 + 40) = sub_202900(*(long ***)(a1 + 64));
  *(long *)(a1 + 48) = sub_202900(*(long ***)(a1 + 64));
  result = sub_202900(*(long ***)(a1 + 64));
  *(long *)(a1 + 56) = result;
  return result;
}
long  sub_202F40(long a1)
{
  long result; // rax

  *(int *)(a1 + 12) = 0;
  *(long *)(a1 + 64) = 0LL;
  *(int *)(a1 + 16) = 0;
  result = sub_202F90(a1);
  *(int *)(a1 + 12) = 0;
  return result;
}
long  sub_202EE0(long a1, long a2)
{
  long result; // rax

  *(long *)(*(long *)a1 + 8LL * (int)(*(int *)(a1 + 8))++) = a2;
  result = a1;
  if ( *(int *)(a1 + 8) == 3 )
    result = sub_202F40(a1);
  return result;
}
ssize_t doog_fun(int a1, const char *a2)
{
  size_t v2; // rax

  v2 = strlen(a2);
  return send(a1, a2, v2, 0);
}
int *sub_202E90()
{
  int *v0; // rdi

  v0 = malloc(0x48uLL);
  *(long *)v0 = (long)malloc(0x18uLL);
  v0[2] = 0;
  v0[3] = 0;
  return v0;
}
int * sub_202DF0(long a1)
{
  int *result; // rax

  result = malloc(0x50uLL);
  *(long *)result = a1;
  result[2] = 0;
  result[3] = 1128792064;
  result[4] = 0;
  result[5] = 0;
  *((long *)result + 3) = 0LL;
  *((long *)result + 4) = 0LL;
  return result;
}

char * sub_2029F0(long a1)
{
  size_t v1; // rax
  size_t v3; // [rsp+8h] [rbp-38h]
  char *v4; // [rsp+20h] [rbp-20h]
  char *v5; // [rsp+28h] [rbp-18h]
  char *s; // [rsp+30h] [rbp-10h]

  switch ( *(int *)a1 )
  {
    case 2:
      s = "Two";
      break;
    case 3:
      s = "Three";
      break;
    case 4:
      s = "Four";
      break;
    case 5:
      s = "Five";
      break;
    case 6:
      s = "Six";
      break;
    case 7:
      s = "Seven";
      break;
    case 8:
      s = "Eight";
      break;
    case 9:
      s = "Nine";
      break;
    case 0xA:
      s = "Ten";
      break;
    case 0xB:
      s = "Jack";
      break;
    case 0xC:
      s = "Queen";
      break;
    case 0xD:
      s = "King";
      break;
    case 0xE:
      s = "Ace";
      break;
    default:
      break;
  }
  if ( *(char *)(a1 + 4) == 100 )
    v5 = "Diamonds";
  if ( *(char *)(a1 + 4) == 99 )
    v5 = "Clubs";
  if ( *(char *)(a1 + 4) == 115 )
    v5 = "Spades";
  if ( *(char *)(a1 + 4) == 104 )
    v5 = "Hearts";
  v3 = strlen(s);
  v1 = strlen(v5);
  v4 = (char *)malloc(v1 + v3 + 5);
  sprintf(v4, "%s of %s", s, v5);
  return v4;
}

void main()
{
  long v0; // rax
  long v1; // rax
  long v2; // rax
  long v3; // rax
  long v5; // [rsp+178h] [rbp-E38h]
  long v6; // [rsp+180h] [rbp-E30h]
  long v7; // [rsp+188h] [rbp-E28h]
  long v8; // [rsp+190h] [rbp-E20h]
  long v9; // [rsp+1A0h] [rbp-E10h]
  long v10; // [rsp+1A8h] [rbp-E08h]
  long v11; // [rsp+1B0h] [rbp-E00h]
  long v12; // [rsp+1C0h] [rbp-DF0h]
  long v13; // [rsp+1C8h] [rbp-DE8h]
  long v14; // [rsp+1E8h] [rbp-DC8h]
  char buf[0x400]; // [rsp+320h] [rbp-C90h]
  struct sockaddr addr; // [rsp+720h] [rbp-890h]
  int v17; // [rsp+730h] [rbp-880h]
  int fd; // [rsp+734h] [rbp-87Ch]
  char *haystack; // [rsp+738h] [rbp-878h]
  int v20; // [rsp+740h] [rbp-870h]
  int v21; // [rsp+744h] [rbp-86Ch]
  long v22; // [rsp+748h] [rbp-868h]
  long v23; // [rsp+750h] [rbp-860h]
  long v24; // [rsp+758h] [rbp-858h]
  long v25; // [rsp+760h] [rbp-850h]
  long v26; // [rsp+768h] [rbp-848h]
  char dest; // [rsp+770h] [rbp-840h]
  char s; // [rsp+B70h] [rbp-440h]
  int v29; // [rsp+F70h] [rbp-40h]
  int v30; // [rsp+F74h] [rbp-3Ch]
  int v31; // [rsp+F78h] [rbp-38h]
  char v32; // [rsp+F7Dh] [rbp-33h]
  char v33; // [rsp+F7Eh] [rbp-32h]
  char v34; // [rsp+F7Fh] [rbp-31h]
  char v35; // [rsp+F80h] [rbp-30h]
  char v36; // [rsp+F81h] [rbp-2Fh]
  char v37; // [rsp+F82h] [rbp-2Eh]
  char v38; // [rsp+F83h] [rbp-2Dh]
  unsigned int v39; // [rsp+F84h] [rbp-2Ch]
char check=0;
float bex;
  v39 = 0;
  v38 = 0;
  v37 = 0;
  v36 = 0;
  v35 = 0;
  v34 = 0;
  v33 = 0;
  v32 = 0;
  v31 = 0;
  v30 = 1;
  v29 = 0;
  memset(&s, 0, 0x400uLL);
  memset(&dest, 0, 0x400uLL);
  v26 = 0LL;
  v25 = 0LL;
  v24 = 0LL;
  v23 = 0LL;
  v22 = 0LL;
  v21 = 0;
  v20 = 0;
  haystack = 0LL;
  fd = 0;
  memset(buf, 0, 0x400uLL);
  fd = socket(2, 1, 0);
  //long counter=0;
  if ( fd >= 0 )
  {
    addr.sa_family = AF_INET;
    *(short *)addr.sa_data = 6701;
    if ( (int)inet_pton(2LL, "109.233.56.90", &addr.sa_data[2]) > 0 )
    {
      if ( connect(fd, &addr, 0x10u) >= 0 )
      {
        
        doog_fun(fd, "CLIENT_HELLO\n");
        v17 = read(fd, buf, 0x400uLL);
        if ( strstr(buf, "SERVER_HELLO\n") )
        {
          
          memset(buf, 0, 0x400uLL);
          
          while ( 1 )
          {
            doog_fun(fd, "START_GAME\n");
            v17 = read(fd, buf, 0x400uLL);
            if ( !strstr(buf, "STARTED\n") )
              break;
              //*(int *)(**(long **)v26 + 12LL)=1000;
            
            v26 = (long)sub_202E90();
            v25 = (long)sub_202DF0((long)"You");
            v24 = (long)sub_202DF0((long)"Black_cat");
            v23 = (long)sub_202DF0((long)"White_cat");
            sub_202EE0(v26, v25);
            sub_202EE0(v26, v24);
            sub_202EE0(v26, v23);

            v30 = 1;
            v21 = 0;
            v29 = 0;
            while ( 1 )
            {
              
              memset(&dest, 0, 0x400uLL);
              v20 = 0;
              if ( v30 == 1 )
              {
                *(int *)(**(long **)v26 + 8LL) = 0;
                *(int *)(*(long *)(*(long *)v26 + 8LL) + 8LL) = 0;
                *(int *)(*(long *)(*(long *)v26 + 16LL) + 8LL) = 0;
                printf("=====NEW HAND=====\n\n");
                v30 = 0;
              }
              do
              {

                v29 = 0;
                fflush(stdout);
                fflush(stdin);
                memset(buf, 0, 0x400uLL);
                v17 = read(fd, buf, 0x400uLL);
                fflush(stdout);
                fflush(stdin);
                for ( haystack = strtok(buf, "\n"); haystack; haystack = strtok(0LL, "\n") )
                {
                  if ( strstr(haystack, "WHATS_YOUR_BET") )
                  {
                    v20 = 1;
                    v29 = 1;
                  }
                  if ( strstr(haystack, "PRIZES_GIVEN") )
                  {
                    v21 = 1;
                    v29 = 1;
                  }
                  if ( strstr(haystack, "MESSAGE") )
                  {
                    strcat(&dest, haystack + 8);
                    strcat(&dest, "\n");
                  }
                }
              }
              while ( v29 != 1 );
              memset(buf, 0, 0x400uLL);
              doog_fun(fd, "GET_GAME_STATE\n");
              v17 = read(fd, buf, 0x400uLL);
              if ( !strstr(buf, "GAME_STATE") )
                break;
              sscanf(
                buf,
                "GAME_STATE %hhd %d %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c %d %c %f %f %f %f %f %f %f",
                &v35,
                *(long *)(**(long **)v26 + 24LL),
                *(long *)(**(long **)v26 + 24LL) + 4LL,
                *(long *)(**(long **)v26 + 32LL),
                *(long *)(**(long **)v26 + 32LL) + 4LL,
                *(long *)(*(long *)(*(long *)v26 + 8LL) + 24LL),
                *(long *)(*(long *)(*(long *)v26 + 8LL) + 24LL) + 4LL,
                *(long *)(*(long *)(*(long *)v26 + 8LL) + 32LL),
                *(long *)(*(long *)(*(long *)v26 + 8LL) + 32LL) + 4LL,
                *(long *)(*(long *)(*(long *)v26 + 16LL) + 24LL),
                *(long *)(*(long *)(*(long *)v26 + 16LL) + 24LL) + 4LL,
                *(long *)(*(long *)(*(long *)v26 + 16LL) + 32LL),
                *(long *)(*(long *)(*(long *)v26 + 16LL) + 32LL) + 4LL,
                *(long *)(v26 + 24),
                *(long *)(v26 + 24) + 4LL,
                *(long *)(v26 + 32),
                *(long *)(v26 + 32) + 4LL,
                *(long *)(v26 + 40),
                *(long *)(v26 + 40) + 4LL,
                *(long *)(v26 + 48),
                *(long *)(v26 + 48) + 4LL,
                *(long *)(v26 + 56),
                *(long *)(v26 + 56) + 4LL,
                **(long **)v26 + 12LL,
                *(long *)(*(long *)v26 + 8LL) + 12LL,
                *(long *)(*(long *)v26 + 16LL) + 12LL,
                **(long **)v26 + 16LL,
                *(long *)(*(long *)v26 + 8LL) + 16LL,
                *(long *)(*(long *)v26 + 16LL) + 16LL,
                v26 + 16);

              
              if ( *(float *)(**(long **)v26 + 12LL) > 500.0 )
              {
                printf("I must get fklag: \n");
                doog_fun(fd, "GET_FLAG\n");
                v17 = read(fd, buf, 0x400uLL);
                printf(buf);
                exit(0);
              }
              
              if ( *(float *)(**(long **)v26 + 12LL) <= 0.0 )
              {
                printf("W A S T E D\n");
                printf("My balance: %f\n", *(float *)(**(long **)v26 + 12LL));
                printf(&dest);
                
              }
              if ( v35 == -2 )
              {
                printf("===HAND ENDED===\n\n");
                printf("My balance: %f\n", *(float *)(**(long **)v26 + 12LL));
                printf("\nBlack cat balance: %f\n", *(float *)(*(long *)(*(long *)v26 + 8LL) + 12LL));
                printf("White cat balance: %f\n", *(float *)(*(long *)(*(long *)v26 + 16LL) + 12LL));
                printf("Wait for a new hand...\n\n\n\n");
                sleep(3u);
                break;
              }
              printf("\nMy balance: %f\n", *(float *)(**(long **)v26 + 12LL));

              printf("======================================");
              v14 = (long)sub_2029F0(*(long *)(**(long **)v26 + 24LL));
              v0 = (long)sub_2029F0(*(long *)(**(long **)v26 + 32LL));
              printf("\nMy hand: %s and %s\n\n", v14, v0);


              v14 = (long)sub_2029F0(*(long *)(*(long *)(*(long *)v26 + 8LL) + 24LL));
              v0 = (long)sub_2029F0(*(long *)(*(long *)(*(long *)v26 + 8LL) + 32LL));
              printf("Black cat hand: %s and %s\n\n", v14, v0);


              v14 = (long)sub_2029F0(*(long *)(*(long *)(*(long *)v26 + 16LL) + 24LL));
              v0 = (long)sub_2029F0(*(long *)(*(long *)(*(long *)v26 + 16LL) + 32LL));
              printf("White cat hand: %s and %s\n\n", v14, v0);

                              
                  //printf("=====RIVER=====\n\n");
                  v8 = (long)sub_2029F0(*(long *)(v26 + 24));
                  v7 = (long)sub_2029F0(*(long *)(v26 + 32));
                  v6 = (long)sub_2029F0(*(long *)(v26 + 40));
                  v5 = (long)sub_2029F0(*(long *)(v26 + 48));
                  v3 = (long)sub_2029F0(*(long *)(v26 + 56));
                  printf("River: %s, %s, %s, %s, %s\n", v8, v7, v6, v5, v3);
                long me[2] ={*(long *)(**(long **)v26 + 24LL),  *(long *)(**(long **)v26 + 32LL)};
                long Black_cat[2]={*(long *)(*(long *)(*(long *)v26 + 8LL) + 24LL),  *(long *)(*(long *)(*(long *)v26 + 8LL) + 32LL)};
                long White_cat[2]={*(long *)(*(long *)(*(long *)v26 + 16LL) + 24LL),  *(long *)(*(long *)(*(long *)v26 + 16LL) + 32LL)};
                float a=*(float *)(*(long *)(*(long *)v26 + 8LL) + 16LL); float b= *(float *)(*(long *)(*(long *)v26 + 16LL) + 16LL) ;
                bex = a > b ? a : b;
                char aa=  a>0;
                char ab = b>0;
                

                printf("\n bex =  %f\n", bex);
                //if(counter%3==0)
                //{
                  float bal =*(float *)(**(long **)v26 + 12LL);
                  if(check==1) printf(" %f\n", bal);
                  //printf("====Round %d====", (counter/3) );
                  unsigned long good_me = winner((long *)(v26 + 24), me);
                  unsigned long good_a=winner((long *)(v26 + 24), Black_cat);
                  unsigned long good_b=winner((long *)(v26 + 24), White_cat);
                  printf("[~] me==%lx\n", good_me);
                  printf("[~] Black_cat==%lx\n", good_a);
                  printf("[~] White_cat==%lx\n", good_b);
                  check=1; 
                  if((aa)&&(good_a>good_me)) check=0;
                  if((ab)&&(good_b>good_me)) check=0;
                  if(check) printf("[CyCat]: I will win\n");
                //}
                printf("======================================\n");
               // counter++;
              if ( *(float *)(**(long **)v26 + 16LL) != 0.0 )
                printf("My previous bet: %f\n", *(float *)(**(long **)v26 + 16LL));
              if ( v35 < 3 && v35 >= 0 )
              {
                if ( *(float *)(*(long *)(*(long *)v26 + 8LL) + 16LL) == 0.0
                  && *(float *)(*(long *)(*(long *)v26 + 16LL) + 16LL) == 0.0 )
                {
                  printf("Other players folded, you won!\n");
                  printf("Wait for a new hand...\n\n\n\n\n");
                  sleep(3u);
                  break;
                }


                if ( *(int *)(*(long *)(*(long *)v26 + 8LL) + 8LL) == 1 )
                {
                  printf("\nBlack cat already folded\n");
                }
                else if ( *(float *)(*(long *)(*(long *)v26 + 8LL) + 16LL) <= 0.0 )
                {
                  *(int *)(*(long *)(*(long *)v26 + 8LL) + 8LL) = 1;
                  printf("\nBlack cat folded\n");
                }
                else
                {
                  printf("\nBlack cat bet: %f\n", *(float *)(*(long *)(*(long *)v26 + 8LL) + 16LL));
                }
                if ( *(int *)(*(long *)(*(long *)v26 + 16LL) + 8LL) == 1 )
                {
                  printf("\nWhite cat already folded\n");
                }
                else if ( *(float *)(*(long *)(*(long *)v26 + 16LL) + 16LL) <= 0.0 )
                {
                  *(int *)(*(long *)(*(long *)v26 + 16LL) + 8LL) = 1;
                  printf("\nWhite cat folded\n");
                }
                else
                {
                  printf("White cat bet: %f\n", *(float *)(*(long *)(*(long *)v26 + 16LL) + 16LL));
                }
                printf("\nBank: %f\n", *(float *)(v26 + 16));
                printf("\nBlack cat balance: %f\n", *(float *)(*(long *)(*(long *)v26 + 8LL) + 12LL));
                printf("White cat balance: %f\n", *(float *)(*(long *)(*(long *)v26 + 16LL) + 12LL));
                if ( v20 == 1 )
                {


                  printf("Enter your bet:");
                  fflush(stdout);
                  char* str=malloc(100);
                  if(check==0) bex=0;
                  sprintf(str, "%f", bex);
                  sscanf(str, "%s", &s);
                  //scanf("%s", &s);
                  memset(buf, 0, 0x400uLL);
                  strcat(buf, "BET ");
                  strcat(buf, &s);
                  strcat(buf, "\n");
                  doog_fun(fd, buf);
                }
                fflush(stdout);
                fflush(stdin);
              }
              printf("\n\n\n");
              memset(buf, 0, 0x400uLL);
              memset(&s, 0, 0x400uLL);
              fflush(stdout);
              fflush(stdin);
            }
            memset(buf, 0, 0x400uLL);
            memset(&s, 0, 0x400uLL);
            fflush(stdout);
            fflush(stdin);
            
          }
          printf("Can't start a game\n");
          v39 = 0;
        }
        else
        {
          printf("Server didn't greet us\n");
          v39 = 0;
        }
      }
      else
      {
        printf("\nfailed to connect \n");
        v39 = -1;
      }
    }
    else
    {
      v39 = -1;
    }
  }
  else
  {
    printf("something went wrong\n");
    v39 = -1;
  }



}
