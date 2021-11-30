---
tags: writeup, rev, ctfcup
---
# **Write-up Simple Protocol**
> [name=User1]

![](https://i.imgur.com/LGh6HSP.png)

> [color=#1cefda] Здесь могла быть ваша реклама, но пока тут реклама [@ch4nnel1](https://t.me/ch4nnel1)

Original write-up: https://hackmd.io/@osogi/Simple_Protocol


## Разбор + история
![](https://i.imgur.com/940xSH7.png)

<p style="text-align: center;">Сам таск(https://game.ctfcup.ru/tasks/948157b9-8d44-477d-afac-4b46488d89d0)</p>

И еще гитхаб таска -> https://github.com/acisoru/ctfcup-21-quals/blob/main/tasks/reverse/simple-protocol/ 

### Пролог (поиск бирника)
Можем попробовать запустить бинарник, но нам это мало что даст, так как он просто читает что-то с ввода и потом отдает несколько байт.
![](https://i.imgur.com/Tr7jUMs.png)

Давайте посмотрим, что находится у него внутри

```clike
[0x000012d2]> pdf
            ; DATA XREF from entry0 @ 0x10a1
┌ 27: int main (int argc, char **argv, char **envp);
│           0x000012d2      f30f1efa       endbr64
│           0x000012d6      55             push rbp
│           0x000012d7      4889e5         mov rbp, rsp
│           0x000012da      488d3d230d00.  lea rdi, [0x00002004]       ; "Hello world!"
│           0x000012e1      e87afdffff     call fcn.00001060
│           0x000012e6      b800000000     mov eax, 0
│           0x000012eb      5d             pop rbp
└           0x000012ec      c3             ret
[0x000012d2]> s fcn.00001060
[0x00001060]> pdf
            ; CALL XREF from main @ 0x12e1
┌ 11: fcn.00001060 ();
│           0x00001060      f30f1efa       endbr64
└           0x00001064      f2ff255d2f00.  bnd jmp qword [reloc.puts]  ; [0x3fc8:8]=0x1030 fcn.00001030 ; "0\x10"
[0x00001060]> 
```

Странно, судя по main'у бинарь должен просто вывести hello word, однако ничего подобного не происходит и близко.  Попробуем поставить бряку на entry point и запустить. Иииии... прога не дойдет до нашей бряки, но будет выполнять какой-то код, а известных мне инитов или чего-то что должно запускаться перед entry нет (на самом деле есть читайте [Эпилог](#Эпилог)), вот так мистика. 
Ладно допустим, что путем странных манипуляций еще до захода в entry исполняется какой-то другой код.  Попробуем запустить приложуху под strace, чтобы хотя бы примерно понять что и как.

По логам последнего видим, что бинарь выполняет как-то слишком много работы с памятью вначале (использует syscall'ы `mmap` и `mprotect`), что может подвести нас к идее, что он запакован. Но проверив с помощью die, ничего интересного не получаем.

![](https://i.imgur.com/trHH7dN.png)

Хммм... Окей, попробуем продебажить радаром до тех пор, пока бинарь не начнет читать наш инпут. Я использовал для этого команду `dcs read`, которая запускает бинарь до вызова сискола read и ждал пока первый операнд будет равен 0, что означает что он читает из `stdin`.  

После окажемся где-то в районе 0x7f... адресов, давайте проверим в каком "мапе" мы очутились

```clike
[0x7fb2b1ac72ee]> dm
0x00007fb2b1846000 - 0x00007fb2b1886000 - usr   256K s rw- unk0 unk0
0x00007fb2b1886000 - 0x00007fb2b18ab000 - usr   148K s r-- /usr/lib/x86_64-linux-gnu/libc-2.31.so /usr/lib/x86_64-linux-gnu/libc-2.31.so
0x00007fb2b18ab000 - 0x00007fb2b1a23000 - usr   1.5M s r-x /usr/lib/x86_64-linux-gnu/libc-2.31.so /usr/lib/x86_64-linux-gnu/libc-2.31.so
0x00007fb2b1a23000 - 0x00007fb2b1a6d000 - usr   296K s r-- /usr/lib/x86_64-linux-gnu/libc-2.31.so /usr/lib/x86_64-linux-gnu/libc-2.31.so
0x00007fb2b1a6d000 - 0x00007fb2b1a6e000 - usr     4K s --- /usr/lib/x86_64-linux-gnu/libc-2.31.so /usr/lib/x86_64-linux-gnu/libc-2.31.so
0x00007fb2b1a6e000 - 0x00007fb2b1a71000 - usr    12K s r-- /usr/lib/x86_64-linux-gnu/libc-2.31.so /usr/lib/x86_64-linux-gnu/libc-2.31.so
0x00007fb2b1a71000 - 0x00007fb2b1a74000 - usr    12K s rw- /usr/lib/x86_64-linux-gnu/libc-2.31.so /usr/lib/x86_64-linux-gnu/libc-2.31.so
0x00007fb2b1a74000 - 0x00007fb2b1a78000 - usr    16K s rw- unk1 unk1
0x00007fb2b1a92000 - 0x00007fb2b1a93000 - usr     4K s r-- /memfd: (deleted) /memfd: (deleted)
0x00007fb2b1a93000 - 0x00007fb2b1a94000 - usr     4K s r-- /usr/lib/x86_64-linux-gnu/ld-2.31.so /usr/lib/x86_64-linux-gnu/ld-2.31.so
0x00007fb2b1a94000 - 0x00007fb2b1ab7000 - usr   140K s r-x /usr/lib/x86_64-linux-gnu/ld-2.31.so /usr/lib/x86_64-linux-gnu/ld-2.31.so
0x00007fb2b1ab7000 - 0x00007fb2b1abf000 - usr    32K s r-- /usr/lib/x86_64-linux-gnu/ld-2.31.so /usr/lib/x86_64-linux-gnu/ld-2.31.so
0x00007fb2b1abf000 - 0x00007fb2b1ac0000 - usr     4K s --- unk2 unk2
0x00007fb2b1ac0000 - 0x00007fb2b1ac1000 - usr     4K s r-- /usr/lib/x86_64-linux-gnu/ld-2.31.so /usr/lib/x86_64-linux-gnu/ld-2.31.so
0x00007fb2b1ac1000 - 0x00007fb2b1ac2000 - usr     4K s rw- /usr/lib/x86_64-linux-gnu/ld-2.31.so /usr/lib/x86_64-linux-gnu/ld-2.31.so
0x00007fb2b1ac2000 - 0x00007fb2b1ac3000 - usr     4K s rw- unk3 unk3
0x00007fb2b1ac4000 - 0x00007fb2b1ac6000 - usr     8K s rw- unk4 unk4
0x00007fb2b1ac6000 - 0x00007fb2b1ac7000 - usr     4K s r-- unk5 unk5
0x00007fb2b1ac7000 - 0x00007fb2b1aca000 * usr    12K s r-x unk6 unk6
0x00007fb2b1aca000 - 0x00007fb2b1acc000 - usr     8K s r-- unk7 unk7
0x00007fb2b1acc000 - 0x00007fb2b1ad0000 - usr    16K s rw- unk8 unk8
0x00007ffc88eda000 - 0x00007ffc88efb000 - usr   132K s rw- [stack] [stack]
0x00007ffc88f49000 - 0x00007ffc88f4d000 - usr    16K s r-- [vvar] [vvar]
0x00007ffc88f4d000 - 0x00007ffc88f4f000 - usr     8K s r-x [vdso] [vdso]
0xffffffffff600000 - 0xffffffffff601000 - usr     4K s --x [vsyscall] [vsyscall] ; map.vsyscall_.__x
```

Вау, сколько тут всего намапилось интересного еще до выполнения основного бинаря. 
Пока дебажил, еще случайно нашел пару строчек, которые содержали "upx", что приводит к мысле что в какой-то момент тут распаковывался upx файл и все эти подозрительные сегменты/мапы вызваны частично и им. Ну что же, поищем elf header

```clike
[0x7fb2b1ac72ee]> / \x7fELF
0x7fb2b1886000 hit11_0 .\u007fELF>.
0x7fb2b1a92000 hit11_1 .\u007fELF>.
0x7fb2b1a92110 hit11_2 .!\u007fELF>`wv.
0x7fb2b1a93000 hit11_3 .v%4!15\u007fELF>.
0x7fb2b1a9b41e hit11_4 .?vH?TH\u007fELFI9FpA.
0x7fb2b1a9b451 hit11_5 .A9~A~\u007fELFA~r.
0x7fb2b1a9b7a2 hit11_6 .jDfH\u007fELFI9F|.
0x7fb2b1a9c1c9 hit11_7 .?vH?H\u007fELFI9FA.
0x7fb2b1a9c1fc hit11_8 .A9~A~\u007fELFgA~.
0x7fb2b1a9c57a hit11_9 .DDH\u007fELFI9FOg.
0x7fb2b1ab74c0 hit11_10 .0-00\u007fELF.
0x7fb2b1ac6000 hit11_11 .\u007fELF>.
0x7ffc88ef9678 hit11_12 .@\u007fELF>.
0x7ffc88f4d000 hit11_13 .\u007fELF>.
```

Сопоставив эти данные с известными мапами, придем к выводу, что нам скорее всего нужны мапы с unk5 по unk8, так как в них и находится бинарник, который выполняет всю "полезную" работу. Сдампим их

```clike
[0x7fb2b1acf6c0]> pr 0x00007fb2b1ad0000-0x00007fb2b1ac6000 @ 0x00007fb2b1ac6000 > unpack_server.elf
```

Попытка запуска распакованного бинаря ничего не даст, 
![](https://i.imgur.com/vU3bPct.png)

Однако, закинув его в тот же радар, получим интересный результат, что это "живой", настоящий бинарь.

```clike
[0x000013c8]> afl
0x00001060    1 47           entry0
0x000013c8   41 1152         main
0x00001040    1 11           fcn.00001040
0x00001090    4 41   -> 34   fcn.00001090
0x00001209    6 122          fcn.00001209
0x00001185   11 132          fcn.00001185
0x000012a9    1 43           fcn.000012a9
0x000012d4    1 35           fcn.000012d4
0x00001316    1 44           fcn.00001316
0x000012f7    1 31           fcn.000012f7
0x00001149    1 60           fcn.00001149
0x00001342    4 134          fcn.00001342
0x00003894   15 1264         fcn.00003894
0x000019a4    1 22           fcn.000019a4
0x000019ba    4 75           fcn.000019ba
0x000018c0    1 32           fcn.000018c0
0x000018e0    1 24           fcn.000018e0
0x00001961    4 67           fcn.00001961
0x00001848    4 120          fcn.00001848
0x00002e6f   15 1241         fcn.00002e6f
0x000018f8    4 105          fcn.000018f8
0x00003d84    1 22           fcn.00003d84
0x00001a05    1 28           fcn.00001a05
0x00001a21    4 286          fcn.00001a21
0x00001050    1 11           fcn.00001050
0x00001fc1   37 1250         fcn.00001fc1
0x00001f60    1 97           fcn.00001f60
0x00001e99    4 199          fcn.00001e99
0x00001b3f    4 288          fcn.00001b3f
0x000024a3   34 1152         fcn.000024a3
0x00001d7b    4 286          fcn.00001d7b
0x00001c5f    4 284          fcn.00001c5f
0x00001000    3 27           segment.LOAD1
0x00001287    1 34           fcn.00001287
0x00002927   21 1352         fcn.00002927
0x0000334c   21 1352         fcn.0000334c
0x00001030    2 31   -> 28   fcn.00001030
0x00001100    5 57   -> 54   fcn.00001100
0x00001140    5 137  -> 60   fcn.00001140
0x00001283    1 4            fcn.00001283
0x00002923    1 4            fcn.00002923
0x00003348    1 4            fcn.00003348
0x00003da0    4 101          fcn.00003da0
0x00003e10    1 5            fcn.00003e10
0x00003e18    1 13           fcn.00003e18
```

[Анпакнутый elf](https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/attachment/unpack_serv.elf), если кому-то нужно

### Завязка (реверсинг бинарника)
Я конечно мазохист, но не настолько чтобы реверсить что-то в радаре, если это можно сделать в иде, так что погнали закинем распакованный бин в иду, и чуть пореверсим.
Тут я быстро расскажу о реверсинге некоторых функций и как это делалось
####  sub_1185 (imul)
```clike
unsigned __int64 __fastcall sub_1185(unsigned __int64 a1, unsigned __int64 a2)
{
  unsigned __int64 v5; // [rsp+10h] [rbp-10h]

  v5 = 0LL;
  while ( a2 )
  {
    if ( (a2 & 1) != 0 )
    {
      if ( v5 < -59LL - a1 )
        v5 += a1;
      else
        v5 -= -59LL - a1;
    }
    if ( a1 < -59LL - a1 )
      a1 *= 2LL;
    else
      a1 = 2 * a1 + 59;
    a2 >>= 1;
  }
  return v5;
}
```
Функция небольшая, но с наскоку трудно понять, что она делает. Но если же подправить и засунуть этот код в какую-нибудь gcc, чтобы потыкать его, то можно быстро определить, что это простое умножение двух чисел.  Обзовем ее `imul`.

####  sub_1209 (pow)
```clike 
__int64 __fastcall pow(__int64 a1, unsigned __int64 a2)
{
  __int64 v3; // rax
  unsigned __int64 v4; // [rsp+18h] [rbp-8h]

  if ( a2 == 1 )
    return a1;
  v3 = pow(a1, a2 >> 1);
  v4 = imul(v3, v3);
  if ( (a2 & 1) != 0 )
    v4 = imul(v4, a1);
  return v4;
}
```
Просто видно, что это бинарное возведение в степень.

#### Syscall funs
```clike
__int64 __fastcall read(int fd, void *buf, size_t count)
{
  __asm { syscall; LINUX - }
  return 1LL;
}
```
В бинаре есть ряд функций имеющих похожую структуру, просто смотрим асм, какой сискол вызывают, и настраиваем функцию под его аргументы.

#### sub_1149 (malloc)
```clike
void *__fastcall _malloc(int a1)
{
  void *v2; // [rsp+10h] [rbp-8h]

  v2 = (void *)((unsigned int)offset_for_empty_data + some_adr_for_malloc);
  offset_for_empty_data += a1;
  return v2;
}
```
По тому где вызывается функция и что делает, можно легко провести параллели с сишным маллоком.

#### sub_3894 и sub_2E6F(decode и encdode)
```clike
unsigned __int64 __fastcall decode(unsigned int *a1, __int64 a2, _BYTE *a3, unsigned int a4)
{
  __int64 v4; // rdx
  unsigned __int64 result; // rax
  int i; // [rsp+28h] [rbp-258h]
  int j; // [rsp+2Ch] [rbp-254h]
  int k; // [rsp+30h] [rbp-250h]
  unsigned int v10; // [rsp+34h] [rbp-24Ch]
  __int64 v11; // [rsp+40h] [rbp-240h]
  __int64 v12; // [rsp+48h] [rbp-238h]
  int v13[134]; // [rsp+50h] [rbp-230h] BYREF
  unsigned __int64 v14; // [rsp+268h] [rbp-18h]

  v14 = __readfsqword(0x28u);
  memset(v13, 0, 0x210uLL);
  crypt_keygen((__int64)v13, a2, (__int64)a3, a4);
  v11 = 0LL;
  v12 = 0LL;
  for ( i = 31; i >= 0; --i )
  {
    if ( i > 30 )
    {
      *a1 ^= v13[128];
      a1[1] ^= v13[129];
      a1[2] ^= v13[130];
      a1[3] ^= v13[131];
    }
    else
    {
      a1[2] = a1[3] ^ rol4(a1[2], 10) ^ (a1[1] << 7);
      *a1 = a1[3] ^ rol4(*a1, 27) ^ a1[1];
      a1[3] = rol4(a1[3], 25);
      a1[1] = rol4(a1[1], 31);
      a1[3] ^= a1[2] ^ (8 * *a1);
      a1[1] ^= *a1 ^ a1[2];
      a1[2] = rol4(a1[2], 29);
      *a1 = rol4(*a1, 19);
    }
    for ( j = 0; j <= 31; ++j )
    {
      v10 = dword_6820[16 * (i % 8)
                     + ((4 * (unsigned __int8)(a1[2] >> j)) & 4 | (2 * (unsigned __int8)(a1[1] >> j)) & 2 | (*a1 >> j) & 1 | (8 * (unsigned __int8)(a1[3] >> j)) & 8)];
      LODWORD(v11) = v11 | ((v10 & 1) << j);
      HIDWORD(v11) |= ((v10 >> 1) & 1) << j;
      LODWORD(v12) = v12 | (((v10 >> 2) & 1) << j);
      HIDWORD(v12) |= ((v10 >> 3) & 1) << j;
    }
    for ( k = 0; k <= 3; ++k )
    {
      a1[k] = *((_DWORD *)&v11 + k) ^ v13[4 * i + k];
      *((_DWORD *)&v11 + k) = 0;
    }
  }
  v4 = *((_QWORD *)a1 + 1);
  *(_QWORD *)a3 = *(_QWORD *)a1;
  *((_QWORD *)a3 + 1) = v4;
  result = __readfsqword(0x28u) ^ v14;
  if ( result )
    result = stack_fail();
  return result;
}
```

```clike
unsigned __int64 __fastcall encdode(unsigned int *mes, __int64 key, _BYTE *dest, unsigned int len)
{
  unsigned int v4; // ebx
  int v5; // ebx
  int v6; // eax
  unsigned int v7; // ebx
  int v8; // ebx
  int v9; // eax
  int v10; // eax
  int v11; // eax
  __int64 v12; // rdx
  unsigned __int64 result; // rax
  int i; // [rsp+28h] [rbp-258h]
  int j; // [rsp+2Ch] [rbp-254h]
  int k; // [rsp+30h] [rbp-250h]
  unsigned int v18; // [rsp+34h] [rbp-24Ch]
  __int64 v19; // [rsp+40h] [rbp-240h]
  __int64 v20; // [rsp+48h] [rbp-238h]
  int main_key[134]; // [rsp+50h] [rbp-230h] BYREF
  unsigned __int64 v22; // [rsp+268h] [rbp-18h]

  v22 = __readfsqword(0x28u);
  memset(main_key, 0, 0x210uLL);
  crypt_keygen((__int64)main_key, key, (__int64)dest, len);
  v19 = 0LL;
  v20 = 0LL;
  for ( i = 0; i <= 31; ++i )
  {
    for ( j = 0; j <= 3; ++j )
    {
      *((_DWORD *)&v19 + j) = main_key[4 * i + j] ^ mes[j];
      mes[j] = 0;
    }
    for ( k = 0; k <= 31; ++k )
    {
      v18 = dword_6020[16 * (i % 8)
                     + ((4 * (unsigned __int8)((unsigned int)v20 >> k)) & 4 | (2 * (unsigned __int8)(HIDWORD(v19) >> k)) & 2 | ((unsigned int)v19 >> k) & 1 | (8 * (unsigned __int8)(HIDWORD(v20) >> k)) & 8)];
      *mes |= (v18 & 1) << k;
      mes[1] |= ((v18 >> 1) & 1) << k;
      mes[2] |= ((v18 >> 2) & 1) << k;
      mes[3] |= ((v18 >> 3) & 1) << k;
    }
    if ( i > 30 )
    {
      *mes ^= main_key[128];
      mes[1] ^= main_key[129];
      mes[2] ^= main_key[130];
      mes[3] ^= main_key[131];
    }
    else
    {
      v4 = mes[1];
      v5 = rol4(*mes, 13) ^ v4;
      v6 = v5 ^ rol4(mes[2], 3);
      mes[1] = rol4(v6, 1);
      v7 = mes[3];
      v8 = rol4(mes[2], 3) ^ v7;
      v9 = v8 ^ (8 * rol4(*mes, 13));
      mes[3] = rol4(v9, 7);
      v10 = rol4(*mes, 13);
      *mes = rol4(v10 ^ mes[1] ^ mes[3], 5);
      v11 = rol4(mes[2], 3);
      mes[2] = rol4(v11 ^ mes[3] ^ (mes[1] << 7), 22);
    }
  }
  v12 = *((_QWORD *)mes + 1);
  *(_QWORD *)dest = *(_QWORD *)mes;
  *((_QWORD *)dest + 1) = v12;
  result = __readfsqword(0x28u) ^ v22;
  if ( result )
    result = stack_fail();
  return result;
}
```
Эти две функции существуют, выполняют какую-то крипту. И так как одна похожа на перевернутую другую, то я решил проверить с помощью дебага радаром, обратные ли они, и это оказалось так. Так что было решено что одна криптит, а другая декриптит.

#### Main
После реверса всех "дочерних" функций получаем примерно такой main

```clike
int __cdecl __noreturn main(int argc, const char **argv, const char **envp)
{
  int v3; // edx
  int v4; // eax
  int v5; // esi
  _BYTE dest[2]; // [rsp+6h] [rbp-5Ah] BYREF
  _BYTE count_1[4]; // [rsp+8h] [rbp-58h] BYREF
  unsigned int count; // [rsp+Ch] [rbp-54h] BYREF
  __int64 first_inp; // [rsp+10h] [rbp-50h] BYREF
  unsigned __int64 rand; // [rsp+18h] [rbp-48h] BYREF
  __int64 mes; // [rsp+20h] [rbp-40h] BYREF
  unsigned __int64 a2; // [rsp+28h] [rbp-38h]
  _BYTE *buf; // [rsp+30h] [rbp-30h]
  _BYTE *important_data; // [rsp+38h] [rbp-28h]
  _BYTE *user_secret; // [rsp+40h] [rbp-20h]
  void *out; // [rsp+48h] [rbp-18h]
  char *pathname; // [rsp+50h] [rbp-10h]
  unsigned __int64 v18; // [rsp+58h] [rbp-8h]

  v18 = __readfsqword(0x28u);
  first_inp = 0LL;
  read(0, &first_inp, 8uLL);
  rand = 0LL;
  get_random(&rand, 8uLL, v3);
  mes = pow(0xF549E9B5207189BCLL, rand);
  write(1, &mes, 8uLL);
  a2 = 0LL;
  a2 = pow(first_inp, rand);
  key_ = _malloc(16);
  crypt_xor(key_, a2);
  while ( 1 )
  {
    while ( 1 )
    {
      buf = _malloc(32);
      important_data = _malloc(32);
      read(0, buf, 0x20uLL);
      decode((unsigned int *)buf, (__int64)key_, important_data, 0x10u);
      decode((unsigned int *)buf + 4, (__int64)key_, important_data + 16, 0x10u);
      if ( *important_data != 0x22 || important_data[1] != 0x33 )
        exit();
      v4 = (unsigned __int8)important_data[2];
      if ( v4 != 4 )
        break;
      if ( fd )
        exit();
      if ( !flag_ok )
        exit();
      count = 0;
      _memcpy(&count, important_data + 3, sizeof(count));
      if ( count > 0x18 )
        exit();
      user_secret = _malloc(count);
      _memcpy(user_secret, important_data + 7, count);
      if ( !check_secret(user_secret, count) )
        exit();
      out = encode_flag(glob_for_flag, leng_of_flag);
      write(1, out, (leng_of_flag & 0xFFFFFFF0) + 16);
    }
    if ( (unsigned __int8)important_data[2] > 4u )
      break;
    switch ( v4 )
    {
      case 3:
        if ( !fd || fd == -1 )
          exit();
        close(fd);
        fd = 0;
        break;
      case 1:
        count = 0;
        _memcpy(&count, important_data + 3, sizeof(count));
        if ( count > 0x18 )
          exit();
        pathname = (char *)_malloc(count);
        v5 = (_DWORD)important_data + 7;
        _memcpy(pathname, important_data + 7, count);
        if ( fd )
          exit();
        fd = fopen(pathname, v5);
        if ( fd == -1 )
          exit();
        break;
      case 2:
        *(_WORD *)dest = 0;
        *(_DWORD *)count_1 = 0;
        count = 0;
        _memcpy(count_1, important_data + 3, sizeof(count_1));
        if ( *(_DWORD *)count_1 > 0x18u )
          exit();
        _memcpy(dest, important_data + 7, sizeof(dest));
        _memcpy(&count, important_data + 9, sizeof(count));
        if ( count > 0x80 )
          exit();
        glob_for_flag = (unsigned int *)_malloc(count);
        if ( !fd || fd == -1 )
          exit();
        read(fd, glob_for_flag, count);
        flag_ok = 1;
        leng_of_flag = count;
        break;
      default:
        goto LABEL_38;
    }
  }
LABEL_38:
  exit();
}
```

Из него мы можем понять, что сначала мы подаем число, которое используется для генерации "ключа". После отсылаем зашифрованные блоки, которые при расшифровке складываются в примерно такой протокол связи, по нулевому смещению находится хедер в виде 2 байт `\x22\x33`.

| Operation\Offset                             | +2 (operation code) | +3                            | +7                        | +9                      |
|----------------------------------------------|---------------------|-------------------------------|---------------------------|-------------------------|
| fopen                                        | 1                   | len of path to file (4 bytes) | filepath (len bytes)      | -                       |
| read file                                    | 2                   | some padding <=0x18 (4 bytes) | another padding (2 bytes) | how much read (4 bytes) |
| close <br>(simply close file, need for last) | 3                   | -                             | -                         | -                       |
| write data from file                         | 4                   | len of secret (4 bytes)       | secret (len bytes)        |                         |

[ДБ иды с этим файлом](https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/attachment/unpack_serv.i64)
### Кульминация (кодинг "клиента")
Вообще зная протокол, написать клиент не должно составить труда. Могут возникнуть только проблемы с тем, что мы не знаем степень, в которую возводится число, используемое для генерации ключа. Но ее можно либо попробовать высчитать, на основе известных данных, либо же заслать 1 или 0, так как что первое, что последнее в любой степени дают сами себя -> мы всегда будем знать, число которое использовалось для генерации ключа.

И так же возникают трудности с автоматическим декриптом и инкриптом сообщений. Что бы решить эту проблему у меня в голове было 3 способа:
- воспользоваться фридой 
- заинжектить so
- воспользоваться ~~r2pipe~~ rzpipe

Первый вариант я не выбрал так как, до сих пор не могу в фриду. Второй вариант мне не понравился, так как нужно писать сокеты внутри so, либо шаманить с обменом инфой между so и сторонним приложением. Так что я выбрал самый безболезненный, но костыльный вариант - rzpipe. И конечно буду кодить на питоне так как это быстрее всего. Разберем как же это все было реализовано.
Для начала разберем конструктор и какие переменные вводятся 
```python
class Crypter:
# заводим класс криптор
    def __init__(self, file_name):
        rflags = ["-dw"]
        r = rzpipe.open(file_name, flags=rflags) 
        # запускаем ризин в дебаг и райт моде 
        for _ in range(5):
            r.cmd("dcs read")
        # ждем пятого рида, так как пятый рид всегда наш
        # тот который находится внутри распакованого бинарника
		
        a=r.cmd("dr rip")
        a = int(a, 16)
        # получаем текущий рип, чтобы посчитать смещения
        # # + основные адреса 
		
        self.off = a-0x12EE
        # в бинаре рид назодится по адресу 0x12EE
        # путем несложной математики находим смещение
        self.r=r
		
		
        self.encode_addr=0x2E6F+self.off
        self.encode_addr_end = self.encode_addr+0x4d8
		
        self.decode_addr=0x3894+self.off
        self.decode_addr_end=self.decode_addr+0x4ef

        self.keyfun_addr=0x1342+self.off
        self.keyfun_addr_end=0x13C7+self.off
        # высчитываем адреса нужных функций и их конца
		
        m =0x9038+self.off
        self.mem_addr=int(r.cmdj(f"?j [{m}]")["uint64"])
        # находим адрес используемый кастомным malloc'ом
        # будем использовать его, чтобы знать куда можно писать
```

Дальше разберу реализацию одной из функций, остальные работают схожим образом
 


```python
def encrypt(self, message, key):
	r=self.r
	# получаем туже сессию ризина

	r.cmd(f"dr rip={self.encode_addr}")
	# прыгаем на адрес функции

	r.cmd(f"dr rcx=0x10") #len
	# передаем в аргументы длину декодируемого сообщения

	dest_addr = self.mem_addr+0x1000
	r.cmd(f"dr rdx={dest_addr}")
	# передаем адрес куда сохранить расшифрованное сообщение

	key_addr = self.mem_addr+0x1100
	r.cmd(f"dr rsi={key_addr}")
	# передаем адрес по которому будет находится ключ
	
	hexkey=key.to_bytes(16, byteorder=bo).hex()
	r.cmd(f"wx {hexkey} @ {key_addr}")
	# записываем ключ по этому адресу

	mes_addr = self.mem_addr+0x1200
	r.cmd(f"dr rdi={mes_addr}")
	# передаем адрес зашифрованного сообщения

	hexmes=message.hex()
	r.cmd(f"wx {hexmes} @ {mes_addr}")
	# записывем зашифрованное сообщение по этому адресу
	
	r.cmd(f"dcu {self.encode_addr_end}")
	# продолжаем выполнение до конца функции
	
	b = r.cmd(f"p8 16 @ {dest_addr}")
	# достаем значение по адресу куда попадает расшифрованное сообщение
	return bytes.fromhex(b)
```


Полный код клиента можно найти [тут](https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/solution/rizin_serv.py) 
*P.S. Чейнд не бей за использрвание pwntools, я хотел заfb'шить таск, а это единственное, что я помнил*

### Развязка (получение "секрета")
Вообще, изначально когда реверсил таск на ctf, я пропустил, что в протоколе идет проверка секрета и все такое, и заметил это в самом конце. Так что в райтапе тоже было решено это отложить на финал.

Вот вроде бы мы написали функции декрипта и инкрипта, все работает. Начинаем писать код для получения флага, как вдруг видим, что чтобы получить байты файла, в протоколе есть еще проверка какого-то секрета. Она реализована следующим образом.

```clike 
__int64 __fastcall secret_sub(_BYTE *secr, int len, unsigned int result)
{
  while ( len-- )
    result = (result << 8) ^ some_array[(unsigned __int8)(HIBYTE(result) ^ *secr++)];
	//some_array - массив из 256 dword'ов 
  return result;
}

_BOOL8 __fastcall check_secret(_BYTE *secr, unsigned int len)
{
  return (unsigned int)secret_sub(secr, len, 0xFFFFFFFF) == 0xDF705098;
}
```

В принципе тут почти сразу видно, что это можно взломать, так как нам известен желаемый результат (`0xDF705098`) и при `result << 8` последний байт 0, то можно найти среди `some_array` элемент, который тоже оканчивается на тот же байт, что и желаемый результат. Дальше сделать так 4 раза (так как 4 байта) + сделать поправочку на ксор, оформить это все в [скрипт](https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/attachment/hack_secret.py) и вуаля мы подобрали нужный секрет, и им оказался `\xca\xdb\x88\x2d`

Добавляем наш секрет в клиент, запускаем его. На локалке все робит, отлично, запускаем на сервер и получаем флаг.

![](https://i.imgur.com/oKT420I.png)


## Решение 

- понять, что по умолчанию в данном нам бинаре, почти нет ничего интересного
- найти, расшифровать, сдампить или еще каким-то образом получить ["настоящий" бинарь]((https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/attachment/unpack_serv.elf) )
- разреверсить его ([файл для иды с +- красивыми именами]((https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/attachment/unpack_serv.i64) ))
	- разобраться в протоколе 
	- понять, что крипто-функции взаимно обратные
- написать [клиент](https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/solution/rizin_serv.py) для получения флага 
	- каким-то образом реализовать крипто-функции сервера
	- реализовать протокол
	- найти секрет для получения флага ([скрипт]((https://github.com/osogi/writeups/blob/main/ctfcup2021/Simple_Server/attachment/hack_secret.py)))
- получить флаг

Авторское решение туть -> https://github.com/acisoru/ctfcup-21-quals/blob/main/tasks/reverse/simple-protocol/README.md

## Эпилог
Я посмотрел, потыкал почему у нас запускается какой-то другой код в начале бинаря, и прога даже не доходит до entry point'а. Как оказалось это вызвано тем что в коде был преинит участок (подробнее о всяких штуках происходящих перед загрузкой main'а можно почитать тут https://habr.com/ru/post/339698/), и радар хоть его и обнаружил, но справился плоховато: никак не обозвал эту функцию + плохо ее проанализировал - сильно ее обрезал (обрезал прям всю), в отличие от ризина, который справился с этим делом лучше
![](https://i.imgur.com/uRZO70G.png)

В очередной раз доказано, что ризин>радара. На самом деле у меня иногда тупит и тот и другой, так что я пользуюсь ими посменно, на одном таске могу по приколу юзать то одно, то другое.

---
Кого-то еще могло заинтересовать почему на картинке, превьюшке, какие-то аниме чувачки и как это связано с таском. Ну вот, это все из-за того, что я загуглил "protocol" нашел такую пикчу к какой-то игре, и мне понравилось как это выглядит.
![](https://i.imgur.com/o0yx6vq.jpg)