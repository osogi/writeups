#!/usr/bin/env python
# -*- coding: utf-8 -*-

import rzpipe
import pwn

lenread=0x40
keyval=1  # must be 0 or 1
endian='little'
bo='big'
test=0
flag_path=b"./flag.txt\0"
file_path='/home/user1/Загрузки/server'

class Crypter:

    def __init__(self, file_name):
        rflags = ["-dw"]
        if(not test):
            rflags.append("-2")
        r = rzpipe.open(file_name, flags=rflags)
        for _ in range(5):
            r.cmd("dcs read")
        
        a=r.cmd("dr rip")
        a = int(a, 16)
        self.off = a-0x12EE
        self.r=r

        self.encode_addr=0x2E6F+self.off
        self.encode_addr_end = self.encode_addr+0x4d8

        self.decode_addr=0x3894+self.off
        self.decode_addr_end=self.decode_addr+0x4ef

        self.keyfun_addr=0x1342+self.off
        self.keyfun_addr_end=0x13C7+self.off

        m =0x9038+self.off
        self.mem_addr=int(r.cmdj(f"?j [{m}]")["uint64"])
    
    def encrypt(self, message, key):
        r=self.r
        
        r.cmd(f"dr rip={self.encode_addr}")
        r.cmd(f"dr rcx=0x10") #len
        
        dest_addr = self.mem_addr+0x1000
        r.cmd(f"dr rdx={dest_addr}")

        key_addr = self.mem_addr+0x1100
        r.cmd(f"dr rsi={key_addr}")

        hexkey=key.to_bytes(16, byteorder=bo).hex()
        r.cmd(f"wx {hexkey} @ {key_addr}")

        mes_addr = self.mem_addr+0x1200
        r.cmd(f"dr rdi={mes_addr}")

        hexmes=message.hex()
        r.cmd(f"wx {hexmes} @ {mes_addr}")
        r.cmd(f"dcu {self.encode_addr_end}")

        b = r.cmd(f"p8 16 @ {dest_addr}")
        return bytes.fromhex(b)


    def decrypt(self, message, key):
        r=self.r
        
        r.cmd(f"dr rip={self.decode_addr}")
        r.cmd(f"dr rcx=0x10") #len
        
        dest_addr = self.mem_addr+0x1000
        r.cmd(f"dr rdx={dest_addr}")

        key_addr = self.mem_addr+0x1100
        r.cmd(f"dr rsi={key_addr}")

        hexkey=key.to_bytes(16, byteorder=bo).hex()
        r.cmd(f"wx {hexkey} @ {key_addr}")

        mes_addr = self.mem_addr+0x1200
        r.cmd(f"dr rdi={mes_addr}")

        hexmes=message.hex()
        r.cmd(f"wx {hexmes} @ {mes_addr}")
        r.cmd(f"dcu {self.decode_addr_end}")

        b = r.cmd(f"p8 16 @ {dest_addr}")
        return bytes.fromhex(b)


    def get_key(self, a=1):
        r=self.r
        
        r.cmd(f"dr rip={self.keyfun_addr}")

        r.cmd(f"dr rsi={a}")

        key_addr = self.mem_addr+0x1100
        r.cmd(f"dr rdi={key_addr}")

        r.cmd(f"dcu {self.keyfun_addr_end}")

        b = r.cmd(f"p8 16 @ {key_addr}")
        return int(b, 16)

    
def encrypt_mes(crypter, pack):
    if(test):
        print(f"pack len = {len(pack)}")
    pack+=b'\x00'*(32-len(pack))
    if(test):
        print(f"new pack len = {len(pack)}")

    sendp = crypter.encrypt(pack[0:16], key)
    sendp+= crypter.encrypt(pack[16:32], key)

    return sendp


def decrypt_flag(crypter, a):
    a+=(0x10-len(a)%0x10)*b"0"
    res=b""
    for i in range(len(a)//0x10):
        res+=crypter.decrypt(a[i*0x10:(i+1)*0x10], key)
    return res



cryp = Crypter(file_path)
key=cryp.get_key(keyval)

if(test):
    print(f"offset: {hex(cryp.off)}")
    enc = cryp.encrypt(b'1\x03)g\x8a\xe3\x9d\xa1\xc7\xdd\xe0}\x94\xa6>\x9b', 0x3452dcf0a7b37e282442cce0b7a36e38)
    print(f"encrypted: {enc}")
    dec = cryp.decrypt( b'123456789\x00\x00\x00\x00\x00\x00\x00', 0x3452dcf0a7b37e282442cce0b7a36e38)
    print(f"decrypted: {dec}")
    print(f"key: {key}")




conn = pwn.remote("185.66.86.54", 17001)
#conn = pwn.process(file_path)


conn.send((keyval).to_bytes(8, endian)) # send data for s3cure key gen
conn.recv(8) # get some trash bytes

head = b"\x22\x33"

# Open file
com=b"\x01"
leng = len(flag_path).to_bytes(4, endian)
pack=head+com+leng+flag_path

sendp=encrypt_mes(cryp, pack)
conn.send(sendp)


# Read file
com=b"\x02"
pad=(0).to_bytes(4, endian) # not more 0x18
flags=b"\x00\x00"
leng = (lenread).to_bytes(4, endian)
pack=head+com+pad+flags+leng

sendp=encrypt_mes(cryp, pack)
conn.send(sendp)


# Close file
com=b"\x03"
pack=head+com

sendp=encrypt_mes(cryp, pack)
conn.send(sendp)

# Get readed bytes
com=b"\x04"
secret=b"\xca\xdb\x88\x2d"
leng = len(secret).to_bytes(4, endian)
pack=head+com+leng+secret

sendp=encrypt_mes(cryp, pack)
conn.send(sendp)


a=conn.recv() # Get encryptesed bytes of flag
flag = decrypt_flag(cryp, a)
print(flag)
