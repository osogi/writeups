a=input()
a=a.split(' ')
good=""
for l in a:
    buf=l[-2:]
    if(len(buf)==0):
        continue
    if(buf[0]=='x'):
        buf="0"+buf[1]
    good+=buf

print(good)
    