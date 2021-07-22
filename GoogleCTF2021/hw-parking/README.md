---
tags: writeup, hardware, z3, googlectf2021, english
---
# **Write-up Parking (Google CTF 2021)**
> [name=User1]
> [time=Thu, Jul 22, 2021 7:37 PM]
![](https://i.imgur.com/JoucNBT.jpg)


> [color=#1cefda] "It's dangerous to go alone! Take this [@ch4nnel1](https://t.me/ch4nnel1).

Original write-up: https://hackmd.io/@osogi/parking_writeup
Russian version: https://hackmd.io/@osogi/parking_writeup_ru


## Story + analysis
![](https://i.imgur.com/esugZLZ.png)
<p style="text-align: center;">This task(https://capturetheflag.withgoogle.com/challenges/hw-parking or https://github.com/google/google-ctf/tree/master/2021/quals/hw-parking)</p>

### Prologue 
Download the zip archive, open it, it contains 4 files

![](https://i.imgur.com/9XGq4Zc.png)

We will forget about `run.sh` almost immediately, because all it does is launch the game along with the first level and tell it that now it is not necessary to give the flag + just starts the second level. (I hope Google will not ban me for using the code from their task. There was just some kind of license and all that, but I was lazy to read it).
```bash=1
echo "Welcome to our game. You need to enter the red vehicle into a crowded parking lot."
echo "There are two levels: the first is introductory, just so you know the rules; the second, when solved, will reveal the flag. Good luck!"
FLAG=0 python3 game.py level1
echo "Good job! Now that you know the rules, let's move to the real level. Note that it is quite large, so allow some loading time."
python3 game.py level2
```
Well, of course, let's run the program, to start with the first level, we will see something like this

![](https://i.imgur.com/VbMdLHx.png)

From the text that the game broadcasts to us, we understand that we need to move the red car + that the final position of the green car (as it turns out later car**s**) encodes the flag. After spending five minutes, with difficulty I solved the first level (I will not give tips on how to solve it). We can see the following, we got a flag. 

![](https://i.imgur.com/MkHhcBC.png)

~~Ok Google, I expected something more from you. I know I’m a genius, but I didn't think, that I would solve the task in 5 minutes.~~ Ahh, the flag doesn’t right. 

![](https://i.imgur.com/qKBYXL0.png)


### Exposition (solution idea)

Okay, I was insolently deceived and palmed off the wrong flag. Let's try to open the second level.  Even it takes a long time to load ...

![](https://i.imgur.com/YrMtxzi.png)

~~Oh my god, can I go out the window? After that, no one saw the author of the write-up~~  All right, maybe not everything is so bad there, and there is already an empty cell next to the red car? 
(After I couldn't find the red car) Every normal person would ask such a question, but I am a mega reverser immediately got into the source code of the game.

There could be a part about what I was stupid and trying to change the coordinates of one green car to get a flag. But that won't apply to the current solution, so I'll just mention it.

Having slightly analyzed the code + poking through the level files.

![](https://i.imgur.com/IUUULon.png)

Let's roughly understand the structure of the levels: the first line is the size of the field, then there are objects in the format `x, y, width, height, type` (if type: 0 - wall; 1 - normal, gray car; -1 - green car; -2 - red car).
The game itself is written in python and everything is quite clear in it, but I will note some useful things that will be used in the further solution.

```python
board #dictionary matching: (x, y) -> the object at this position
blocks #list matching "number", car index with its x, y, width, height, type
target #variable containing the number of the red car 
def redraw(): #the function responsible for redrawing the field + on the first run saves the result of the initial position to a file 
def printflag(): #function that prints the flag, the output depends on the position of the green cars (if they changed position, then the flag bit corresponding to this car will be 1, otherwise 0) 
```

Knowing how the structure of the level file works, we will find in the figure red and green cars.

![](https://i.imgur.com/Nwpzh7n.png)
<p style="text-align: center;">In the green sections (there are 64 of them) there are green cars, the red arrow (lower right corner) is pointing to the red car</p>

Let's zoom to see it all closer

![](https://i.imgur.com/hFaKgaE.png)

![](https://i.imgur.com/J6TFqN1.png)

![](https://i.imgur.com/QPgNgZI.png)

Okay, you know what? This all resembles logic circuits (+ hw after all). From the liveoverflow's video I remembered that they can be solved through z3 (mega solver of equations, inequalities and etc.), but I did not know how, so I decided to revise it.

{%youtube nI8Q1bqT8QU %}
> https://www.youtube.com/watch?v=nI8Q1bqT8QU

P.S. As a result, I didn't even use it, but the video is still cool and useful. By the way, it is also about hardware Google ctf, only 2019.


#### Mini-task 1 
Let's look at the simplest tasks with cars and understand our actions.

![](https://i.imgur.com/8ExCG8a.png)

We need to move the red car, for this we: 
1) we determine its possible directions of movement (horizontally / vertically).
2) where it can advance in theory, that is, where it is not the wall that prevents it from moving.
3) if a car prevents it from moving there, repeat everything for this car 

Using such an algorithm, we can solve this task.

![](https://i.imgur.com/FtAdqho.png)

#### Mini-task 2

Let's look at a more difficult task

![](https://i.imgur.com/C0pd3RB.png)

For it, we will have to slightly modify our algorithm, because if we just move the orange car to the right (as it should be according to the previous algorithm), then it will not clear the way. Therefore, we add 1 step to our algorithm
1) we determine its possible directions of movement (horizontally / vertically).
2) where it can advance in theory, that is, where it is not the wall that prevents it from moving.
3) **see what prevents it from moving to the "possible position".** 
4) **repeat for interfering machines, passing them the position to be cleared.** 

![](https://i.imgur.com/SrGq1h9.png)

#### Mini-task 3

And consider the last example, in which we will discuss one subtlety of the algorithm.

![](https://i.imgur.com/T1LuWX1.png)

In step 4, we have to do logical disjunction ('or', '|')  for all the results of possible positions (this task will only contain 1-2).And logical conjunction ('and', '&') within the results themselves. 

> Better to explain with an example. For example, in mini-task 2, in order to move the red car, as a result, we need to move the blue **AND** purple car. Right there (mini-task 3), in order to move the red car we need to move the green **OR** blue one.

![](https://i.imgur.com/jsToXce.png)

Uhhh, all that's left is to code this and give to z3.

### Climax (coding)

>  Most of my bad are omitted in this part, as there were too many of them.

#### some_foo()
First, I'll make a function `some_foo()`, and place a call to it before drawing the initial field. 

```python
def some_foo():
    print("Yep i'm work")
    print(blocks[target])
```

Boom, it works.

![](https://i.imgur.com/0Th8v7A.png)

#### get_obj(x, y)
Let's add:
- function `get_obj(x, y)`, which will return what is at position `x, y` 
- a function defining (`isHorizintal(which)`) how the car can move (horizontally / vertically) 
- an `important` array that will contain the indices of the green cars.
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
Looks working 

![](https://i.imgur.com/vlibuvy.png)
#### getMoveDir
> There have been many attempts to add / modify the `getMoveDir ()` function, but let's pretend that it was written correctly the first time 
Let's add some bad code and a function that will "test" our algorithm 

- `getMoveDir(which, free_x, free_y)`  - a recursive function that determines where the car can move
- `MvDict` - dictionary containing "machine index" -> direction 
- `MyTestBlocks` - list containing numbers of "parsed" cars 
- `putparsecar(x,y,w,h,m)` - the function that paints the cars from `MyTestBlocks` in blue, made to understand where `getMoveDir()` breaks
- also added `sys.setrecursionlimit(150000)`, so that python doesn't break when parsing the second level
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
Checking at level 1: 

![](https://i.imgur.com/07RhMHm.png)

Checking at level 2 (had to be transferred to a file for more convenient analysis), reached all (64) green machines - success: 

![](https://i.imgur.com/DIJCY5l.png)

#### getBranch
> Many attempts have been made here to get the `getBranch()` function to work properly, but I will  talk about only one of them. 

So now you need to rewrite the `getMoveDir()` function in `getBranch()`, changing its functionality to get under what conditions (with respect to green cars) the machine can free the given coordinates.

For this was added: 
- `inputs` - list with variables (associated with green cars) for z3 
- `inp_dict` - a dictionary that links the "green car index" -> its corresponding `inputs[i]`
- `BranchDict` - replacement of `MvDict` + was a bug. To eliminate infinite recursions, I forbade finding new directions for cars, which are in `MvDict` -> (see the picture) when the purple machine told blue to move, it "blocked" access to it. And after the yellow car thought that the blue == wall. And as a consequence, `BranchDict` matches not just `which`, but `f" {which}_{free_x}_{free_y} "` with the result of `getBranch(which, free_x, free_y)`

![](https://i.imgur.com/XjINgsA.png)

- function `get_green_by_head()` and workaround `GREENHEAD` - it marks the square above the green car to understand that the car can move into it only if the green one does not move 
- the `parseBranch` function is applied to a branch and if it has several conditions, it connects them using AND 
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
Yuhu, it was able to solve the first level: 

![](https://i.imgur.com/8B7YvGl.png)

But with the second, some problems occur (of course, if at first your computer is not dead from the amount of garbage that the script throws out): 

![](https://i.imgur.com/2bP4VDA.png)


#### z3.simplify()

Ok, maybe it should have been done at beginning, but I just thought of it now - add `simplify()`. Deal with everything at once. Just condition on `True`,` False` been added, otherwise it breaks.

```python

def getBranch(which, free_x, free_y):
    [...]
    if(res!="True") and (res!="False"):
        return str(simplify(eval(res)))
    return res
```

**...**
I'll just keep silent, and note that now the program crashes due to the fact that `eval` cuts long lines (~10k characters) using `...` and z3 breaks when trying to parse them. 

![](https://i.imgur.com/pYIFzrT.png)

#### main_dots

As a result, it was decided to create such a thing as a list of control points `main_dots`, and write there z3's variables that would be equal to formulas, the length of which is more than 1000. 

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
Launch it, wait and get the necessary bits (`0100110000101010000101101010011001000010010011101000110010110110`).

![](https://i.imgur.com/A0ewZ9M.png)

### Resolution (getting the flag) 

Go to [cyberchef](https://gchq.github.io/CyberChef/). We shove the bits ... 

![](https://i.imgur.com/yot5z5u.png)

And we don't get the flag. ~~Dead end.~~ Let's start to reverse ~~engineering~~

![](https://i.imgur.com/nw1FWLN.png)

Ooh, understandable letters, only meaningless, but okay, let's try to pass CTF{m1rBehT2}. 

![](https://i.imgur.com/JXAzGvo.png)

Doesn't pass( Let's try to reverse further

![](https://i.imgur.com/D7dnzoP.png)

Uh-huh, it sounds and looks cool,  pass CTF{2TheBr1m} 

![](https://i.imgur.com/1efPfPY.png)

~~Easy game, easy life~~ Easy flag, hard CTF

## Solution 

- understand that there is no way to cheat and what needs to be solved level2
- understand that level2 is like a logic circuit -> can be solved by z3 
- convert level2 to logic circuit
- give it to z3
- get flag bits -> get flag 

## Solver script

Usage: `python3 solver.py level2` and go and have some tea (wait about 10-20 minutes or more). 

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

## Epilogue

While making the script more beautiful, freed from unnecessary functions, accidentally removed `s.check()`. I started it to check, but it crashes. I was already panic that while I was writing the write-up I messed up something in the script, and as a result I also put the crooked code somewhere here, and I would have to rewrite it almost from scratch.
And also, when I was rewriting the code, I somehow missed the `GREENHEAD`s, which again made me nervous, because it led to the wrong flag. 
Yes, in general I shouldn't be allowed to code and everything connected with it, if I make such mistakes while rewriting my code.
**Sorry me for my english**
![](https://i.imgur.com/Ie5G5sX.png)
