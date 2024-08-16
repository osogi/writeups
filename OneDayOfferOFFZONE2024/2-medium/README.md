![preview](https://github.com/user-attachments/assets/c90aa73c-b9ae-49c6-9e55-0adbf41e8eed)

> Второе пришествие [@ch4nnel1](https://t.me/ch4nnel1)?

---
- [Research](#research)
- [Итерация 1: убираем делегатов](#итерация-1-убираем-делегатов)
  - [Анализ кода](#анализ-кода)
  - [Деобфускатор](#деобфускатор)
      - [Предисловие](#предисловие)
      - [Идея](#идея)
      - [launch2method](#launch2method)
      - [deobfuscateDelegateCalls](#deobfuscatedelegatecalls)
  - [Результат](#результат)
- [Итерация 2: Возвращаем читаемые константы](#итерация-2-возвращаем-читаемые-константы)
  - [Анализ кoда](#анализ-кoда)
  - [Деобфускатoр](#деобфускатoр)
      - [Идея](#идея-1)
      - [deobfuscateConstFromDict](#deobfuscateconstfromdict)
- [Финал](#финал)
- [Files](#files)


# Ликвидация посредников. Создаём свой деобфускатор с помощью PowerShell и dnlib
## Research

Скачаем и запустим [файл](./pon.exe) текущего задания. Нас попросят ввести флаг, попробуем угадать --- не получится.

```powershell
PS > .\pon.exe
Input Flag: coolflag
PLOH(N)
```

Проанализируем файл в DIE. Узнаем что `.NET` приложение. Следовательно, для дальнейшего анализа воспользуемся dnspy.

![die](https://github.com/user-attachments/assets/39de9908-6dbe-48ae-b8d6-388afa184093)


Открыв файл в dnspy, увидим страшные вещи: точка входа, как и многие функции в модуле заобфусцированы. 

```csharp
// Пример обфускации на "main" методе
private static void main(string[] args)
{
    pon_IPQUAUXMNR.pon_2OIJ6KTUQA(pon_JIJQE1HP2Q.pon_U9YZAE7UDH(1417757702, pon_JIJQE1HP2Q.pon_RVJXB43APH), pon_IPQUAUXMNR.pon_JC4AGZQDAQ);
    string text = pon_4XLUIBFYXR.pon_X4TWO3HGYZ(pon_4XLUIBFYXR.pon_HQW77REXTK);
    //  <...>
    pon_EAOACI1CWG.pon_KZ31C70X6U(true, pon_EAOACI1CWG.pon_0KSFJLNT0L);
}
```

Попробуем поверить в удачу и закинем таск в de4dot, но это не даст никаких результатов.

Про обфускацию пока можно сказать что имена всех методов и классов заменены на мусорные, большое по первоначальному анализу не определить. Давайте посмотрим структуру исследуемого модуля. Внутри у нас есть 2 namespace:
- Один без названия (далее будем называть его "-")
    - с множеством небольших классов (к их наполнению вернёмся позже).
- И второй "sharpon".
    - В "sharpon" два класса: 
        - Один с точкой входа, этот класс обзовём "mainPon" 
        - И второй с двумя статическими методами, назовём его "helpPon".

Того структура проекта выходит следующей:
```
pon.exe
|--- "-"
|     |--- pon_07FL9X0LJD
|     | <...>
|     |--- pon_ZW430KDN50
|
|--- "sharpon"
|     |--- mainPon
|     |     |--- main()
|     |     |--- pongo_66YVMK7M7G()
|     |     | <...>
|     |     |---  pongo_6Y6W2WOJSR()
|     |
|     |--- helpPon
|     |    |--- method1()
|     |    |--- method2()
```

## Итерация 1: убираем делегатов
### Анализ кода 

Изучать код классов из "sharpon" проблематично, так как единственная функция не подверженная обфускации (кроме пары функций выполняющих банальные битовые операции) это `helpPon.method1()`, но в ней происходят не самые тривиальные вещи (вызываются неизвестные методы, для которых нужно читать доку). 

Так что самое время посмотреть что там в "-", все классы в нём являются однотипными:
- наследуются от `MulticastDelegate`
- инициализация класса (метод `.cctor`) происходит с помощью `helpPon.method1()` 
- содержат статическое поле с экземпляром самого класса, обзовём его `instance`
- содержат единственный "осмысленный" метод `pon_*`, который применяет переданный экземпляр класса к переданным аргументам. Обзовём такие методы `launch()`. 
  - Так как пока известен только один экземпляр класса, и нигде не видно намёка на создание других экземпляров. Будем придерживаться гипотезы, что как экземпляр класса всегда передаётся уже известное статическое поле

```csharp
internal sealed class pon_0L0I8O9NRZ : MulticastDelegate
{
	public extern uint Invoke(uint, int);

	// "Осмысленный" метод, бывший pon_06W0JNA81N
	public static uint launch(uint A_0, int A_1, pon_0L0I8O9NRZ A_2)
	{
		return A_2(A_0, A_1);
	}

	// Инициализация класса
	static .cctor()
	{
		helpPon.method1(typeof(pon_0L0I8O9NRZ).TypeHandle);
	}

	public extern pon_0L0I8O9NRZ(object, IntPtr);

    // Cтатическое поле с экземпляром самого класса
	internal static pon_0L0I8O9NRZ instance;
}

```

Всё довольно понятно, кроме метода инициализация класса, давайте его и изучим.

 С помощью динамического анализа, можно примерно понять суть работы `helpPon.method1()`:
- получаем metadata token соответствующий `instance` текущего класса
- из файла ресурсов "realpongo_1" достаём `Dictionary<int, int>`, который сопоставляет: токен `instance` -> токену метода
- если метод статический:
    - создаём делегата указывающего на этот метод
    - сохраняем его в `instance` 
- если метод не статический
    - создаём промежуточный метода из IL кода, для корректной работы делегата
    - создаём и сохраняем получившийся делегат в `instance` 

Дальше будем ссылаться на метод `helpPon.method1()` как на `helpPon.initDelegate()`.

Теперь мы понимаем как работает одна из частей обфускации. Код вида:

```csharp
    pon_JXF858MNLL.launch(input, pon_JXF858MNLL.instance);
```

на самом деле запускает функцию, на которую ссылается `instance`, к остальным аргументам. 

То есть на деле этот код выглядит следующим образом:


```csharp
    coolClass.coolMethod(input);
```

А так как значение `instance` устанавливается единожды и больше не меняется. То можно узнать, на какую функцию он ссылается, и заменить все запуски `launch()` с ним на вызов этой функции.


### Деобфускатор

#### Предисловие
Каким-то образом выходим на этот [репозиторий посвящённый реверсу и малвари](https://github.com/Dump-GUY/Malware-analysis-and-Reverse-engineering/tree/main). В нём находим такую интересную [статью Deobfuscation of .NET using PowerShelling & dnlib](https://github.com/Dump-GUY/Malware-analysis-and-Reverse-engineering/blob/main/Deobfuscation%20of%20.NET%20using%20PowerShelling%20&%20dnlib%20-%20Eternity%20Malware/readme.md), где есть ссылка на 2-х часовой ютуб видос. Ютуб не работает :( Но кто-то загрузил этот видос на [рутуб](https://rutube.ru/video/7702ad049145d853abfbe9be5ec4decb/) :)

Изучаем по этому видео как пользоваться dnlib'ом без документации, и как в этом может помочь powershell.

> dnlib --- .NET module/assembly reader/writer library

> dnlib --- библиотека для работы со структурой .NET модулей. "Превращает" файл на компе (и не только) в класс, с кучей полей и методов.

#### Идея

- с помощью дебагера "сдампим" словарь из метода `helpPon.initDelegate()`
    - теперь по `instance` можем определять метод, на который он ссылается (`INSTANCE2METHOD[instance]`)
- через `this.instance` находим `this.launch()`
- при деобфускации метода находим в нём все вызовы `this.launch()` и заменяем их на `INSTANCE2METHOD[this.instance]`, параллельно исправляя работу с аргументами


#### launch2method

Для начала составим словарь сопоставляющий методу делегата `this.launch()` действительный метод `INSTANCE2METHOD[this.instance]`

```powershell
function init_launch2method(){
    # Итерируемся по всем instance токенам
    foreach ($instance_token in $TINSTANCE2TMETHOD.Keys)
    {
        # Определяем токен соответствующего метода
        $method_token = $TINSTANCE2TMETHOD[$instance_token]

        # По токенам получаем сам метод и instance
        $target_method = $mod.ResolveToken($method_token)
        $instance = $mod.ResolveToken($instance_token)

        # По instance находим ссылку на класс
        $class = $instance.FieldType.TypeDef
    
        # Среди методов класса ищем соответствкющий "launch"
        # Т.е. имя которого не Invoke, .cctor, .ctor
        foreach ($m in $class.Methods){
            if($m.Name -notmatch "^((Invoke)|(\.cctor)|(\.ctor))$"){
                
                # Заполняем таблицу сопоставляющую launch'у 
                # действительный метод
                $launch2method[$m]=$target_method
            }
        }
    }
}
```

#### deobfuscateDelegateCalls

Теперь пропатчим IL код обфусцированного метода. Кроме того, чтобы изменить вызов `launch` на `launch2method[launch]`, нужно ещё разобраться с аргументами. Но это не тяжело, аргументы в `launch2method[launch]` передаются в таком же порядке, как и в `launch`, а единственный "лишний" аргумент `instance` передаётся за одну инструкцию до вызова.

```csharp
	nop
	ldarg.0
	ldsfld    class pon_JXF858MNLL pon_JXF858MNLL::instance
	call      int32 pon_JXF858MNLL::launch(string, class pon_JXF858MNLL)
	stloc.0
	ldloc.0
	ret
```

```powershell
function deobfuscateDelegateCalls([dnlib.DotNet.MethodDef]$meth){
    # Получаем IL код метода
    $code = $meth.Body.Instructions

    # Идём по всему IL коду
    for($i = 0; $i -lt $code.Count; $i++){
        $cur = $code[$i]

        # Если операнд текущей инструкции один из launch'ей
        if($cur.Operand -in $launch2method.Keys){
            # Заменяем этот операнд на новый метод
            $cur.Operand = $launch2method[$cur.Operand]
            
            # Заменяем предыдущую инструкцию на NOP
            replaceInstruction ($code[$i-1]) $NOPI
        }
    }
}
```

### Результат

Применяем полученный деобфускатор ко всем методам из namespace "sharpon".

```csharp
// Пример функции после деобфускации
private static void main(string[] args)
{
    Console.Write(helpPon.method2(1417757702));
    string text = Console.ReadLine();
    // <...>
    if (flag)
    {
        Console.WriteLine(helpPon.method2(984420011));
    }
    else
    {
        Console.WriteLine(helpPon.method2(964919168));
    }
    Console.ReadKey(true);
}
```

## Итерация 2: Возвращаем читаемые константы
### Анализ кoда

Код стал гораздо красивее, но он везде заполнен вызовами `helpPon.method2()`. Так как этот метод мы тоже деобфусцировали, то давайте посмотрим что в нём происходит. 

```csharp
public static string method2(int num)
{
	BinaryFormatter binaryFormatter = new BinaryFormatter();
	BinaryReader binaryReader = new BinaryReader(typeof(helpPon).Assembly.GetManifestResourceStream("realpongo_2"));
	byte[] array = new byte[(int)binaryReader.BaseStream.Length];
	byte[] array2 = binaryReader.ReadBytes((int)binaryReader.BaseStream.Length);
	Array.Copy(array2, array, array.Length);
	Dictionary<int, string> dictionary = (Dictionary<int, string>)binaryFormatter.Deserialize(new MemoryStream(array));
	binaryReader.Close();
	return dictionary[num];
}
```

Угу. Этот метод:
- восстанавливает словарь `Dictionary<int, string>` из ресурса "realpongo_2" 
- и возвращает значение из словаря по полученному номеру

Значит это что-то вроде способа сокрытия констант, давайте вернём их назад.

### Деобфускатoр

#### Идея
- с помощью дебагера "дампим" словарь из `helpPon.method2()`
- по токену находим `helpPon.method2()`
- при деобфускации:
    - находим все вызовы `helpPon.method2()` 
    - определяем какое значение аргумента ему передаётся при каждом вызове и на основе этого выбираем нужную строку
    - заменяем вызов метода на значение строки


#### deobfuscateConstFromDict

Снова будем патчить IL код метода. Вообще ничего сложного, так как .NET использует стековую "vm", то результат функции просто складывается на стек. Тогда достаточно заменить вызов функции на "push" нужной строки, и желательно убрать лишние аргументы (число, которое `helpPon.method2()` получает на вход) со стека, чтобы не случилось казусов. Так же как и раньше очень удобно оказывается, что во всём файле аргумент для `helpPon.method2()` передаётся за две инструкции до вызова.


```csharp
		nop
		ldc.i4    1417757702
		nop
		call      string sharpon.helpPon::method2(int32)
		nop
		call      void [mscorlib]System.Console::Write(string)
```

```powershell
function deobfuscateConstFromDict([dnlib.DotNet.MethodDef]$meth){
    # Получаем IL код метода
    $code = $meth.Body.Instructions

    # Находим helpPon.method2()
    $dictFun = $mod.ResolveToken([uint32]0x06000010)

    # Идём по всему IL коду
    for($i = 0; $i -lt $code.Count; $i++){
        $cur = $code[$i]

        # Если операнд текущей инструкции helpPon.method2()
        if($cur.Operand -eq $dictFun){

            # Находим инструкцию отвеечающую за загрузку аргумента
            $putarg = $code[$i-2]
            # на основе её операнда определяем нужную строчку
            $str = $CONSTS[[uint32]$putarg.Operand]

            # Заменяем инструкцию загрузки аргумента на nop
            replaceInstruction $putarg $NOPI

            # Заменяем вызов функции на закрузку нужной строки
            replaceInstruction ($code[$i]) (createLdstr($str))            
        }
    }
}
```

## Финал 

После применения деобфускатора к файлу и переименовании функций, получаем такой результат (полный код `mainPon` после деобфускации можно посмотреть [здесь](./source.cs)):

```csharp
private static void main(string[] args)
{
    Console.Write("Input Flag: ");
    string text = Console.ReadLine();
    List<byte> list = Encoding.ASCII.GetBytes(text).ToList<byte>();
    uint num = mainPon.ToUInt32("322376503");
    string text2 = "";
    while (list.Count % mainPon.ToInt32("4") != mainPon.ToInt32("0"))
    {
        list.Add((byte)mainPon.ToInt32("0"));
    }
    byte[] array = list.ToArray();
    for (int i = mainPon.ToInt32("0"); i < list.Count; i += mainPon.ToInt32("4"))
    {
        uint num2 = mainPon.crypt_flag(BitConverter.ToUInt32(array, i), num);
        ushort num3 = (ushort)(num2 % 65536U);
        ushort num4 = (ushort)mainPon.bsr(num2, 16);
        text2 = text2 + mainPon.sha_hash(BitConverter.GetBytes(num3)) + mainPon.sha_hash(BitConverter.GetBytes(num4));
    }
    if (text2 == /*long sha string*/)
    {
        Console.WriteLine("HAROSH");
    }
    else
    {
        Console.WriteLine("PLOH(N)");
    }
    Console.ReadKey(true);
}
```


Чуть сидим, думаем над кодом и пишем [скрипт](./decrypt.py) для восстановления флага. Единственное что тут более менее интересное, это то что, так как sha256 генерится для short'ов, то мы можем перебрать их все и составить обратную таблицу. 

Вводим получившийся флаг в изначальную программу, она говорит что мы молодцы.

```
PS > .\pon.exe
Input Flag: ctfzone{d3l3g@t3d_m@lw@r3_t0_y0u,_ch3ck_7927c8b3f2860394fb6159c688f31625}
HAROSH
```

> Flag: `ctfzone{d3l3g@t3d_m@lw@r3_t0_y0u,_ch3ck_7927c8b3f2860394fb6159c688f31625}`

## Files
- [deobfuscate.ps1](./deobfuscate.ps1): деобфускатор
- [dnlib.dll](./dnlib.dll): библиотека для работы скрипта
- [decrypt.py](./decrypt.py): скрипт для восстановления флага
- [pon.exe](./pon.exe): оригинальный exe файл
- [pon-nice.exe](./pon-nice.exe): exe файл после деобфускации и переименования символов
- [source.cs](./source.cs): код основного класса после деобфускации и небольшого рефакторинга
