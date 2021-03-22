import r2pipe

path_to_VM="/home/user1/Загрузки/sprush/2(asembly)/ASSembly"
path_to_bytecode="/home/user1/Загрузки/sprush/2(asembly)/kek"
order = [ 0x0B, 0x07, 0x18, 0x05, 0x00, 0x17, 0x1B, 0x03, 0x06, 0x09, 0x0F, 0x19, 0x12, 0x1A, 0x0C, 0x08, 0x0D, 0x01, 0x02, 0x11, 0x1E, 0x1D, 0x0E, 0x10, 0x04, 0x15, 0x13, 0x14, 0x1C, 0x1F, 0x0A, 0x16 ]
flag="0123456789qwertyuiopasdfghjklzxc"
good=bytearray(32)
with open('profile.rr2', 'w+') as f:
        f.write('#!/bin/rarun2\nstdin="'+flag+'"\narg1="'+ path_to_bytecode +'"') #указываем аргументы и инпут
r = r2pipe.open( path_to_VM, flags=['-d','-e', 'dbg.profile=profile.rr2'])

r.cmd("db 0x00401ec8") #ставим брейкпоинт на compare
counter=0

while counter!=32:
    r.cmd("dc") #продолжаем исполнение программы
    xored_val=int(r.cmd("p8 1 @ rbp-0x58"), 16) #получаем проксоренный байт нашего инпута
    res=int(r.cmd("p8 1 @ rbp-0x5c"), 16) #получаем чему он должен равняться
    r.cmd("dr rip=0x00401ecd") #перепрыгиваем jump, чтобы программа не завершилась
    good[order.index(counter)]=ord(flag[order.index(counter)])^res^xored_val #считаем байт флага
    counter+=1
    print(good)
print(good)

