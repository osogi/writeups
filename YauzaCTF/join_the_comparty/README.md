---
tags: writeup, rev, yauzactf
---
# **Write-up Join the ComParty (YauzaCTF 2021)**
> [name=User1]
> [time=Tue, Aug 31, 2021 12:59 AM]


![](https://i.imgur.com/FkAfFM8.png)


> [color=#1cefda] W3lc0m3 t0 th3 [@ch4nnel1](https://t.me/ch4nnel1) L4meR! 

Original write-up: https://hackmd.io/@osogi/join_the_comparty_writeup


## История + разбор
![](https://i.imgur.com/jVzGMHS.png)

<p style="text-align: center;">Сам таск (https://game.yauzactf.com/challenges#Join%20the%20ComParty-29)</p>

### Мини-дисклеймер
Этот таск решал не только я со своей шизой, но еще и @rozetkinrobot, попробую отмечать моменты где он мне помогал. Но так же стоит учесть, что когда у тебя на другом конце провода сидит человек и слушает, наблюдает, что ты делаешь. Это дает +100 очков к концентрации, и так же ты можешь отдыхать, не смотря анимешечку/ютубчик, а наблюдая стрим, как чел пытается решить этот же таск, что тоже положительно сказывается на решении задачи.
**TL;DR** @rozetkinrobot посопутствовал решению, за что я ему благодарен.

### Пролог (реверсинг dotnet'а)
Качаем файлик, это инстолер, он предлагает выбрать папку куда, что сохранять и тд., классические операции инстолера.

![](https://i.imgur.com/BSZp384.png)

В папочке, куда мы устанавливали прогу, находим саму прогу и деинстолер.

![](https://i.imgur.com/e47qLbc.png)

Запусти прогу, услышим приятную мелодию и увидим следующее окошечко.

![](https://i.imgur.com/llHciNH.png)

Попробовав ввести пароль и флаг, он поругается, что пароль неверный. По инерции, засунем exe'шник в иду, откуда узнаем, что это вообще-то dotnet файл и его надо бы засунуть в dnspy или что-то подобное.

![](https://i.imgur.com/RfMUman.png)

В dnspy находим метод связанный с кнопкой `Button_Click`.
```csharp=1
private void Button_Click(object sender, RoutedEventArgs e)
    {
	string text = this.PasswordTextBox.Text;
	string text2 = this.FlagTextBox.Text;
	if (!PswGen.CheckPassword(text))
	{
		MessageBox.Show("Wrong password!");
		return;
	}
	switch (PswGen.CheckFlag(RC4.Encrypt(Convert.ToBase64String(Encoding.UTF8.GetBytes(text)), Convert.ToBase64String(Encoding.UTF8.GetBytes(text2)))))
	{
	case -1:
		MessageBox.Show("Can't write to registry!");
		return;
	case 0:
		MessageBox.Show("Wrong flag!");
		return;
	case 1:
		MessageBox.Show("Welcome, komrad!");
		return;
	default:
		return;
	}
    }
```

Как видно в нем просто проверяется пароль и флаг (флаг криптуется rc4), давайте заглянем глубже и изучим для начала функцию `PswGen.CheckPassword`. 

```csharp=1
public static bool CheckPassword(string password)
    {
        int num = (int)DateTime.UtcNow.Subtract(new DateTime(1970, 1, 1)).TotalSeconds;
        num /= 86400;
        string str = MethodBase.GetCurrentMethod().DeclaringType.ToString();
        MD5.Hash(num.ToString() + str).Substring(3, 6);
        return password == MD5.Hash(num.ToString() + str).Substring(3, 6);
    }
```

Тут можно посмотреть и понять, что происходит, но объяснять что и как мне лень. Так как видно, что пароль, который мы получили как аргумент, просто сравнивается с какой-то строчкой. Давайте отдебажим процесс и посмотрим, что это за строка (строчка меняется каждый день, так что вполне вероятно, что у вас она может отличаться).

![](https://i.imgur.com/pxqBB2f.png)

Попробуем запустить приложение просто (без дебага, так как мало ли что, и в нем были встроены анти-дебаг методы. И пароль который мы нашли окажется неверным). Вставим туда выуженный пароль (пока я писал райтап, сутки изменились и пароль тоже, так что он отличается от того что выше) и какой-то флаг

![](https://i.imgur.com/wrK3kVu.png)

Теперь он ругается на `wrong flag`, а не `wrong password` => пароль определили правильно. Идем дальше пойдем смотреть функцию проверки флага

```csharp=1
public static int CheckFlag(string text)
    {
        try
        {
            Registry.LocalMachine.OpenSubKey("Software\\DefinitelyUnnecessary\\Program\\Settings", true).SetValue("Password", "", RegistryValueKind.String);
            Thread.Sleep(200);
            Registry.LocalMachine.OpenSubKey("Software\\DefinitelyUnnecessary\\Program\\Settings", true).SetValue("Password", text, RegistryValueKind.String);
            Thread.Sleep(1000);
            if (((string)Registry.LocalMachine.OpenSubKey("Software\\DefinitelyUnnecessary\\Program\\Settings", true).GetValue("Output")).ToUpper().Contains("SUCCESS"))
            {
                return 1;
            }
        }
        catch (Exception)
        {
            try
            {
                Registry.LocalMachine.OpenSubKey("Software\\WOW6432Node\\DefinitelyUnnecessary\\Program\\Settings", true).SetValue("Password", "", RegistryValueKind.String);
                Thread.Sleep(200);
                Registry.LocalMachine.OpenSubKey("Software\\WOW6432Node\\DefinitelyUnnecessary\\Program\\Settings", true).SetValue("Password", text, RegistryValueKind.String);
                Thread.Sleep(1000);
                if (((string)Registry.LocalMachine.OpenSubKey("Software\\WOW6432Node\\DefinitelyUnnecessary\\Program\\Settings", true).GetValue("Output")).ToUpper().Contains("SUCCESS"))
                {
                    return 1;
                }
            }
            catch (Exception)
            {
                return -1;
            }
        }
        return 0;
    }
```

Так тут как видим, флаг (уже заэнкрипченный) кладется в регистр `Password` (не асм регистры, а виндовские, я не знаю как правильно описать данное явление, но в контексте данной задачи их можно считать просто спец. файлами). Потом проверяется состояние регистра `Output`, если там "SUCCESS", значит круто таск решён.

Откроем `regedit` - "штука для работы с регистрами винды", и поиграемся с регистрами "пароля" и "вывода". Поставим `Output` = SUCCESS ~~(сукес, хехе)~~, вроде все получилось.

![](https://i.imgur.com/QLx7iRK.png)

Но стоит перезайти в "папку" этих регистров, как он сразу поменялся обратно. 

![](https://i.imgur.com/jWOdLXQ.png)

Следовательно, что-то скорее всего проверяет `Password` и в зависимости от итога пишет разные данные в `Output`. На этом про dotnet можно забыть, ведь это работает даже если его отключить.

### Завязка (поиск "проверки" флага)

Попробуем отследить, что обращается к нашим регистрам с помощью procmon'а (полезная штука отслеживающая кучу всего, что происходит с виндой). Тут я чуть-чуть затупил при выставлении фильтров и долго не мог понять почему у меня даже `FlagChecker.exe` (dotnet'овский файл), будто не меняет регистры. Но потом я заметил, что в списке фильтров у меня были "забаненны"/скрыты все обращения к регистрам. Потом я починил это и все стало работать нормально.

![](https://i.imgur.com/2m2s2Hs.png)

Дальше добавим в фильтр условие, что путь должен начинаться с `HKLM\Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings`, запустим `FlagChecker.exe`, попросим изменить его флаг и что мы видим:

![](https://i.imgur.com/xMr5FdY.png)

А мы видим что какой-то svchost очень подозрительно обрабатывает нащи регистры реестра, но открыв его свойства мы обнаружим, что он подписан майкрософтом (вроде как это гарантирует, что ФАЙЛ (именно файл), выпущен из под пера n-ой компании и не был никем редачнут. Но это только мои соображения)

![](https://i.imgur.com/hdTw5MK.png)

Но это не должно было отводить от него глаза, ведь инжекты и всякие другие "мальварные" техники никто не отменял. Но я подумал, что организаторы ctf'а навряд ли бы стали так заморачиваться (что оказалось зря, ведь оказывалось что в svchost, как раз инжектился другой код). И стал искать, как можно задать поведение регистров в виндовс (тип я думал, что это можно как-то кастомизировать и потом svchost работает с каким-то вашим скриптом и обрабатывает по нему регистры), но ничего не найдя начал чуть тупить. 

И тут мне на помощь пришел Розеткин, я ему объяснил всю сложившуюся ситуацию и стал наблюдать, как он пихает приложухи на всякие сайты для отслеживания малвари и тд (узнал что в virustotal'е можно посмотреть, что примерно делает бинарь (какие создает файлы, как их запускает и тд) + открыл для себя кулл сайт https://app.any.run/ на котором прога трейсится прям прикольно и красиво). Фактически нам это ничего особо не дало, кроме того что подтолкнуло на идею того, что прога все-таки может инжектиться в процесс и все такое. Позже Розеткин догадался посмотреть с помощью process hacker'а (апгрейднутый диспетчер задач), какие приложения запускаются из под инстолера, и вот удивление в этом списке оказался и наш svchost, так же Розеткин сдампил его (уже с помощью process hacker nightly, так как обычный не мог) и отправил дамп мне.

![](https://i.imgur.com/OxjqRJB.png)

Открыв дамп в иде, мне несильно понравился ее "дебажный режим", так что я закинул его в rizin, и после завершения анализа выполнил парочку команд

```r
[0x004e2660]> izz~DefinitelyUnnecessary #выплюнуть все строчки из бинаря и grep'нуть  DefinitelyUnnecessary
3494   0x000ca03a 0x02c1beb0 59   120  Memory_Section_25  utf16le Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings
3495   0x000ca0b2 0x02c1bf28 47   96   Memory_Section_25  utf16le Software\DefinitelyUnnecessary\Program\Settings
3788   0x000d89ba 0x02c6e830 82   165  Memory_Section_26  utf16le \REGISTRY\MACHINE\Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings:\Wvx
4274   0x001397c2 0x02e06638 77   156  Memory_Section_38  utf16le \REGISTRY\MACHINE\Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings
4277   0x00139f0c 0x02e06d82 103  208  Memory_Section_38  utf16le \Registry\Mach\Registry\Ma\REGISTRY\MACHINE\Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings
4278   0x0013a00a 0x02e06e80 69   140  Memory_Section_38  utf16le Y\MACHINE\Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings
4280   0x0013a778 0x02e075ee 77   156  Memory_Section_38  utf16le \REGISTRY\MACHINE\Software\WOW6432Node\DefinitelyUnnecessary\Program\Settings
[0x004e2660]> axt @ 0x02c1beb0   #xref'ы на адрес 0x02c1beb0
fcn_02c01f50 0x2c01fa0 [DATA] push 0x2c1beb0
[0x004e2660]> pdf @ fcn_02c01f50 #дизасм функции fcn_02c01f50
```

По строчкам что использует функция не остается сомнений, что это та функция что нам нужна

![](https://i.imgur.com/LhfLzwc.png)


### Кульминация (реверсинг "проверки" флага)

Тут дальше я пытался прицепится rizin'ом к запущенной проге (тип сразу статичный и динамический анализ - круто), но он просто подыхал почему-то

![](https://i.imgur.com/ElDcBt5.png)

Сначала подумал, что прога имеет какой-то антидебаг, но нет. Ида вполне справлялась с ее отладкой, так что было решено продолжить в ней + псевдо-код, все красивенько. Но возникли некоторые проблемы, связаны они были с 
[ASLR](https://ru.wikipedia.org/wiki/ASLR). После небольшого доп. анализа было выяснено, что есть подозрительный rwx сегмент размером 144 kB,

![](https://i.imgur.com/486HexP.png)

в котором как раз находится наша функция по смещению `0x1f50`. Посмотрим в иде, что примерно в ней происходит (переменные как здесь, так и дальше будут переименованы для удобства).

```clike=1
  off_99602C(0, 1, aFlagcheckerins);
  if ( off_996028() != 183 )
  {
    v4 = off_996004[0];
    v5 = off_996018[0];
    while ( ((int (__stdcall *)(int, __int16 *, _DWORD, int, int *))v4)(-2147483646, aSoftwareWow643, 0, 983103, &v11)
         && ((int (__stdcall *)(int, __int16 *, _DWORD, int, int *))v4)(-2147483646, aSoftwareDefini, 0, 983103, &v11) )
      ((void (__stdcall *)(int))v5)(100);
    v6 = off_99601C;
    v7 = off_996000[0];
    while ( 1 )
    {
      while ( 1 )
      {
        v8 = ((int (__stdcall *)(_DWORD, int, _DWORD, _DWORD))v6)(0, 1, 0, 0);
        off_99600C(v11, 1, 15, v8, 1);
        off_996020(v8, -1);
        v10 = 2048;
        if ( !off_996008(v11, aPassword, 0, 0, input_widechar, &v10) )
          break;
        v20 = dword_99BFE0;
        v19 = qword_99BFD8;
        v21 = word_99BFE4;
        ((void (__stdcall *)(int, __int16 *, _DWORD, int, __int64 *, unsigned int))v7)(
          v11,
          aOutput,
          0,
          1,
          &v19,
          2 * wcslen((const unsigned __int16 *)&v19));
      }
      if ( wcslen(input_widechar) )
      {
        sub_987E81((int)input_ascii, (int)input_widechar, 1024);
        if ( mega_func(input_ascii) )
        {
          v17 = dword_99BFD0;
          v16 = xmmword_99BFC0;
          v18 = word_99BFD4;
          ((void (__stdcall *)(int, __int16 *, _DWORD, int, __int128 *, unsigned int))v7)(
            v11,
            aOutput,
            0,
            1,
            &v16,
            2 * wcslen((const unsigned __int16 *)&v16));
        }
        else
        {
          v15 = word_99BFAC;
          v14 = xmmword_99BF9C;
          ((void (__stdcall *)(int, __int16 *, _DWORD, int, __int128 *, unsigned int))v7)(
            v11,
            aOutput,
            0,
            1,
            &v14,
            2 * wcslen((const unsigned __int16 *)&v14));
        }
      }
    }
  }
  return -1;
}
```
Судя из строчек 19, 32 и 34: `input_widechar` - это переменная в которую помещается, то что находится в регистре `Password`, проверим это с помощью дебага

![](https://i.imgur.com/P5JbMp1.png)

Как раз с помощью него узнаем, что изначально наша строка хранится в widechar (использует 2 байта на символ). Дебажим дальше ведь функции внутри не особо понятны и какие-то страшные, теперь попробуем узнать, что делает функция `sub_987E81` и что будет храниться в `input_ascii`

![](https://i.imgur.com/jVTMCWf.png)

И судя по всему `sub_987E81` конвертит widechar стринги в обычные сишные. А вот дальше идет `mega_func`, в которой и происходит проверка пароля/флага, я подробно? Пошагово разбирать ее не буду, только примерно опишу ее ход, и все это было выяснено с помощью дебага и логики.

```python=1
def mega_func(base64Pass):
    deBase64 = decryptB64(base64Pass)
    md5Hash = encryptMd5(GetCurrentDay() + "FlagChecker.Crypto.PswGen")
    passForFlag = md5Hash[3:9]
    flag = decryptRC4(deBase64, passForFlag) #в программе реализовано довольно страно, часть rc4 происходит внутри новой функции, и еще часть - в виде отдельного цикла в mega_func()
    encrypt_flag = custom_crypt(flag) #в оригинале это не функция, но будем рассматривать ее как таковую
    some_const=b"\xf8\x25\xb0\xd4[...]xbf\x43\x62\xec"
    if(encrypt_flag == some_const):
        return 0;
    else:
        return 1;
```

Так а теперь внимательней посмотрим, что из себя представляет custom_crypt() (кстати, возможно и не кастом, но я ничего не смог найти по константам). Псевдо код от хексрейс в студию

```clike=1
v25 = 0;
  qmemcpy(&v63, "is program canno", sizeof(v63));
  HIDWORD(v65) = flag_len >> 3;
  v54 = 0;
  if ( flag_len >> 3 )
  {
    do
    {
      block2 = *(_DWORD *)(flag + 8 * v25);
      v55 = flag + 8 * v25;
      v57 = 64;
      v48 = (unsigned int *)(flag + 8 * v25 + 4);
      spec_counter = 0;
      block1 = *v48;
      do
      {
        hashik = spec_counter + *((_DWORD *)&v63 + (spec_counter & 3));
        spec_counter -= 0x67452302;
        block2 += hashik ^ (block1 + ((16 * block1) ^ (block1 >> 5)));
        block1 += (spec_counter + *((_DWORD *)&v63 + ((spec_counter >> 11) & 3))) ^ (block2 + ((16 * block2) ^ (block2 >> 5)));
        --v57;
      }
      while ( v57 );
      *(_DWORD *)v55 = block2;
      *v48 = block1;
      v25 = v54 + 1;
      v54 = v25;
    }
    while ( v25 < HIDWORD(v65) );
  }
```

А теперь, чуть подумав преобразуем его к такому питоновскому коду

```python=1
def uint32(a):
    return a & 0xffffffff

def opd(a):
    res=a
    res+=(a<<4)^(a>>5)
    return res & 0xffffffff
    
def crypt_blocks(block1, block2):
    con = 0x67452302
    key=[int.from_bytes(b"is p", "little") ,
            int.from_bytes(b"rogr", "little") ,
            int.from_bytes(b"am c", "little") ,
            int.from_bytes(b"anno", "little")]
    counter=0
    for i in range(64):
        hashik=uint32(counter + key[counter%4])
        counter=uint32(counter - con)
        block1=uint32(block1+(hashik^opd(block2)))
        
        hashik=uint32(counter + key[(counter>>11)%4])
        block2=uint32(block2+(hashik^opd(block1)))
    return block1, block2

def custom_crypt(flag):
    res=b""
    for i in range(len(flag)//8):
        l=i*8
        block1=int.from_bytes(flag[l:l+4], "little")
        block2=int.from_bytes(flag[l+4:l+8], "little")
        block1, block2 = crypt_blocks(block1, block2)
        res+=long_to_bytes(block1)[::-1]+long_to_bytes(block2)[::-1]
    return res
```

### Развязка (кодинг solver'а)

Тут произошел большой затуп с моей стороны, я почему-то решил, что мне по результату функции `opd()` нужно было понять какой аргумент был ей дан. И там одно за другим потянулось и свелось к тому, что я воспользовался z3. Самое что прикольное - скрипт на z3 мог раскриптить 2-3 раунда, почти без проблем, но вот на 4 тупил очень долго (я так и не дождался и вырубил его), так что про 64 раунда и слов идти не могло. Воть... На работу с z3 я потратил около семи часов. 

А потом Розеткин (он наблюдал за моими мучениями с z3 и подсказывал мне ощибки) понял, что я его где-то конкретно обманываю и попытался донести это до меня. Я понял как жёстко затупил, ведь нам известны block1 и block2 (так как мы знаем чему должен равняться encrypt_flag), а hashik мы можем посчитать, так как знаем чему будет равняться counter (который мы знаем за счет номера итерации). И быстренько переписал верхний код encrypt'а в decrypt и получил такой скриптец.

#### Solver script
```python=1
from Crypto.Util.number import long_to_bytes

def uint32(a):
    return a & 0xffffffff


def opd(a):
    res=a
    res+=(a<<4)^(a>>5)
    return res & 0xffffffff

# iter - номер штуки начинается с 1 заканчивается 65
def get_hash(iter, num):
    key=[int.from_bytes(b"is p", "little") ,
        int.from_bytes(b"rogr", "little") ,
        int.from_bytes(b"am c", "little") ,
        int.from_bytes(b"anno", "little")]
    con = 0x67452302
    counter= uint32(-con*(iter+num-2))
    hashik=uint32(counter + key[(counter>>11*(num-1))%4])
    return hashik

blocks = [
    0x0D4B025F8,
    0x3863D8BF,
    0x0D326B2DB,
    0x0FEFE2FDE,
    0x387C477A,
    0x0EA4F0B03,
    0x25C46283,
    0x5CEE3876,
    0x62C674E1,
    0xEC6243BF,
    ]
    
for i in range(len(blocks)//2):
    block1=blocks[2*i]
    block2=blocks[2*i+1]


    for i in range(64, 0, -1):
        hashik=get_hash(i, 2)
        block2=uint32(block2-(hashik^opd(block1)))
        hashik=get_hash(i, 1)
        block1=uint32(block1-(hashik^opd(block2)))

    print(str(long_to_bytes(block1)[::-1]+long_to_bytes(block2)[::-1])[2:-1], end='')
print()
```

## Решение 

- установить FlagChecker 
- разреверсить dotnet файл
    - узнать верный пароль
    - понять что флаг проверяется через регистры
- найти что регистры меняет svchost
- найти в нем проверку флага
- разреверсить проверку флага (в том числе и крипт метод)
- написать скрипт для декрипта ([пример](#Solver-script))
- раздекриптить значение, которому должен равняться энкрипченный флаг
- получить флаг

## Эпилог

Так, тут я хотел попробовать взять и разреверсить бинарь, чтобы посмотреть что и откуда он инжектит в svchost, но мне стало лень после 5 минут и я забил. Так что эпилога нет, можно сказать открытый конец...


![](https://nerdist.com/wp-content/uploads/2018/08/Top_081618.gif)

