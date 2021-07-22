---
tags: writeup, hardware, z3, googlectf2021
---
# **Write-up Parking (Google CTF 2021)**
> [name=User1]
> [time=Thu, Jul 22, 2021 7:37 PM]

![](https://i.imgur.com/JoucNBT.jpg)


> [color=#1cefda] Вы выиграли @@@@@@@@@@@@[@ch4nnel1](https://t.me/ch4nnel1).

Original write-up: https://hackmd.io/@osogi/parking_writeup
Russian version: https://hackmd.io/@osogi/parking_writeup_ru


## История + разбор
![](https://i.imgur.com/esugZLZ.png)
<p style="text-align: center;">Сам таск (https://capturetheflag.withgoogle.com/challenges/hw-parking или https://github.com/google/google-ctf/tree/master/2021/quals/hw-parking)</p>

### Пролог
Качаем зип архив, открываем, в нем 4 файла

![](https://i.imgur.com/9XGq4Zc.png)

Про `run.sh` забудем почти сразу, так как все что он делает, это запускает игру вместе с первым уровнем и говорит ей, что сейчас не надо давать флаг + просто запускает второй уровень. (Надеюсь гугл не забанит меня за то, что я исползую код из их таска. Там просто была какая-то лицензия всё такое, а я ленивый не прочитал).
```bash=1
echo "Welcome to our game. You need to enter the red vehicle into a crowded parking lot."
echo "There are two levels: the first is introductory, just so you know the rules; the second, when solved, will reveal the flag. Good luck!"
FLAG=0 python3 game.py level1
echo "Good job! Now that you know the rules, let's move to the real level. Note that it is quite large, so allow some loading time."
python3 game.py level2
```
Ну и само собой, запустим прогу, для начала с первым уровнем, увидим что-то такое

![](https://i.imgur.com/VbMdLHx.png)

Из текста, что вещает нам игра понимаем, что нужно сдвинуть красную машинку + что конечное положение зеленой машинки (как окажется позже машинок) кодирует флаг. Потратив пять минут, с трудом, но решаем первый уровень (подсказки как его решить я не дам). Видим следующее, мы получили флаг. 

![](https://i.imgur.com/MkHhcBC.png)

~~Окей гугл, я ожидал от тебя чего-то большего. Я конечно знаю, что гений, но решить таск за 5 минут.~~ Ааа, флаг не подходит.

![](https://i.imgur.com/qKBYXL0.png)


### Завязка (идея решения)

Окей, меня внаглую обманули и подсунули неверный флаг. Давайте попробуем открыть второй уровень. Чет он долговато грузиться...

![](https://i.imgur.com/YrMtxzi.png)

~~Твою же мать, можно я выйду в окно? После этого автора райтапа никто не видел~~ Ладно, может там не все так плохо, и рядом с красной машинкой уже есть пустая клеточка? (После того как не смог найти красную машинку) Таким бы вопросом задался каждый нормальный человек, но я мега реверсер сразу полез в исходники игры. 

Тут могла бы быть часть про то, что я тупил и пытаясь менять координаты одной зеленой машины получить флаг. Но это не будет относиться к текущему решению, так что просто упомяну это. 

Чуть проанализировав код + потыкав файлы уровней. 

![](https://i.imgur.com/IUUULon.png)

Примерно поймем структуру последних: первая строка размер поля, дальше идут объекты в формате `x, y, ширина, высота, тип` (если тип: 0 - стена; 1 - обычная, серая машина; -1 - зеленая машина; -2 - красная машина).
Игра сама написана на питоне и в ней вполне все понятно, но отмечу некоторые полезные штуки, которые будут использоваться при дальнейшем решении. 

```python
board #словарь сопоставляющий: (x, y) -> объект находящийся на этой позиции
blocks #список сопоставляющий "номер", индекс машины с её x,y,ширина,высота,тип
target #переменная содержащая номер красной машины
def redraw(): #функция отвечающая за перерисовку поля + при первом прогоне сохраняет результат начальной позиции в файл
def printflag(): #функция печатающая флаг, вывод зависит от позиции зеленых машинок (если они меняли позицию, то бит флага соответствующий этой машинке будет 1, иначе 0)
```

Зная как устроена структура файла уровня, найдем на рисунке красную и зеленные машинки

![](https://i.imgur.com/Nwpzh7n.png)
<p style="text-align: center;">В зеленных секциях (всего их 64) находятся зеленные машинки, туда куда указывает красная стрелочка (правый нижний угол) красная машина</p>

Приблизим чтобы рассмотреть это все поближе

![](https://i.imgur.com/hFaKgaE.png)

![](https://i.imgur.com/J6TFqN1.png)

![](https://i.imgur.com/QPgNgZI.png)

Окей, знаете что? Это все напоминает логические схемы (+ hw как-никак). Из видео liveoverflow помнил, что их можно решать через z3 (мега решалка уравнений, неравенств и тд), но как точно не знал, так что решил пересмотреть его.

{%youtube nI8Q1bqT8QU %}
> https://www.youtube.com/watch?v=nI8Q1bqT8QU

P.S. В результате им даже не воспользовался, но видео все-равно прикольное, полезное. Кстати оно тоже с hardware Google ctf, только 2019.


#### Мини-задача 1
Давайте рассмотрим простейшие задачки с машинками и поймем наши действия.

![](https://i.imgur.com/8ExCG8a.png)

Нам нужно подвинуть красную машинку, для этого мы:
1) определяем ее возможные направления движения (горизонтально/вертикально).
2) куда она может продвинуться в теории, то есть где ей мешает двигаться не стена.
3) если туда куда она хочет подвинуться есть, машина повторить все для нее.

Используя такой алгоритм, мы сможем решить эту задачку.

![](https://i.imgur.com/FtAdqho.png)

#### Мини-задача 2

Давайте рассмотрим задачу посложнее

![](https://i.imgur.com/C0pd3RB.png)

Для нее нам придется чуть доработать наш алгоритм, ведь если мы просто подвинем оранжевую машинку вправо (как и должно быть по прошлому алгоритму), то она не освободит путь. Следовательно добавим в наш алгоритм 1 шаг 
1) определяем ее возможные направления движения (горизонтально/вертикально).
2) куда она может продвинуться в теории, то есть где ей мешает двигаться не стена.
3) **посмотреть что мешает продвинуться в "возможную позицию".**
4) **повторить для мешающих машин, передав им позицию которую нужно освободить.**

![](https://i.imgur.com/SrGq1h9.png)

#### Мини-задача 3

И рассмотрим последний пример, в котором обговорим одну тонкость алгоритма.

![](https://i.imgur.com/T1LuWX1.png)

В 4 шаге мы должны сделать логическое сложение ('или', 'дизъюнкция', '|') для всех результатов возможных позиций (в этом таске будет только 1-2). И логическое умножение ('и', 'конъюнкция', '&') внутри самих результатов. 

> Легче объяснить на примере. Допустим в мини-задаче 2, чтобы подвинуть красную машинку нам в результате надо подвинуть синюю **И** фиолетовую машинку.  Тут же (мини-задача 3), чтобы подвинуть красную машинку нам нужно подвинуть зеленую **ИЛИ** голубую.

![](https://i.imgur.com/jsToXce.png)

Уххх, осталось только закодить это и скормить z3.

### Кульминация (кодинг)

>  В этой части опущены большинство моих косяков, так как их было слишком много.

#### some_foo()
Для начала сделаю функцию `some_foo()`, и помещу ее вызов перед рисованием изначального поля.

```python
def some_foo():
    print("Yep i'm work")
    print(blocks[target])
```

Бум, оно работает.

![](https://i.imgur.com/0Th8v7A.png)

#### get_obj(x, y)
Добавим:
- функцию `get_obj(x, y)`, которая будет возвращать, что находится по позиции `x, y` 
- функцию определяющую (`isHorizintal(which)`), как может двигаться машинка (горизонтально/вертикально)
- список `important`, который будет содержать индексы зеленых машинок. И кусочек кода, который будет его заполнять
```python
SPACE=-2
important = []

def get_obj(x, y):
    if (x,y) in board:
        return board[(x, y)]
    else:
        return SPACE
        
def isHorizintal(which): #horiz==x vertical==y
    h=blocks[which][3]
    if(h == 1):
        return True
    else:
        return False
        
def some_foo():
    print("Yep i'm work") 
    for i in important:
        print(isHorizintal(i))
    print(getObj(3, 5))
    
[...]
walls = []
for line in boardfile.splitlines():
    if not line.strip(): continue
    x,y,w,h,movable = [int(x) for x in line.split()]
    if movable == -1:
        important.append(len(blocks)) #add
        flagblocks[len(blocks)] = (x,y)
[...]
```
Выглядит рабочим

![](https://i.imgur.com/vlibuvy.png)
#### getMoveDir
> Тут было потрачено много попыток добавить/изменить функцию `getMoveDir()`, но сделаем вид, что она была правильно написана с первого раза
Давайте добавим чуть спагети кода (а окей, это не спагети код, но все-равно плохой) и функцию, которая "протестит" наш алгоритм 

- `getMoveDir(which, free_x, free_y)`  - рекурсивная функция, определяющая куда может двинуться машина
- `MvDict` - словарь содержащий "индекс машины" -> направление
- `MyTestBlocks` - список содержащий номера "спарщенных" машин
- `putparsecar(x,y,w,h,m)` - функция закрасывающая машины из`MyTestBlocks` в синий цвет, сделана чтобы понимать, где ломается `getMoveDir()`
- еще также было добавлено `sys.setrecursionlimit(150000)`, чтобы питон не ломался при анализе второго уровня
```python
import sys
sys.setrecursionlimit(150000)

MvDict={}
MyTestBlocks=[]
def getMoveDir(which, free_x, free_y):
    x,y,w,h,mv=blocks[which]
    res=""
    if(isHorizintal(which)):
        check=1
        buf=[]
        for i in range(free_x+1, free_x+w+1):
            obj=getObj(i, y)
            if(obj != which):
                if((obj==WALL)):
                    check=0
                    break
                elif(obj!=SPACE):
                    buf.append([obj, i, y])
        if(check):
            res+="Right"
            for l in buf:
                getMoveDir(l[0], l[1], l[2])
        check=1
        buf=[]
        for i in range(free_x-w, free_x):
            obj=getObj(i, y)
            if(obj != which):
                if((obj==WALL)):
                    check=0
                    break
                elif(obj!=SPACE) :
                    buf.append([obj, i, y])
        if(check):
            res+="Left"
            for l in buf:
                getMoveDir(l[0], l[1], l[2])
    else:
        check=1
        buf=[]
        for i in range(free_y+1, free_y+h+1):
            obj=getObj(x, i)
            if(obj != which):
                if((obj==WALL)):
                    check=0
                    break
                elif(obj!=SPACE) :
                    buf.append([obj, x, i])
        if(check):
            res+="Down"
            for l in buf:
                getMoveDir(l[0], l[1], l[2])
        check=1
        buf=[]
        for i in range(free_y-h, free_y):
            obj=getObj(x, i)
            if(obj != which):
                if((obj==WALL)):
                    check=0
                    break
                elif(obj!=SPACE) :
                    buf.append([obj, x, i])
        if(check):
            res+="Up"
            for l in buf:
                getMoveDir(l[0], l[1], l[2])
    print(f"{which}: {blocks[which]}| {free_x}, {free_y} |{res}")
    return res

def putparsecar(x,y,w,h,m):
    x0 = x*SZ
    x1 = (x+w)*SZ
    y0 = y*SZ
    y1 = (y+h)*SZ
    color = (0, 0, 128)
    draw.rectangle((x0+1,y0+1,x1-2,y1-2), fill=color)


def some_foo():
    print("Yep i'm work") 
    x,y,w,h,mv=blocks[target]
    print(getMoveDir(target, x+w-1, y))
    

def redraw():
    [...]
    for block in blocks:
        putcar(*block)
    for block in MyTestBlocks: #add
        putparsecar(*block)
    [...]
```
Проверка на 1 уровне:

![](https://i.imgur.com/07RhMHm.png)

Проверка на 2 уровне (пришлось перенести в файл, для более удобного анализа), дошел до всех (64) зеленым машин - успех:

![](https://i.imgur.com/DIJCY5l.png)

#### getBranch
> Тут тоже было потрачено много попыток для нормальной работы функции `getBranch()`, я же упояну только одну из них.

Так теперь нужно переписать функцию `getMoveDir()` в `getBranch()`, изменив ее функционал на то, чтоб получить при каких условиях (относительно зеленых машин) машина может освободить заданые координаты.

Для этого было добавлено:
- `inputs` - список с переменными (связанными с зелеными машинками) для z3
- `inp_dict` - словарь, который связывает "индекс зеленой машинки" -> соответствующей ей `inputs[i]`
- `BranchDict` - замена `MvDict` + была бага. Чтобы устранять бесконечные рекурсии, я запретил находить новые направления для машинок, кооторые есть в `MvDict` -> (см рисунок) когда фиолетовая машинка говорила синей сдвинуться, она "блокировала" к ней доступ. И желтая машинка считала, что синяя==стена. И как следствие `BranchDict` сопоставляет не просто `which`, а `f"{which}_{free_x}_{free_y}"` с результатом `getBranch(which, free_x, free_y)` 
![](https://i.imgur.com/XjINgsA.png)
- функция `get_green_by_head()` и костыль `GREENHEAD` - им помечается квадрат выше зеленной машинки, чтобы понять, что машинка сможет сдвинуться в него, только если зеленая не будет двигаться
- функция `parseBranch` применяется к ветке и если там есть несколько условий, соеденяет их с помощью AND
```python
from z3 import *
s=Solver()
c_OR="Or("
c_NOT="Not("
c_AND="And("
GREENHEAD=-3

BranchDict={}#MvDict={}
inp_dict={}
inputs=[]

def get_green_by_head(x, y):
    return getObj(x, y+1)
    
def parseBranch(branch):
    if(len(branch)>0):
        if(len(branch)==1):
            res=branch[0]
        else:
            res=c_AND
            for s in branch:
               res+=s+", "
            res=res[0:-2]+")" 
    else:
        res=""
    return res

def getBranch(which, free_x, free_y):
    res=""
    #GREENHEAD
    if(which==GREENHEAD):
        res= c_NOT+getBranch( get_green_by_head(free_x,free_y),free_x,free_y)+")"
    else:
        x,y,w,h,mv=blocks[which]
        #Gree car
        if(mv==-1):
            if(inp_dict.get(which)==None):
                inputs.append(Bool('inputs[%s]' % len(inputs)))
                inp_dict.update({which: f"inputs[{len(inputs)-1}]"})
            res = inp_dict[which]
        #common car
        else:
            dictbuf=f"{which}_{free_x}_{free_y}"
            if(BranchDict.get(dictbuf)!=None):
                res=BranchDict.get(dictbuf)
                if(res==""):
                    res="False"
            else:
                branches=[[], []]
                BranchDict.update({dictbuf:res})
                if(isHorizintal(which)):
                    check=1
                    buf=[]
                    for i in range(free_x+1, free_x+w+1):
                        obj=getObj(i, y)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE):
                                buf.append([obj, i, y])
                    if(check):
                        for l in buf:
                            branches[0].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[0].append("True")
                    check=1
                    buf=[]
                    for i in range(free_x-w, free_x):
                        obj=getObj(i, y)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE) :
                                buf.append([obj, i, y])
                    if(check):
                        for l in buf:
                            branches[1].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[1].append("True")
                else:
                    check=1
                    buf=[]
                    for i in range(free_y+1, free_y+h+1):
                        obj=getObj(x, i)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE) :
                                buf.append([obj, x, i])
                    if(check):
                        for l in buf:
                            branches[0].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[0].append("True")
                    check=1
                    buf=[]
                    for i in range(free_y-h, free_y):
                        obj=getObj(x, i)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE) :
                                buf.append([obj, x, i])
                    if(check):
                        for l in buf:
                            branches[1].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[1].append("True")
                print(f"{which}: {blocks[which]}| {branches[0]}, {branches[1]}")
                if((len(branches[0])+len(branches[1]))==0):
                    res="False"
                elif (len(branches[0])>0 and len(branches[1])>0):
                    res+=c_OR
                    res+=parseBranch(branches[0])+","
                    res+=parseBranch(branches[1])+")"
                else:
                    for i in range(2):
                        res+=parseBranch(branches[i])

                MyTestBlocks.append(blocks[which])
                BranchDict.update({dictbuf:res})
    return res
    
def some_foo():
    
    print("Yep i'm work")
    x,y,w,h,mv=blocks[target]
    #print(getMoveDir(target, x+w-1, y))
    qua=getBranch(target, x+w-1, y)
    s.add(eval(qua))
    print(s.check())
    print("Bits of flag:")
    flag=""
    for ind in important:
        var=eval(inp_dict[ind])
        bit=str(int(s.model()[var].__bool__()))
        print(bit, end="")
    print()

walls = []
for line in boardfile.splitlines():
    [...]
    if movable==-1: #add
        board[(x,y-1)] = GREENHEAD
    if movable:
        blocks.append([x,y,w,h, movable])
    [...]
```
Юху, оно смогло решить первый уровень:

![](https://i.imgur.com/8B7YvGl.png)

А вот со вторым, происходят некоторые проблемы (конечно если сначала Ваш комп не сдох от кол-ва мусора, которого выкидывает скрипт):

![](https://i.imgur.com/2bP4VDA.png)



#### z3.simplify()

Хорошо, возможно это стоило сделать сразу, но я додумался только сейчас, это добавить simplify(). Что бы сразу разбираться со всем. Так же были добавлены два условия на `True`, `False`, так как иначе оно ломалось.
```python

def getBranch(which, free_x, free_y):
    [...]
    if(res!="True") and (res!="False"):
        return str(simplify(eval(res)))
    return res
```

**...**
Я просто промолчу, и отмечу, что теперь прога падает из-за того, что eval обрезает длиные строчки (~10k символов) с помощью `...` и z3 пытаясь распарсить ломается. 

![](https://i.imgur.com/pYIFzrT.png)

#### main_dots

В результате было решено завести такую штуку как список контрольных точек `main_dots`, и записывать туда z3 переменные, которые бы равнялись формулам, длина которых больше 1000.

```python
def getBranch(which, free_x, free_y):
    [...]
                if(len(res)>1000):
                    ln=len(main_dots)
                    main_dots.append(Bool('main_dots[%s]' % ln))
                    s.add(eval(f'main_dots[{ln}]=={res}'))
                    res=f'main_dots[{ln}]'
                MyTestBlocks.append(blocks[which])
                BranchDict.update({dictbuf:res})
    if(res!="True") and (res!="False"):
        return str(simplify(eval(res)))
    return res
```
Запускаем его, ждем и получаем нужные битики (`0100110000101010000101101010011001000010010011101000110010110110`).

![](https://i.imgur.com/A0ewZ9M.png)

### Развязка (получение флага)

Заходим на [cyberchef](https://gchq.github.io/CyberChef/). Пихаем битики...

![](https://i.imgur.com/yot5z5u.png)

И не получаем флаг. ~~Все, закончили упражнение.~~ Начинаем реверсить 

![](https://i.imgur.com/nw1FWLN.png)

Ооо, понятные буковки, только бесмысленные, но ладно попробуем сдать CTF{m1rBehT2}.

![](https://i.imgur.com/JXAzGvo.png)

Не сдается( Попробуем реверсить дальше

![](https://i.imgur.com/D7dnzoP.png)

Угу, звучит и выглядит круто, сдаем CTF{2TheBr1m}

![](https://i.imgur.com/1efPfPY.png)

~~Easy game, easy life~~ Easy flag, hard CTF

## Решение 

- понять, что никак нельзя схалтурить и что нужно решить level2
- понять, что level2 похож на логическую схему -> можно засунуть в z3
- преобразовать level2 в логическую схему
- засунуть её в z3
- получить биты флага -> получить флаг

## Скрипт-решение

Использование: `python3 solver.py level2` и пойти попить чаек (ждать около 10-20 минут или больше).
![](https://i.imgur.com/zG5HEzW.png)

```python=1
from z3 import *
import sys
sys.setrecursionlimit(150000)

WALL = -1
SPACE=-2
GREENHEAD=-3

blocks = []
board = {}

s=Solver()
c_OR="Or("
c_NOT="Not("
c_AND="And("


BranchDict={}
inp_dict={}
inputs=[]
MyTestBlocks=[]
main_dots=[]
important = []

def Myprintflag(bits):
    flag = b"CTF{"
    while bits:
        byte, bits = bits[:8], bits[8:]
        flag += bytes([ int(byte[::-1], 2) ])
    flag += b"}"
    print(flag)

def getObj(x, y):
    if (x,y) in board:
        return board[(x, y)]
    else:
        return SPACE
        
def isHorizintal(which): #horiz==x vertical==y
    h=blocks[which][3]
    if(h == 1):
        return True
    else:
        return False

def get_green_by_head(x, y):
    return getObj(x, y+1)

def parseBranch(branch):
    if(len(branch)>0):
        if(len(branch)==1):
            res=branch[0]
        else:
            res=c_AND
            for s in branch:
               res+=s+", "
            res=res[0:-2]+")" 
    else:
        res=""
    return res


def getBranch(which, free_x, free_y):
    res=""
    #GREENHEAD
    if(which==GREENHEAD):
        res= c_NOT+getBranch( get_green_by_head(free_x,free_y),free_x,free_y)+")"
    else:
        x,y,w,h,mv=blocks[which]
        #Gree car
        if(mv==-1):
            if(inp_dict.get(which)==None):
                inputs.append(Bool('inputs[%s]' % len(inputs)))
                inp_dict.update({which: f"inputs[{len(inputs)-1}]"})
            res = inp_dict[which]
        #common car
        else:
            dictbuf=f"{which}_{free_x}_{free_y}"
            if(BranchDict.get(dictbuf)!=None):
                res=BranchDict.get(dictbuf)
                if(res==""):
                    res="False"
            else:
                branches=[[], []]
                BranchDict.update({dictbuf:res})
                if(isHorizintal(which)):
                    check=1
                    buf=[]
                    for i in range(free_x+1, free_x+w+1):
                        obj=getObj(i, y)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE):
                                buf.append([obj, i, y])
                    if(check):
                        for l in buf:
                            branches[0].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[0].append("True")
                    check=1
                    buf=[]
                    for i in range(free_x-w, free_x):
                        obj=getObj(i, y)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE) :
                                buf.append([obj, i, y])
                    if(check):
                        for l in buf:
                            branches[1].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[1].append("True")
                else:
                    check=1
                    buf=[]
                    for i in range(free_y+1, free_y+h+1):
                        obj=getObj(x, i)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE) :
                                buf.append([obj, x, i])
                    if(check):
                        for l in buf:
                            branches[0].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[0].append("True")
                    check=1
                    buf=[]
                    for i in range(free_y-h, free_y):
                        obj=getObj(x, i)
                        if(obj != which):
                            if((obj==WALL)):
                                check=0
                                break
                            elif(obj!=SPACE) :
                                buf.append([obj, x, i])
                    if(check):
                        for l in buf:
                            branches[1].append(getBranch(l[0], l[1], l[2]))
                        if(len(buf)==0):
                            branches[1].append("True")
                #print(f"{which}: {blocks[which]}| {branches[0]}, {branches[1]}")
                if((len(branches[0])+len(branches[1]))==0):
                    res="False"
                elif (len(branches[0])>0 and len(branches[1])>0):
                    res+=c_OR
                    res+=parseBranch(branches[0])+","
                    res+=parseBranch(branches[1])+")"
                else:
                    for i in range(2):
                        res+=parseBranch(branches[i])
                if(len(res)>1000):
                    ln=len(main_dots)
                    main_dots.append(Bool('main_dots[%s]' % ln))
                    s.add(eval(f'main_dots[{ln}]=={res}'))
                    res=f'main_dots[{ln}]'
                MyTestBlocks.append(blocks[which])
                BranchDict.update({dictbuf:res})
    if(res!="True") and (res!="False"):
        return str(simplify(eval(res)))
    return res

def some_foo():
    print("You should wait!")
    x,y,w,h,mv=blocks[target]
    qua=getBranch(target, x+w-1, y)
    s.add(eval(qua))
    print(s.check())
    flag=""
    for ind in important:
        var=eval(inp_dict[ind])
        bit=str(int(s.model()[var].__bool__()))
        flag+=bit
    Myprintflag(flag)


boardfile = open(sys.argv[1]).read()
header, boardfile = boardfile.split("\n", 1)
W, H = [int(x) for x in header.split()]
target = -1

walls = []
for line in boardfile.splitlines():
    if not line.strip(): continue
    x,y,w,h,movable = [int(x) for x in line.split()]

    if movable == -1: #add
        important.append(len(blocks)) 
    elif movable == -2:
        target = len(blocks)

    for i in range(x, x+w):
        for j in range(y, y+h):
            if movable != 0:
                if (i,j) in board:
                    print("Car overlap at %d, %d" % (i,j))
                    #assert False
                board[(i,j)] = len(blocks)
            else:
                if (i,j) in board and board[i,j] != WALL:
                    print("Wall-car overlap at %d, %d" % (i,j))
                    #assert False
                board[(i,j)] = WALL
    if movable==-1:#add
        board[(x,y-1)] = GREENHEAD
    if movable:
        blocks.append([x,y,w,h, movable])
    else:
        walls.append([x,y,w,h])

some_foo()
```

## Эпилог

Пока делал скрипт более красивым, освобождал от не нужных функций, случайно убрал `s.check()`. Запустил чтобы проверить, а он падает. Уже испугался, что пока писал райтап что-то накосячил в скрипте, и как следствие куда-то сюда тоже скинул кривой код, и придется его переписывать чуть ли не с нуля.
А еще, когда я переписывал код, я как-то пропустил `GREENHEAD`'ов, что снова заставило меня понервничать, ведь приводило к неверному флагу.
Да, в общем мне нельзя давать кодить и все, что с этим связано, если я, переписывая свой код, допускаю такие ляпы. 
**И простите меня за мой русский**
![](https://i.imgur.com/Ie5G5sX.png)