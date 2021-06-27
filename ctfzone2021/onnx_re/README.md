---
tags: writeup, rev, ai, ctfzone2021
---
# **Write-up onnx_re (CTFZone 2021)**
> [name=User1]
> [time=Sun, Jun 27, 2021 5:13 PM]



![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/1.jpg)

> [color=#1cefda]Так как никто до сих пор не купил у меня рекламы, я просто прорекламирую мой канал [@ch4nnel1](https://t.me/ch4nnel1) в райтапах от этого же канала.

Original write-up: https://hackmd.io/CeWJhxjFRMuHI1uOzgkocw


## Разбор
![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/2.png)
<p style="text-align: center;">Сам таск (https://ctf.bi.zone/challenges/7)</p>

Скачиваем прикрепленный файл, это 7z архив со следующими файлами
![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/3.png)

Установив все необходимое с помощью команды `python3.7 -m pip install -r requirements.txt`, изучим питон код
```python=0
import numpy as np
from onnx import load
from onnx.onnx_ONNX_REL_1_7_ml_pb2 import ModelProto
from onnxruntime.backend import prepare

def run_model(data: bytearray, model: ModelProto):
    """Run model and check result"""
    x = np.array(data, dtype=np.int64)
    i = np.array([0], dtype=np.int64)
    keep_going = np.array([True], dtype=np.bool)
    max_index = np.array([32], dtype=np.int64)
    model_rep = prepare(model)
    out = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
    assert np.array_equal(
        out,
        np.array( [16780, 9831, ... 6214, 7169], dtype=np.int64,),
    )
    print(f"Flag: ctfzone\x7b{data.decode()}\x7d")

if __name__ == "__main__":
    data = input("Enter your password 32 bytes long: ")
    if len(data) != 32:
        exit("Invalid length")
    model = load("model.onnx")
    try:
        run_model(bytearray(data.encode()), model)
    except AssertionError:
        print("Wrong password")
```
Из кода видим, что нам нужно ввести какие-то 32 символа, которые после обработки нейронкой преобразуются в массив на 30 интов, и получившийся массив должен совпасть с данным нам.

Введем в код новую функцию `run_model_test`, кторая пока только выводит массив полученный от нейронки, и поставим ее на запуск сразу после загрузки модели
```python=0
def run_model_test(model: ModelProto):
    data=bytearray(("1"*32).encode())
    x = np.array(data, dtype=np.int64)
    i = np.array([0], dtype=np.int64)
    keep_going = np.array([True], dtype=np.bool)
    max_index = np.array([32], dtype=np.int64)
    model_rep = prepare(model)
    ob = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
    print(ob)
```
Чуть потыкавшись и поигравшись с результирующим массивом
![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/4.png)

Доработаем нашу функцию `run_model_test` так, чтобы мы могли нагляднее увидеть зависимоть выхлопа ии от предоставленных данных. Для этого посчитаем результат ии от неких 32 байт, после будем менять по 1 байту и сравнивать новый результат с изначальным.
```python=0
def run_model_test(model: ModelProto):
    data=bytearray(("1"*32).encode())
    x = np.array(data, dtype=np.int64)
    i = np.array([0], dtype=np.int64)
    keep_going = np.array([True], dtype=np.bool)
    max_index = np.array([32], dtype=np.int64)
    model_rep = prepare(model)
    standard = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
    res=[]
    for c in range(0, 32):
        inp="1"*c+"2"+"1"*(31-c)
        data=bytearray((inp).encode())
        x = np.array(data, dtype=np.int64)
        i = np.array([0], dtype=np.int64)
        keep_going = np.array([True], dtype=np.bool)
        max_index = np.array([32], dtype=np.int64)
        model_rep = prepare(model)
        out = model_rep.run([x, i, keep_going, max_index])[1].reshape((1, 30))[0]
        buf=(out-standard)
        res.append(buf)
        print(str(c)+": "+str(buf))
```
После запуска этого кода, уже можно понять что `data[i]` влияет на `out[i-2:i+1]`
![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/5.png)

Сделав еще один запуск, но в этот раз с "0" вместо "1" и "1" вместо "2" (1 и 10 строчка функции `run_model_test`), получим такой же результат как и от предыдущей версии скрипта => нейросетка просто делает `out[i]=data[i]*x+data[i+1]*y+data[i+2]*z+b`, где `x`, `y`, `z`, `b` - некие коэфиценты. `x`, `y` и `z` мы уже выясняли с помощью предыдущего скрипта. Осталось выяснить только `b`, сделаь это несложно, все что нужно это дать на вход `data=b"\x00"*32`. Сделав это, получим что все `b` равны 0.

Все что осталось сделать это решить систему линейных уравнений, я это сделал [доработав скрипт](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/solution/solution.py) с помощью z3.

![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/6.png)

## Решение 

- Понять зависимость out'а нейронки от data поданной на вход (я не знаю как еще расписать этот шаг)
- Составить систему линейных уравнений (желательно с помощью скрипта)
- Решить эту систему

[Скрипт решение](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/solution/solution.py)

## Эпилог

Так же в чате ctfzone при обсуждение этого таска всплывали такие ссылки https://github.com/onnx/onnx-mlir/ и https://netron.app/, говорят они как раз могут помочь при реверсе нейронок. Первую ссыль я особо не тыкал, но открыв сетку во второй я запутался, испугался и закрыл.
![](https://github.com/osogi/writeups/tree/main/ctfzone2021/onnx_re/attachments/7.png)

Так же я наверное буду переходить с https://telegra.ph на https://hackmd.io, так как в первом нельзя простым способом красиво вставить код, + markdown -> можно безпроблемотично переносить на другие сайты и тд.