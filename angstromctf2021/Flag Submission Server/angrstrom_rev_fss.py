# -*- coding: utf-8 -*-
from pwn import *
import r2pipe

def get_second_check_val(arg):
    #print("[arg]: "+ hex(arg))
    r2.cmd("dr rip=main+0xa47")
    r2.cmd("dr rdi="+str(arg))
    r2.cmd("dcu main+0xa4c")
    #print("[reg]: "+r2.cmd("dr"))
    return int(r2.cmd("dr?rax"), 16)


def send_pac(data, first_check_val, second_check_val):
    len_p=22
    #print("[first_check_val]: "+str(first_check_val))
    arr=[0xde,0xad,0xbe,0xef,0xfe,0xed,0xca,0xfe,0x13,0x37,0xab,0xcd, 0xef]
    packet=b""
    for i in range(len(first_check_val)):
        packet+=bytes([first_check_val[i]^arr[i]])
    packet=(second_check_val).to_bytes(8, 'little')+data+packet
    packet=packet+b"2"*(len_p-len(packet))
    #print("[packet]: "+str(packet))
    r.send(packet)



path_to_bin='/home/user1/Загрузки/fss_patch'
r2 = r2pipe.open(path_to_bin, flags=['-d'])
r2.cmd("dcu main")

remote_p=1
if(remote_p==1):
    r=remote("rev.2021.chall.actf.co", 21450)
    first_check_val=r.recv(timeout=1)+r.recv()
else:
    r=remote("127.0.0.1", 21450) #if you want to test script local, first launch server (fss) 
    first_check_val=r.recv(timeout=1)
save="actf{"
good=" "
second_check_val=get_second_check_val(0x13371337)
counter=0
while(good[-1]!='}'):
    for char in range(32, 126):
        if(counter<len(save)):
            char=ord(save[counter])
        send_pac(char.to_bytes(1, 'little'), first_check_val, second_check_val)
        buf=r.recv(timeout=1)
        if(buf==b'%'):
            second_check_val=get_second_check_val(int.from_bytes(r.recv(), 'big'))
            good+=chr(char)
            break
        elif(buf!=b'\r'):
            print("[error_buf]: "+str(buf))
    counter+=1
    print("[flag]: "+ good)











