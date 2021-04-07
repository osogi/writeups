import r2pipe

path_to_bin="/home/user1/Загрузки/infinity_gauntlet"
r = r2pipe.open( path_to_bin, flags=['-d'])
r.cmd("db main+0xc4") #for changing "time"
r.cmd("db main+0x1bb") #for checking ebx
print(r.cmd("db")) #checking bp
check=False
time=1617479350
ebx=11370

while not check:
    time+=1
    r.cmd("dc") #go to main+0xc4
    r.cmd("dr rax="+str(time)) #set "time"
    r.cmd("dc") #go to main+0x1bb
    buf=int(r.cmd("dr?ebx"), 16) #get ebx
    #print(buf)
    if(buf==ebx):
        check=True
    else:
        r.cmd('dr rip=main+0xbf') #jmp to instruction before main+0xc4
print(time)

