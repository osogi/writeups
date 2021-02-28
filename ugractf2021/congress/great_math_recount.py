import requests
import urllib
import sys
import time

seq=[]

def game():
    leng=30
    for i in range(60*2):
        x=abs(2*(i%leng)-i%(2*leng))
        time.sleep(1)
        print("#"+" "*x +"0"+ (leng-x)*" "+"$", end="\r")

def save():
    f=open('saveconf', 'w')
    f.write('seq='+str(seq)+"\n\n")
    f.write('pins='+str(pins)+"\n\n")
    f.close()
    print("[+]: Progress saved")

def load():
    global seq
    global pins
    print("[+]: Progress loading")
    f=open('saveconf', 'r')
    buf=f.readlines()
    seq=[n[1:] for n in buf[0][5:-2].replace('\\n', '\n').split("', ")]
    seq[-1]=seq[-1][:-1]
    pins=[int(n) for n in buf[2][6:-2].split(", ")]
    print_inf()

def sort_col(i):
    return i[1]

def unic_seq(s):
    if(seq.count(s)==0):
        seq.append(s)
        buf=get_seq(s)
        ban(pins, buf)
    


def ban( dlist, banlist):
    for n in banlist:
        if(n<10000):
            dlist[n]+=1
    
def get_seq(desc):
    #data_f={'q': desc}
    url='https://oeis.org/search?q='+urllib.parse.quote_plus(desc)+'&fmt=data'
    s = requests.Session()
    buf=requests.get(url)
    buf=buf.text
    st=  buf.find('<tt>')+4
    end=  buf.find('</tt>')
    buf=buf[st: end]
    buf=buf.split(',')
    print(buf)
    res=[]
    for n in buf:
        try:
            if(999<int(n)<10000):
                res.append(int(n))
        except:
            print(n)
    return res

def parse_file(path='/home/user1/myscripts/vzlom'):
    f=open(path,'r')
    for s in f:
        if(len(s)>3):
            unic_seq(s)
            print_inf()
            save()
    print("[+]: File parsed")
    f.close()


def try_num(q):
    url='https://congress.q.2021.ugractf.ru/check?ref=8e6d61b64e9459b071633bbcbe6706ce&q='+str(q)
    s = requests.Session()
    buf=s.get(url).json()
    return buf

def print_inf():
    print("\n\n\n")
    print("######- New inf -#######")
    zero_c=0
    good=[]
    minban=min(pins)
    minban=pins.count(minban)
    for i in range(1000, 9999+1):
        good.append([i, pins[i]])
    good.sort(key=sort_col)
    for i in range(10):
        print(good[i])
    print("[...]")
    good=good[::-1]
    for i in range(10):
        print(good[9-i])
    print("with_min_ban: "+str(minban))
    print("unic_sequence: "+str(len(seq)))



pins=[]
for i in range(0, 1000):
    pins.append(9)
for i in range(1000, 9999+1):
    pins.append(0)
#parse_file()
load()
ok=True
while ok:
    min_ind=pins.index(min(pins))
    print("[+]: Trying to conect with pin "+str(min_ind))
    buf=try_num(min_ind)
    #print(buf)
    if(buf['status']=='ok'):
        try:
            count=buf['count']
            buf=buf['items']
            for i in buf:
                unic_seq(i['desc'])
                print_inf()
            save()
            if(count<=5):
                print(min_ind) #из-за отсутствия этой функции в первой версии пострадала моя клавматура
                print(buf)
                ok=False
        except:
            print("[-]: Error- "+str( sys.exc_info()))
            print(buf)
    else:
        print("[/]: I'm waiting...")
        game()
