# -*- coding: utf-8 -*-
from pwn import *
import re

def rev_in(arr):
    print(arr)
    if (arr[0]==b'foo'):
        if(arr[1]==b'?'):
            esi=int(arr[2])
            edx=int(arr[3])
            edx^=0x539
            esi+=1
            return esi^edx
        if(arr[2]==b'?'):
            esi=int(arr[1])
            edx=int(arr[3])
            edx^=0x539
            edx^=esi
            return edx-1
        if(arr[3]==b'?'):
            esi=int(arr[1])
            edx=int(arr[2])
            esi^=0x539
            edx+=1
            return edx^esi
    else:
        if(arr[1]==b'?'):
            deli=int(arr[2])
            deli2=int(arr[3])+1
            vse=int(arr[4])
            return vse-(deli*deli2)
        if(arr[2]==b'?'):
            ost=int(arr[1])
            deli=int(arr[3])+1
            vse=int(arr[4])
            return ((vse-ost)/deli)
        if(arr[3]==b'?'):
            ost=int(arr[1])
            deli=int(arr[2])
            vse=int(arr[4])
            return ((vse-ost)/deli)-1
        if(arr[4]==b'?'):
            esi=int(arr[1])
            edx=int(arr[2])
            ecx=int(arr[3])
            ecx+=1
            edx*=ecx
            return edx+esi

unic=[]
while (1==1):
    count=0
    tme=int(time.time())
    strme=""
    r = process('/problems/2021/infinity_gauntlet/infinity_gauntlet')
    #r = process('/home/user1/Загрузки/infinity_gauntlet_patch')
    r=remote("shell.actf.co", 21700)
    print(r.recvline())
    while(1==1):
        count+=1
        buf=r.recvline()
        print(buf)
        if(buf==b'Wrong!\n'):
            break
        print(r.recvline())
        buf=r.recvline()
        print(buf)
        buf=buf.replace(b'=', b',').replace(b'(', b',').replace(b')', b'').replace(b' ', b'').replace(b'\n', b'')
        buf=re.split(b',', buf)
        res=int(rev_in(buf))
        if(count==1):
            strme=str(res)
        print(str(res).split('.')[0])
        r.sendline(str(res).split('.')[0])
        if (count>0x31):
            unic.append(hex(res)[-2:])
            print(unic)
            print(tme)
            print(strme)

    r.interactive()



