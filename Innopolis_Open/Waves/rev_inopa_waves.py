import wave
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import math

types = {
    1: np.int8,
    2: np.int16,
    4: np.int32
}


wav = wave.open('/home/user1/Загрузки/output.wav', mode="r")
(nchannels, sampwidth, framerate, nframes, comptype, compname) = wav.getparams()

duration = nframes / framerate
DPI = 72
peak = 256 ** sampwidth / 2

content = wav.readframes(nframes)
samples = np.fromstring(content, dtype=types[sampwidth])
n=len(samples)
''' 
#часть кода для поиска громкких кадров
big=[]
for i in range(n):
    buf="["+str(i)+"]: "+ str(samples[i])
    print(buf)
    if(abs(samples[i])>100):
        big.append(buf)
print("Interesting places:")
for i in big:
    print(i)
'''
key_restore=32764
wor=0
i=1337


key=[]
if (nchannels==2):
    i*=2
    while (wor!=32) and (len(samples)>i):

        buf=samples[i]*1000
        print(samples[i])
        key.append(int(round(buf/key_restore)))
        print(hex(key[-1]))
        i+=45887*2
        if(wor%2==0):
            i+=1
        else:
            i-=1
        wor+=1
else:
    while (wor!=32) and (len(samples)>i):
        buf=samples[i]*1000
        print(samples[i])
        key.append(int(round(buf/key_restore)))
        print(hex(key[-1]))
        i+=45887
        wor+=1

res=""
for i in key:
    buf=hex(i)[2:]
    if(len(buf)==1):
        buf='0'+buf
    res+=buf

print(res)

print("End")