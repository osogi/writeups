---
tags: writeup, rev, ctfzone2021, sql
---
# **Write-up Secure Database (CTFZone 2021)**
> [name=User1]
> [time=Mon, Jun 28, 2021 5:07 AM]



![](https://i.imgur.com/qorHxoo.jpg)

> [color=#1cefda]Самый секьюрный канал на бешаном востоке [@ch4nnel1](https://t.me/ch4nnel1)

Original write-up: https://hackmd.io/UKS6HgoKTj-LNbw-ajGGVg

## Разбор
![](https://i.imgur.com/4sj6Tl8.png)
<p style="text-align: center;">Сам таск (https://ctf.bi.zone/challenges/8)</p>

Сначала, постучимся на сервер с помощью приведенной команды `nc securedb.2021.ctfz.one 7777`, он попросит у нас id дадим ему id, который столь любезно дали нам админы `229386e9-d6b0-4006-babc-5a15f7acee72`
![](https://i.imgur.com/oSf4CW4.png)

Просто блекбокся сервер, мы мало чего получим, так что стоит взглянуть файл из таска. В нем как нестрано дан бинарник.
Так как время поджимало (до конца ctf'а осталось около часа), то я сразу ~~подключил читы~~ воспользовался идой.
Там в списке функций сразу приметилась одна `start_routine`, рутина все такое.

![](https://i.imgur.com/Vr02UJY.png)

Открыв ее сразу замечаем строчку `"To retrieve your medical report..."`, что говорит о том, что мы нашли функцию с которой стоит начать курить бинарь.
### start_routine
```clike=1
void __fastcall __noreturn start_routine(void *a1)
{
  __int16 v1; // [rsp+20h] [rbp-20h] BYREF
  unsigned __int16 v2; // [rsp+22h] [rbp-1Eh]
  int fd; // [rsp+24h] [rbp-1Ch]
  void *buf; // [rsp+28h] [rbp-18h]
  void *ptr; // [rsp+30h] [rbp-10h]
  unsigned __int64 v6; // [rsp+38h] [rbp-8h]

  v6 = __readfsqword(0x28u);
  fd = *(_DWORD *)a1;
  buf = "To retrieve your medical report, please enter the unique identifier.\n\n";
  ptr = 0LL;
  v2 = 70;
  v1 = 0;
  if ( fd )
  {
    if ( send(fd, buf, v2, 0) != -1 )
    {
      ptr = (void *)sub_4013C8((unsigned int)fd, &v1);
      if ( ptr )
      {
        sub_4018E5((unsigned int)fd, ptr, (unsigned int)v1);
        free(ptr);
      }
    }
    shutdown(fd, 2);
    close(fd);
    puts("Thread exit..");
    pthread_exit(0LL);
  }
  puts("Connection failed, thread exit..");
  pthread_exit(0LL);
}
```
Дальше нас интересует функция `sub_4013C8`, в которую передается сокет (20 строчка).
### sub_4013C8
```clike=1
char *__fastcall sub_4013C8(int a1, __int16 *a2)
{
  char *result; // rax
  __int16 v3; // [rsp+10h] [rbp-C0h]
  __int16 v4; // [rsp+12h] [rbp-BEh]
  int v5; // [rsp+14h] [rbp-BCh]
  int v6; // [rsp+14h] [rbp-BCh]
  int v7; // [rsp+14h] [rbp-BCh]
  void *s; // [rsp+20h] [rbp-B0h]
  char *ptr; // [rsp+28h] [rbp-A8h]
  struct timeval timeout; // [rsp+30h] [rbp-A0h] BYREF
  fd_set readfds; // [rsp+40h] [rbp-90h] BYREF
  unsigned __int64 v12; // [rsp+C8h] [rbp-8h]

  v12 = __readfsqword(0x28u);
  memset(&readfds, 0, sizeof(readfds));
  readfds.fds_bits[a1 / 64] |= 1LL << (a1 % 64);
  timeout.tv_sec = 180LL;
  timeout.tv_usec = 0LL;
  v5 = select(a1 + 1, &readfds, 0LL, 0LL, &timeout);
  puts("Receiving data..");
  if ( v5 == -1 )
    goto LABEL_2;
  if ( !v5 )
    goto LABEL_28;
  puts("Data is available now..");
  s = malloc(3uLL);
  if ( !s )
    return 0LL;
  memset(s, 0, 3uLL);
  if ( recv(a1, s, 2uLL, 0) == -1 )
  {
    free(s);
    return 0LL;
  }
  *a2 = (unsigned __int8)sub_401847(s);
  free(s);
  if ( !*a2 )
    return 0LL;
  if ( *a2 <= 0 || *a2 > 36 )
    return 0LL;
  v6 = select(a1 + 1, &readfds, 0LL, 0LL, &timeout);
  if ( v6 == -1 )
  {
LABEL_2:
    puts("Select error...");
    return 0LL;
  }
  if ( v6 )
  {
    puts("Data is available now..");
    ptr = (char *)malloc(*a2 + 1);
    if ( ptr )
    {
      memset(ptr, 0, *a2 + 1);
      v3 = 0;
      timeout.tv_sec = 1LL;
      while ( *a2 > v3 )
      {
        v7 = select(a1 + 1, &readfds, 0LL, 0LL, &timeout);
        if ( v7 == -1 )
        {
          puts("Select error...");
          free(ptr);
          return 0LL;
        }
        if ( !v7 )
          break;
        v4 = recv(a1, &ptr[v3], *a2 - v3, 0);
        if ( v4 <= 0 )
        {
          free(ptr);
          return 0LL;
        }
        v3 += v4;
      }
      if ( *a2 == v3 )
      {
        result = ptr;
      }
      else
      {
        puts("Invalid data...");
        free(ptr);
        result = 0LL;
      }
    }
    else
    {
      puts("ERROR..");
      result = 0LL;
    }
  }
  else
  {
LABEL_28:
    puts("No data within 3 minutes. Exiting..");
    result = 0LL;
  }
  return result;
}
```
Тут нас будет интересовать получение данных и что потом с ними происходит. Для начала это строчки 31 и 36, по строчке 31 все понятно в переменную `s` записываются 2 первых байта, и после если все хорошо в 36 строчке `s` передается в функцию `sub_401847`. 
### sub_401847
```clike=1
__int64 __fastcall sub_401847(unsigned __int8 *a1)
{
  unsigned __int8 *v1; // rax
  unsigned __int8 v4; // [rsp+1Dh] [rbp-3h]
  unsigned __int8 v5; // [rsp+1Eh] [rbp-2h]
  unsigned __int8 v6; // [rsp+1Fh] [rbp-1h]
  char v7; // [rsp+1Fh] [rbp-1h]

  v4 = 0;
  v5 = 0;
  while ( v4 <= 1u )
  {
    v1 = a1++;
    v6 = *v1;
    if ( *v1 <= '/' || v6 > '9' )
    {
      if ( v6 <= '`' || v6 > 'f' )
      {
        if ( v6 <= '@' || v6 > 'F' )
        {
          puts("ERROR: hex convert to int..");
          return 0LL;
        }
        v7 = v6 - 55;
      }
      else
      {
        v7 = v6 - 87;
      }
    }
    else
    {
      v7 = v6 - 48;
    }
    v5 = (16 * v5) | v7 & 0xF;
    ++v4;
  }
  return v5;
}
```
Чуть изучив эту функцию легко понять, что она конвертит хексовое представление байта в инт (`int(s[:2], 16)` *- псевдокод в студию*).

Окей, вернемся к анализу [sub_4013C8](#sub_4013C8). Этот инт как раз записывается по адресу `a2`, из дальнейшего кода модно понять, что в `a2` хранится длина  данных, которые будут при приняты дальше (после первых 2 символов), записаны в `ptr` и возвращены в [start_routine](#start_routine). В результате в `start_routine` `ptr` хранит переданные данные (без первых двух байт), а `v1` - размер этих данных, и все это вместе с сокетом передается в `sub_4018E5`.
### sub_4018E5
```clike==1
__int64 __fastcall sub_4018E5(int a1, __int64 a2, unsigned int a3)
{
  const char *v3; // rax
  __int64 result; // rax
  _BYTE n_4[12]; // [rsp+24h] [rbp-2Ch] BYREF
  __int64 src; // [rsp+30h] [rbp-20h] BYREF
  const char *v8; // [rsp+38h] [rbp-18h] BYREF
  void *s; // [rsp+40h] [rbp-10h]
  unsigned __int64 v10; // [rsp+48h] [rbp-8h]

  v10 = __readfsqword(0x28u);
  *(_QWORD *)&n_4[4] = 0LL;
  src = 0LL;
  v8 = 0LL;
  s = 0LL;
  *(_QWORD *)n_4 = (unsigned int)sqlite3_open("patient_data.db", &n_4[4]);
  if ( *(_DWORD *)n_4 )
  {
    v3 = (const char *)sqlite3_errmsg(*(_QWORD *)&n_4[4]);
    fprintf(stderr, "Cannot open database: %s\n", v3);
    sqlite3_close(*(_QWORD *)&n_4[4]);
    result = 0LL;
  }
  else
  {
    s = malloc(0x34uLL);
    if ( s )
    {
      memset(s, 0, 0x34uLL);
      memcpy(s, "SELECT * FROM PatientList WHERE HashID = '", 0x2AuLL);
      src = sub_401B1B(a2, a3, 64400LL);
      memcpy((char *)s + 42, &src, 8uLL);
      memcpy((char *)s + 50, "'", sizeof(char));
      if ( (unsigned int)sqlite3_exec(*(_QWORD *)&n_4[4], s, sub_401CE1, a1, &v8) )
      {
        fprintf(stderr, "SQL error: %s\n", v8);
        free(s);
        sqlite3_free(v8);
        sqlite3_close(*(_QWORD *)&n_4[4]);
        result = 0LL;
      }
      else
      {
        free(s);
        sqlite3_close(*(_QWORD *)&n_4[4]);
        result = 1LL;
      }
    }
    else
    {
      puts("ERROR..");
      sqlite3_close(*(_QWORD *)&n_4[4]);
      result = 0LL;
    }
  }
  return result;
}
```
Одно из самых интересных мест для нас это строчки 30-36 в них создается строчка `s="SELECT * FROM PatientList WHERE HashID = '{src}'"`, где `src` занимает 8 байт и получается посредством передачи в `sub_401B1B` значений `ptr` и `v1` из `sub_4018E5`. После как нестрано `s` используется как sql запрос, форматированный результат которого и выдается нам по сокету назад.

Дальше я пытался понять каким HashID обладает нужный мне пациент, но решил подумать об этом позже, когда научусь генерить нужные `s`. Так что я пошел изучать `sub_401B1B`.
### sub_401B1B
```clike=1
unsigned __int64 __fastcall sub_401B1B(unsigned __int8 *a1, unsigned __int64 a2, __int64 a3)
{
  unsigned __int8 *v3; // rax
  unsigned __int64 v5; // [rsp+20h] [rbp-38h]
  unsigned __int8 *i; // [rsp+28h] [rbp-30h]

  v5 = a3 ^ (0xC6A4A7935BD1E995LL * a2);
  for ( i = a1; i != &a1[8 * (a2 >> 3)]; i += 8 )
  {
    v3 = i;
    v5 = 0xC6A4A7935BD1E995LL
       * ((0xC6A4A7935BD1E995LL
         * (((0xC6A4A7935BD1E995LL * *(_QWORD *)v3) >> 47) ^ (0xC6A4A7935BD1E995LL * *(_QWORD *)v3))) ^ v5);
  }
  switch ( a2 & 7 )
  {
    case 1uLL:
      goto LABEL_11;
    case 2uLL:
      goto LABEL_10;
    case 3uLL:
      goto LABEL_9;
    case 4uLL:
      goto LABEL_8;
    case 5uLL:
      goto LABEL_7;
    case 6uLL:
      goto LABEL_6;
    case 7uLL:
      v5 ^= (unsigned __int64)i[6] << 48;
LABEL_6:
      v5 ^= (unsigned __int64)i[5] << 40;
LABEL_7:
      v5 ^= (unsigned __int64)i[4] << 32;
LABEL_8:
      v5 ^= (unsigned __int64)i[3] << 24;
LABEL_9:
      v5 ^= (unsigned __int64)i[2] << 16;
LABEL_10:
      v5 ^= (unsigned __int64)i[1] << 8;
LABEL_11:
      v5 = 0xC6A4A7935BD1E995LL * (*i ^ v5);
      break;
    default:
      return ((0xC6A4A7935BD1E995LL * ((v5 >> 47) ^ v5)) >> 47) ^ (0xC6A4A7935BD1E995LL * ((v5 >> 47) ^ v5));
  }
  return ((0xC6A4A7935BD1E995LL * ((v5 >> 47) ^ v5)) >> 47) ^ (0xC6A4A7935BD1E995LL * ((v5 >> 47) ^ v5));
}
```
Увидев эту страшную крипто-функцию, я сразу же пошел гуглить константу `C6A4A7935BD1E995`, первой же ссылкой получил https://github.com/vitkyrka/kninja/blob/master/mmh2.py, чуть изучив этот код я пришел к выводу, что он очень схож с функцией `sub_401B1B`. Подав им на входы одинаковые данные (`sub_401B1B` и коду), я получил одинаковый результат, что лучше всего доказывает, что `sub_401B1B` есть MurmurHash2 (такое название было в ссылке выше).

Загуглив `MurmurHash2 reverse`, натыкаемся на любимый [сайт, stackoverflow](https://stackoverflow.com/questions/59976917/is-it-possible-to-reverse-a-murmurhash-in-python-with-mmh3). Хоть конкретно в этом вопросе и спрашивают про mmh3, а не про mmh2 во втором ответе есть полезная [ссылочка с инверсией разных алгоритмов](http://bitsquid.blogspot.com/2011/08/code-snippet-murmur-hash-inverse-pre.html). Воспользовавшись все тойже константой `C6A4A7935BD1E995`, найдем нужную функцию инверсии `murmur_hash_64_inverse`.

Чуть поигравшись, понастраивая и помучавшись с ендингами, получил [такой сишный скрипт](https://github.com/osogi/writeups/blob/main/ctfzone2021/Secure_db/inverse_murmurr.c), который получает в аргументах желаемую 8 битную строчку, и выдает 8 байт необходимых для генерации такой строчки. Потестил, работает, теперь пришло время вспомнить про `s="SELECT * FROM PatientList WHERE HashID = '{src}'"`, и тут я приисполнился в сознании, осознал (вспомнил), что sql это максимально дырявая технология. Но так как из меня вебер (ну или кто обычно эксплуатирует sql), как из рыбы топор (лучшей аналогии в 4 часа ночи я не придумал), то я попросил помощи у ~~пошлых одуванчиков~~ моей команды

![](https://i.imgur.com/R3BsPum.png)

Они мне предожили использовать `'or not'`, запихнем это в уже скомпилированный [код, что был разработан раньше](https://github.com/osogi/writeups/blob/main/ctfzone2021/Secure_db/inverse_murmurr.c)

![](https://i.imgur.com/FnNHYkT.png)

С первого раза не получилось, но потом я исправился. Теперь все что осталось это отослать эти байтики на сервер, не забыв про 'хедер', который будет равен `08` (так как байтов после него 8 штук). И так же стоит не забыть про греп, чтобы потом не искать флаг в куче мусора.

![](https://i.imgur.com/nL53rvv.png)
Ееее флаг!

## Решение 

1.  Найти набор функций отвечающий за обработку внесенных данных
2. Отреверсить их, понять, что: 
     - из первых двух байт формируется хедер, отвечающий за кол-во последующих байт
     - из оставшихся байт посредством функции sub_401B1B, формируется `src`
    - приложение делает запрос к бд, вида `"SELECT * FROM PatientList WHERE HashID = '{src}'"`
3. Понять, что `sub_401B1B` == MurmurHash2
4. Найти алгоритм инверсии для MurmurHash2
5. Найти значение `src` для эксплуатации `"SELECT * FROM PatientList WHERE HashID = '{src}'"` (один из вариантов `src`==`'or not'`)
6. С помощью шага 4 сгенерировать байты, которые преобразуются в нужное значение `src` ([пример скрипта](https://github.com/osogi/writeups/blob/main/ctfzone2021/Secure_db/inverse_murmurr.c))
7. Отправить нужные байты + хедер на сервер, получить флаг

## Эпилог

Он будет коротким, иметь умных сокомандников - круто! А вообще, вот такие смешаные таски они и вправду прикольные, понимаешь надобность команды.

А может и неочень коротким, вот написал я уже второй райтап на русском для ctf'а, где вроде участвует немало другого народа, незнающих русский язык. Тип вот будет им интересно решение таска, а у меня мало того, что используется свободный стиль (пытаюсь не писать сухо, оставлять коментарии), так и в русском языке есть ошибки.

In short, exuse me if you read my writeups by translater.
![](https://i.imgur.com/amfOYVJ.png)
<p style="text-align: center;">Using russian text on photo for apology, genius!</p>